
#include "demoph/audio/advanced_audio_engine.h"
#include "demoph/core/log.h"
#include "demoph/utils/profiler.h"
#include "demoph/utils/math.h"
#include <algorithm>
#include <cstring>

namespace Demoph {

AdvancedAudioEngine::AdvancedAudioEngine()
    : m_isInitialized(false)
    , m_masterVolume(1.0f)
    , m_listenerPosition(Vec3::Zero())
    , m_listenerVelocity(Vec3::Zero())
    , m_listenerOrientation{Vec3::Forward(), Vec3::Up()}
    , m_dopplerFactor(1.0f)
    , m_speedOfSound(343.3f)
    , m_distanceModel(DistanceModel::InverseDistance) {
}

AdvancedAudioEngine::~AdvancedAudioEngine() {
    Shutdown();
}

bool AdvancedAudioEngine::Initialize(const AudioConfig& config) {
    DEMOPH_PROFILE_FUNCTION();
    
    if (m_isInitialized) {
        DEMOPH_LOG_WARNING("Audio engine already initialized");
        return true;
    }
    
    m_config = config;
    
    // Initialize OpenAL
    if (!InitializeOpenAL()) {
        DEMOPH_LOG_ERROR("Failed to initialize OpenAL");
        return false;
    }
    
    // Initialize audio processing
    InitializeAudioProcessing();
    
    // Initialize effects system
    InitializeEffectsSystem();
    
    // Initialize streaming system
    InitializeStreamingSystem();
    
    // Initialize 3D audio
    Initialize3DAudio();
    
    // Initialize reverb system
    InitializeReverbSystem();
    
    // Start audio thread
    StartAudioThread();
    
    m_isInitialized = true;
    DEMOPH_LOG_INFO("Advanced Audio Engine initialized successfully");
    DEMOPH_LOG_INFO("Audio Device: {}", GetDeviceName());
    DEMOPH_LOG_INFO("Sample Rate: {} Hz", m_config.sampleRate);
    DEMOPH_LOG_INFO("Buffer Size: {} samples", m_config.bufferSize);
    DEMOPH_LOG_INFO("Max Sources: {}", m_config.maxSources);
    
    return true;
}

void AdvancedAudioEngine::Shutdown() {
    DEMOPH_PROFILE_FUNCTION();
    
    if (!m_isInitialized) {
        return;
    }
    
    // Stop audio thread
    StopAudioThread();
    
    // Shutdown subsystems
    ShutdownReverbSystem();
    ShutdownStreamingSystem();
    ShutdownEffectsSystem();
    ShutdownAudioProcessing();
    ShutdownOpenAL();
    
    // Clear resources
    m_audioSources.clear();
    m_audioBuffers.clear();
    m_streamingSources.clear();
    
    m_isInitialized = false;
    DEMOPH_LOG_INFO("Advanced Audio Engine shutdown complete");
}

void AdvancedAudioEngine::Update(float deltaTime) {
    DEMOPH_PROFILE_FUNCTION();
    
    if (!m_isInitialized) {
        return;
    }
    
    // Update streaming sources
    UpdateStreamingSources(deltaTime);
    
    // Update 3D audio calculations
    Update3DAudio(deltaTime);
    
    // Update audio effects
    UpdateAudioEffects(deltaTime);
    
    // Update reverb system
    UpdateReverbSystem(deltaTime);
    
    // Process audio callbacks
    ProcessAudioCallbacks();
    
    // Update statistics
    UpdateStatistics(deltaTime);
}

AudioSource* AdvancedAudioEngine::CreateAudioSource(const AudioSourceDesc& desc) {
    DEMOPH_PROFILE_FUNCTION();
    
    if (m_audioSources.size() >= m_config.maxSources) {
        DEMOPH_LOG_WARNING("Maximum number of audio sources reached");
        return nullptr;
    }
    
    auto source = std::make_unique<AudioSource>(*this, desc);
    if (!source->Initialize()) {
        DEMOPH_LOG_ERROR("Failed to initialize audio source");
        return nullptr;
    }
    
    AudioSource* sourcePtr = source.get();
    m_audioSources.push_back(std::move(source));
    
    DEMOPH_LOG_DEBUG("Created audio source: {}", desc.name);
    return sourcePtr;
}

void AdvancedAudioEngine::DestroyAudioSource(AudioSource* source) {
    auto it = std::find_if(m_audioSources.begin(), m_audioSources.end(),
        [source](const std::unique_ptr<AudioSource>& s) {
            return s.get() == source;
        });
    
    if (it != m_audioSources.end()) {
        DEMOPH_LOG_DEBUG("Destroyed audio source");
        m_audioSources.erase(it);
    }
}

AudioBuffer* AdvancedAudioEngine::LoadAudioFile(const std::string& filepath) {
    DEMOPH_PROFILE_FUNCTION();
    
    // Check if already loaded
    auto it = m_audioBuffers.find(filepath);
    if (it != m_audioBuffers.end()) {
        return it->second.get();
    }
    
    // Load audio file
    auto buffer = std::make_unique<AudioBuffer>();
    if (!buffer->LoadFromFile(filepath)) {
        DEMOPH_LOG_ERROR("Failed to load audio file: {}", filepath);
        return nullptr;
    }
    
    AudioBuffer* bufferPtr = buffer.get();
    m_audioBuffers[filepath] = std::move(buffer);
    
    DEMOPH_LOG_INFO("Loaded audio file: {}", filepath);
    return bufferPtr;
}

AudioBuffer* AdvancedAudioEngine::CreateAudioBuffer(const AudioData& data) {
    DEMOPH_PROFILE_FUNCTION();
    
    auto buffer = std::make_unique<AudioBuffer>();
    if (!buffer->LoadFromMemory(data)) {
        DEMOPH_LOG_ERROR("Failed to create audio buffer from memory");
        return nullptr;
    }
    
    AudioBuffer* bufferPtr = buffer.get();
    std::string key = "memory_buffer_" + std::to_string(reinterpret_cast<uintptr_t>(bufferPtr));
    m_audioBuffers[key] = std::move(buffer);
    
    return bufferPtr;
}

void AdvancedAudioEngine::SetListenerPosition(const Vec3& position) {
    m_listenerPosition = position;
    UpdateListener();
}

void AdvancedAudioEngine::SetListenerVelocity(const Vec3& velocity) {
    m_listenerVelocity = velocity;
    UpdateListener();
}

void AdvancedAudioEngine::SetListenerOrientation(const Vec3& forward, const Vec3& up) {
    m_listenerOrientation[0] = forward;
    m_listenerOrientation[1] = up;
    UpdateListener();
}

void AdvancedAudioEngine::SetMasterVolume(float volume) {
    m_masterVolume = Math::Clamp(volume, 0.0f, 1.0f);
    alListenerf(AL_GAIN, m_masterVolume);
}

void AdvancedAudioEngine::SetDopplerFactor(float factor) {
    m_dopplerFactor = factor;
    alDopplerFactor(factor);
}

void AdvancedAudioEngine::SetSpeedOfSound(float speed) {
    m_speedOfSound = speed;
    alSpeedOfSound(speed);
}

void AdvancedAudioEngine::SetDistanceModel(DistanceModel model) {
    m_distanceModel = model;
    
    ALenum alModel;
    switch (model) {
        case DistanceModel::None:
            alModel = AL_NONE;
            break;
        case DistanceModel::InverseDistance:
            alModel = AL_INVERSE_DISTANCE;
            break;
        case DistanceModel::InverseDistanceClamped:
            alModel = AL_INVERSE_DISTANCE_CLAMPED;
            break;
        case DistanceModel::LinearDistance:
            alModel = AL_LINEAR_DISTANCE;
            break;
        case DistanceModel::LinearDistanceClamped:
            alModel = AL_LINEAR_DISTANCE_CLAMPED;
            break;
        case DistanceModel::ExponentDistance:
            alModel = AL_EXPONENT_DISTANCE;
            break;
        case DistanceModel::ExponentDistanceClamped:
            alModel = AL_EXPONENT_DISTANCE_CLAMPED;
            break;
    }
    
    alDistanceModel(alModel);
}

AudioEffect* AdvancedAudioEngine::CreateAudioEffect(AudioEffectType type) {
    DEMOPH_PROFILE_FUNCTION();
    
    auto effect = std::make_unique<AudioEffect>(type);
    if (!effect->Initialize()) {
        DEMOPH_LOG_ERROR("Failed to create audio effect");
        return nullptr;
    }
    
    AudioEffect* effectPtr = effect.get();
    m_audioEffects.push_back(std::move(effect));
    
    DEMOPH_LOG_INFO("Created audio effect: {}", GetEffectTypeName(type));
    return effectPtr;
}

void AdvancedAudioEngine::DestroyAudioEffect(AudioEffect* effect) {
    auto it = std::find_if(m_audioEffects.begin(), m_audioEffects.end(),
        [effect](const std::unique_ptr<AudioEffect>& e) {
            return e.get() == effect;
        });
    
    if (it != m_audioEffects.end()) {
        m_audioEffects.erase(it);
    }
}

void AdvancedAudioEngine::SetGlobalReverb(const ReverbSettings& settings) {
    m_globalReverbSettings = settings;
    UpdateGlobalReverb();
}

StreamingAudioSource* AdvancedAudioEngine::CreateStreamingSource(const std::string& filepath) {
    DEMOPH_PROFILE_FUNCTION();
    
    auto source = std::make_unique<StreamingAudioSource>(*this, filepath);
    if (!source->Initialize()) {
        DEMOPH_LOG_ERROR("Failed to create streaming audio source: {}", filepath);
        return nullptr;
    }
    
    StreamingAudioSource* sourcePtr = source.get();
    m_streamingSources.push_back(std::move(source));
    
    DEMOPH_LOG_INFO("Created streaming audio source: {}", filepath);
    return sourcePtr;
}

const AudioEngineStatistics& AdvancedAudioEngine::GetStatistics() const {
    return m_statistics;
}

bool AdvancedAudioEngine::InitializeOpenAL() {
    // Open audio device
    m_device = alcOpenDevice(m_config.deviceName.empty() ? nullptr : m_config.deviceName.c_str());
    if (!m_device) {
        DEMOPH_LOG_ERROR("Failed to open audio device");
        return false;
    }
    
    // Create audio context
    ALCint attributes[] = {
        ALC_FREQUENCY, static_cast<ALCint>(m_config.sampleRate),
        ALC_REFRESH, static_cast<ALCint>(m_config.updateRate),
        0
    };
    
    m_context = alcCreateContext(m_device, attributes);
    if (!m_context) {
        DEMOPH_LOG_ERROR("Failed to create audio context");
        alcCloseDevice(m_device);
        return false;
    }
    
    // Make context current
    if (!alcMakeContextCurrent(m_context)) {
        DEMOPH_LOG_ERROR("Failed to make audio context current");
        alcDestroyContext(m_context);
        alcCloseDevice(m_device);
        return false;
    }
    
    // Initialize OpenAL settings
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    alDopplerFactor(m_dopplerFactor);
    alSpeedOfSound(m_speedOfSound);
    
    // Set listener properties
    UpdateListener();
    
    return true;
}

void AdvancedAudioEngine::ShutdownOpenAL() {
    if (m_context) {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
        m_context = nullptr;
    }
    
    if (m_device) {
        alcCloseDevice(m_device);
        m_device = nullptr;
    }
}

void AdvancedAudioEngine::InitializeAudioProcessing() {
    // Initialize audio processing pipeline
    m_audioProcessor = std::make_unique<AudioProcessor>(m_config);
    
    // Initialize DSP chain
    InitializeDSPChain();
    
    DEMOPH_LOG_INFO("Audio processing initialized");
}

void AdvancedAudioEngine::InitializeEffectsSystem() {
    // Check for EFX support
    m_hasEFXSupport = alcIsExtensionPresent(m_device, "ALC_EXT_EFX");
    
    if (m_hasEFXSupport) {
        // Initialize EFX functions
        InitializeEFXFunctions();
        
        // Create effect slots
        CreateEffectSlots();
        
        DEMOPH_LOG_INFO("EFX effects system initialized");
    } else {
        DEMOPH_LOG_WARNING("EFX effects not supported");
    }
}

void AdvancedAudioEngine::InitializeStreamingSystem() {
    // Initialize streaming thread pool
    m_streamingThreadPool = std::make_unique<ThreadPool>(m_config.streamingThreads);
    
    // Initialize streaming buffers
    InitializeStreamingBuffers();
    
    DEMOPH_LOG_INFO("Audio streaming system initialized");
}

void AdvancedAudioEngine::Initialize3DAudio() {
    // Initialize HRTF if available
    if (alcIsExtensionPresent(m_device, "ALC_SOFT_HRTF")) {
        EnableHRTF();
        DEMOPH_LOG_INFO("HRTF enabled for enhanced 3D audio");
    }
    
    // Initialize occlusion system
    m_occlusionSystem = std::make_unique<AudioOcclusionSystem>();
    
    DEMOPH_LOG_INFO("3D audio system initialized");
}

void AdvancedAudioEngine::InitializeReverbSystem() {
    if (m_hasEFXSupport) {
        // Create global reverb effect
        m_globalReverbEffect = CreateReverbEffect();
        
        // Set default reverb settings
        ReverbSettings defaultSettings;
        defaultSettings.roomSize = 0.5f;
        defaultSettings.dampening = 0.3f;
        defaultSettings.wetLevel = 0.2f;
        defaultSettings.dryLevel = 0.8f;
        
        SetGlobalReverb(defaultSettings);
        
        DEMOPH_LOG_INFO("Reverb system initialized");
    }
}

void AdvancedAudioEngine::UpdateListener() {
    // Set listener position
    alListener3f(AL_POSITION, m_listenerPosition.x, m_listenerPosition.y, m_listenerPosition.z);
    
    // Set listener velocity
    alListener3f(AL_VELOCITY, m_listenerVelocity.x, m_listenerVelocity.y, m_listenerVelocity.z);
    
    // Set listener orientation
    float orientation[6] = {
        m_listenerOrientation[0].x, m_listenerOrientation[0].y, m_listenerOrientation[0].z,  // Forward
        m_listenerOrientation[1].x, m_listenerOrientation[1].y, m_listenerOrientation[1].z   // Up
    };
    alListenerfv(AL_ORIENTATION, orientation);
}

void AdvancedAudioEngine::UpdateStreamingSources(float deltaTime) {
    for (auto& source : m_streamingSources) {
        source->Update(deltaTime);
    }
}

void AdvancedAudioEngine::Update3DAudio(float deltaTime) {
    if (m_occlusionSystem) {
        m_occlusionSystem->Update(deltaTime);
    }
    
    // Update 3D audio calculations for all sources
    for (auto& source : m_audioSources) {
        if (source->Is3D()) {
            Update3DSource(*source, deltaTime);
        }
    }
}

void AdvancedAudioEngine::Update3DSource(AudioSource& source, float deltaTime) {
    // Calculate distance attenuation
    float distance = (source.GetPosition() - m_listenerPosition).Length();
    float attenuation = CalculateDistanceAttenuation(distance, source.GetMinDistance(), source.GetMaxDistance());
    
    // Apply occlusion if enabled
    if (m_occlusionSystem && source.IsOcclusionEnabled()) {
        float occlusionFactor = m_occlusionSystem->CalculateOcclusion(
            source.GetPosition(), m_listenerPosition);
        attenuation *= occlusionFactor;
    }
    
    // Update source properties
    source.SetCalculatedVolume(source.GetVolume() * attenuation);
}

void AdvancedAudioEngine::UpdateAudioEffects(float deltaTime) {
    for (auto& effect : m_audioEffects) {
        effect->Update(deltaTime);
    }
}

void AdvancedAudioEngine::UpdateReverbSystem(float deltaTime) {
    if (m_globalReverbEffect) {
        m_globalReverbEffect->Update(deltaTime);
    }
}

void AdvancedAudioEngine::UpdateStatistics(float deltaTime) {
    m_statistics.activeSources = GetActiveSourceCount();
    m_statistics.totalSources = m_audioSources.size();
    m_statistics.streamingSources = m_streamingSources.size();
    m_statistics.loadedBuffers = m_audioBuffers.size();
    m_statistics.memoryUsage = CalculateMemoryUsage();
    m_statistics.cpuUsage = CalculateCPUUsage();
    m_statistics.hasEFXSupport = m_hasEFXSupport;
    m_statistics.hasHRTFSupport = m_hasHRTFSupport;
}

float AdvancedAudioEngine::CalculateDistanceAttenuation(float distance, float minDistance, float maxDistance) {
    if (distance <= minDistance) {
        return 1.0f;
    }
    
    if (distance >= maxDistance) {
        return 0.0f;
    }
    
    switch (m_distanceModel) {
        case DistanceModel::LinearDistance:
        case DistanceModel::LinearDistanceClamped:
            return 1.0f - (distance - minDistance) / (maxDistance - minDistance);
            
        case DistanceModel::InverseDistance:
        case DistanceModel::InverseDistanceClamped:
            return minDistance / distance;
            
        case DistanceModel::ExponentDistance:
        case DistanceModel::ExponentDistanceClamped:
            return std::pow(distance / minDistance, -1.0f);
            
        default:
            return 1.0f;
    }
}

} // namespace Demoph
