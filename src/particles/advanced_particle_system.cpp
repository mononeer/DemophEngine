
#include "demoph/particles/advanced_particle_system.h"
#include "demoph/core/log.h"
#include "demoph/utils/profiler.h"
#include "demoph/utils/math.h"
#include <algorithm>
#include <random>

namespace Demoph {

AdvancedParticleSystem::AdvancedParticleSystem(uint32_t maxParticles)
    : m_maxParticles(maxParticles)
    , m_activeParticles(0)
    , m_isGPUBased(true)
    , m_simulationTime(0.0f) {
    
    Initialize();
}

AdvancedParticleSystem::~AdvancedParticleSystem() {
    Shutdown();
}

void AdvancedParticleSystem::Initialize() {
    DEMOPH_PROFILE_FUNCTION();
    
    // Allocate particle data
    m_particles.resize(m_maxParticles);
    m_particleIndices.reserve(m_maxParticles);
    
    // Initialize GPU resources
    InitializeGPUResources();
    
    // Create compute shaders
    InitializeComputeShaders();
    
    // Initialize emitters
    InitializeEmitters();
    
    // Initialize physics simulation
    InitializePhysics();
    
    // Initialize rendering resources
    InitializeRendering();
    
    DEMOPH_LOG_INFO("Advanced Particle System initialized with {} max particles", m_maxParticles);
}

void AdvancedParticleSystem::Shutdown() {
    if (m_isInitialized) {
        ShutdownRendering();
        ShutdownPhysics();
        ShutdownEmitters();
        ShutdownComputeShaders();
        ShutdownGPUResources();
        
        m_particles.clear();
        m_particleIndices.clear();
        
        m_isInitialized = false;
        DEMOPH_LOG_INFO("Advanced Particle System shutdown complete");
    }
}

void AdvancedParticleSystem::Update(float deltaTime) {
    DEMOPH_PROFILE_FUNCTION();
    
    m_simulationTime += deltaTime;
    
    if (m_isGPUBased) {
        UpdateGPU(deltaTime);
    } else {
        UpdateCPU(deltaTime);
    }
    
    // Update emitters
    for (auto& emitter : m_emitters) {
        emitter->Update(deltaTime);
    }
    
    // Update statistics
    UpdateStatistics();
}

void AdvancedParticleSystem::Render(Renderer* renderer) {
    DEMOPH_PROFILE_FUNCTION();
    
    if (m_activeParticles == 0) {
        return;
    }
    
    // Sort particles if needed
    if (m_sortParticles) {
        SortParticles();
    }
    
    // Render particles based on type
    switch (m_renderMode) {
        case ParticleRenderMode::Billboard:
            RenderBillboards(renderer);
            break;
        case ParticleRenderMode::Mesh:
            RenderMeshes(renderer);
            break;
        case ParticleRenderMode::Trail:
            RenderTrails(renderer);
            break;
        case ParticleRenderMode::Volumetric:
            RenderVolumetric(renderer);
            break;
    }
}

ParticleEmitter* AdvancedParticleSystem::CreateEmitter(const ParticleEmitterDesc& desc) {
    auto emitter = std::make_unique<ParticleEmitter>(*this, desc);
    ParticleEmitter* emitterPtr = emitter.get();
    m_emitters.push_back(std::move(emitter));
    
    DEMOPH_LOG_INFO("Created particle emitter: {}", desc.name);
    return emitterPtr;
}

void AdvancedParticleSystem::DestroyEmitter(ParticleEmitter* emitter) {
    auto it = std::find_if(m_emitters.begin(), m_emitters.end(),
        [emitter](const std::unique_ptr<ParticleEmitter>& e) {
            return e.get() == emitter;
        });
    
    if (it != m_emitters.end()) {
        DEMOPH_LOG_INFO("Destroyed particle emitter");
        m_emitters.erase(it);
    }
}

uint32_t AdvancedParticleSystem::EmitParticles(const ParticleEmissionData& emission) {
    DEMOPH_PROFILE_FUNCTION();
    
    uint32_t particlesToEmit = std::min(emission.count, m_maxParticles - m_activeParticles);
    if (particlesToEmit == 0) {
        return 0;
    }
    
    if (m_isGPUBased) {
        return EmitParticlesGPU(emission, particlesToEmit);
    } else {
        return EmitParticlesCPU(emission, particlesToEmit);
    }
}

void AdvancedParticleSystem::SetGravity(const Vec3& gravity) {
    m_gravity = gravity;
    
    // Update GPU constant buffer if using GPU simulation
    if (m_isGPUBased) {
        UpdateSimulationConstants();
    }
}

void AdvancedParticleSystem::SetWindForce(const Vec3& wind) {
    m_windForce = wind;
    
    if (m_isGPUBased) {
        UpdateSimulationConstants();
    }
}

void AdvancedParticleSystem::AddForceField(const ForceField& field) {
    m_forceFields.push_back(field);
    
    if (m_isGPUBased) {
        UpdateForceFieldBuffer();
    }
}

void AdvancedParticleSystem::RemoveForceField(size_t index) {
    if (index < m_forceFields.size()) {
        m_forceFields.erase(m_forceFields.begin() + index);
        
        if (m_isGPUBased) {
            UpdateForceFieldBuffer();
        }
    }
}

void AdvancedParticleSystem::SetCollisionPlanes(const std::vector<Plane>& planes) {
    m_collisionPlanes = planes;
    
    if (m_isGPUBased) {
        UpdateCollisionBuffer();
    }
}

const ParticleSystemStatistics& AdvancedParticleSystem::GetStatistics() const {
    return m_statistics;
}

void AdvancedParticleSystem::InitializeGPUResources() {
    // Create particle buffers
    CreateParticleBuffers();
    
    // Create constant buffers
    CreateConstantBuffers();
    
    // Create structured buffers
    CreateStructuredBuffers();
    
    DEMOPH_LOG_INFO("GPU resources initialized for particle system");
}

void AdvancedParticleSystem::InitializeComputeShaders() {
    // Load and compile compute shaders
    m_simulationShader = LoadComputeShader("shaders/particles/simulation.comp");
    m_emissionShader = LoadComputeShader("shaders/particles/emission.comp");
    m_sortingShader = LoadComputeShader("shaders/particles/sorting.comp");
    m_collisionShader = LoadComputeShader("shaders/particles/collision.comp");
    
    DEMOPH_LOG_INFO("Particle compute shaders loaded");
}

void AdvancedParticleSystem::InitializeEmitters() {
    // Reserve space for emitters
    m_emitters.reserve(16);
}

void AdvancedParticleSystem::InitializePhysics() {
    // Initialize physics constants
    m_gravity = Vec3(0.0f, -9.81f, 0.0f);
    m_windForce = Vec3::Zero();
    m_airResistance = 0.01f;
    m_restitution = 0.8f;
    
    // Initialize collision detection
    m_enableCollisions = true;
    m_collisionResponse = CollisionResponse::Bounce;
}

void AdvancedParticleSystem::InitializeRendering() {
    // Create vertex buffers for rendering
    CreateRenderBuffers();
    
    // Load rendering shaders
    LoadRenderShaders();
    
    // Initialize sorting system
    InitializeSorting();
    
    DEMOPH_LOG_INFO("Particle rendering initialized");
}

void AdvancedParticleSystem::UpdateGPU(float deltaTime) {
    DEMOPH_PROFILE_FUNCTION();
    
    // Update simulation constants
    UpdateSimulationConstants();
    
    // Dispatch simulation compute shader
    DispatchSimulationShader(deltaTime);
    
    // Handle collisions
    if (m_enableCollisions) {
        DispatchCollisionShader();
    }
    
    // Update alive particle count
    UpdateAliveCount();
}

void AdvancedParticleSystem::UpdateCPU(float deltaTime) {
    DEMOPH_PROFILE_FUNCTION();
    
    // Update particles on CPU
    for (uint32_t i = 0; i < m_activeParticles; ++i) {
        auto& particle = m_particles[i];
        
        if (particle.life <= 0.0f) {
            continue;
        }
        
        // Update physics
        UpdateParticlePhysics(particle, deltaTime);
        
        // Update life
        particle.life -= deltaTime;
        
        // Update properties based on life
        UpdateParticleProperties(particle);
    }
    
    // Remove dead particles
    RemoveDeadParticles();
}

void AdvancedParticleSystem::UpdateParticlePhysics(Particle& particle, float deltaTime) {
    // Apply gravity
    particle.velocity += m_gravity * deltaTime;
    
    // Apply wind force
    particle.velocity += m_windForce * deltaTime;
    
    // Apply air resistance
    particle.velocity *= (1.0f - m_airResistance * deltaTime);
    
    // Apply force fields
    for (const auto& field : m_forceFields) {
        ApplyForceField(particle, field, deltaTime);
    }
    
    // Update position
    particle.position += particle.velocity * deltaTime;
    
    // Handle collisions
    if (m_enableCollisions) {
        HandleParticleCollisions(particle);
    }
}

void AdvancedParticleSystem::UpdateParticleProperties(Particle& particle) {
    // Calculate life ratio
    float lifeRatio = particle.life / particle.initialLife;
    
    // Interpolate color
    particle.color = Math::Lerp(particle.endColor, particle.startColor, lifeRatio);
    
    // Interpolate size
    particle.size = Math::Lerp(particle.endSize, particle.startSize, lifeRatio);
    
    // Update rotation
    particle.rotation += particle.angularVelocity * GetDeltaTime();
}

void AdvancedParticleSystem::ApplyForceField(Particle& particle, const ForceField& field, float deltaTime) {
    Vec3 direction = particle.position - field.position;
    float distance = direction.Length();
    
    if (distance > field.radius) {
        return;
    }
    
    direction.Normalize();
    
    float strength = field.strength;
    if (field.falloff != ForceFieldFalloff::None) {
        float falloffFactor = CalculateFalloff(distance / field.radius, field.falloff);
        strength *= falloffFactor;
    }
    
    Vec3 force;
    switch (field.type) {
        case ForceFieldType::Attract:
            force = -direction * strength;
            break;
        case ForceFieldType::Repel:
            force = direction * strength;
            break;
        case ForceFieldType::Vortex:
            force = direction.Cross(Vec3::Up()) * strength;
            break;
        case ForceFieldType::Turbulence:
            force = GenerateTurbulence(particle.position, field.turbulenceScale) * strength;
            break;
    }
    
    particle.velocity += force * deltaTime;
}

void AdvancedParticleSystem::HandleParticleCollisions(Particle& particle) {
    for (const auto& plane : m_collisionPlanes) {
        float distance = plane.DistanceToPoint(particle.position);
        
        if (distance < particle.size * 0.5f) {
            // Collision detected
            switch (m_collisionResponse) {
                case CollisionResponse::Die:
                    particle.life = 0.0f;
                    break;
                case CollisionResponse::Bounce:
                    {
                        Vec3 reflection = particle.velocity.Reflect(plane.normal);
                        particle.velocity = reflection * m_restitution;
                        particle.position = particle.position - plane.normal * (distance - particle.size * 0.5f);
                    }
                    break;
                case CollisionResponse::Stick:
                    particle.velocity = Vec3::Zero();
                    break;
            }
        }
    }
}

uint32_t AdvancedParticleSystem::EmitParticlesCPU(const ParticleEmissionData& emission, uint32_t count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    
    uint32_t emitted = 0;
    for (uint32_t i = 0; i < count && m_activeParticles < m_maxParticles; ++i) {
        auto& particle = m_particles[m_activeParticles];
        
        // Initialize particle properties
        InitializeParticle(particle, emission, gen);
        
        m_activeParticles++;
        emitted++;
    }
    
    return emitted;
}

void AdvancedParticleSystem::InitializeParticle(Particle& particle, const ParticleEmissionData& emission, std::mt19937& gen) {
    // Position
    particle.position = SamplePosition(emission.shape, gen);
    particle.position += emission.position;
    
    // Velocity
    particle.velocity = SampleVelocity(emission.velocity, gen);
    
    // Life
    std::uniform_real_distribution<float> lifeDist(emission.minLife, emission.maxLife);
    particle.life = particle.initialLife = lifeDist(gen);
    
    // Size
    std::uniform_real_distribution<float> sizeDist(emission.minSize, emission.maxSize);
    particle.startSize = particle.size = sizeDist(gen);
    particle.endSize = particle.startSize * emission.sizeOverLifetime;
    
    // Color
    particle.startColor = emission.startColor;
    particle.endColor = emission.endColor;
    particle.color = particle.startColor;
    
    // Rotation
    std::uniform_real_distribution<float> rotDist(0.0f, 2.0f * Math::PI);
    particle.rotation = rotDist(gen);
    
    std::uniform_real_distribution<float> angVelDist(emission.minAngularVelocity, emission.maxAngularVelocity);
    particle.angularVelocity = angVelDist(gen);
    
    // Mass
    std::uniform_real_distribution<float> massDist(emission.minMass, emission.maxMass);
    particle.mass = massDist(gen);
}

void AdvancedParticleSystem::SortParticles() {
    DEMOPH_PROFILE_FUNCTION();
    
    if (m_isGPUBased) {
        SortParticlesGPU();
    } else {
        SortParticlesCPU();
    }
}

void AdvancedParticleSystem::SortParticlesCPU() {
    // Get camera position for distance calculation
    Vec3 cameraPos = GetCameraPosition();
    
    // Update particle indices with distances
    m_particleIndices.clear();
    for (uint32_t i = 0; i < m_activeParticles; ++i) {
        if (m_particles[i].life > 0.0f) {
            float distance = (m_particles[i].position - cameraPos).LengthSquared();
            m_particleIndices.push_back({i, distance});
        }
    }
    
    // Sort by distance (back to front for transparency)
    std::sort(m_particleIndices.begin(), m_particleIndices.end(),
        [](const ParticleIndex& a, const ParticleIndex& b) {
            return a.distance > b.distance;
        });
}

void AdvancedParticleSystem::RemoveDeadParticles() {
    // Compact particle array by removing dead particles
    uint32_t writeIndex = 0;
    for (uint32_t readIndex = 0; readIndex < m_activeParticles; ++readIndex) {
        if (m_particles[readIndex].life > 0.0f) {
            if (writeIndex != readIndex) {
                m_particles[writeIndex] = m_particles[readIndex];
            }
            writeIndex++;
        }
    }
    
    m_activeParticles = writeIndex;
}

void AdvancedParticleSystem::UpdateStatistics() {
    m_statistics.activeParticles = m_activeParticles;
    m_statistics.maxParticles = m_maxParticles;
    m_statistics.emissionRate = CalculateEmissionRate();
    m_statistics.memoryUsage = CalculateMemoryUsage();
    m_statistics.simulationTime = GetSimulationTime();
    m_statistics.isGPUBased = m_isGPUBased;
}

} // namespace Demoph
