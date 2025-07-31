
#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

namespace Demoph {

class AudioClip;
class AudioSource;

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();
    
    bool Initialize();
    void Shutdown();
    
    void Update(float deltaTime);
    
    std::shared_ptr<AudioClip> LoadAudio(const std::string& filePath);
    void PlaySound(const std::string& clipID, float volume = 1.0f);
    void PlaySound3D(const std::string& clipID, const glm::vec3& position, float volume = 1.0f);
    
    void SetListenerPosition(const glm::vec3& position);
    void SetListenerOrientation(const glm::vec3& forward, const glm::vec3& up);
    
    void SetMasterVolume(float volume) { m_masterVolume = volume; }
    float GetMasterVolume() const { return m_masterVolume; }
    
private:
    std::unordered_map<std::string, std::shared_ptr<AudioClip>> m_audioClips;
    std::vector<std::unique_ptr<AudioSource>> m_activeSources;
    
    glm::vec3 m_listenerPosition;
    glm::vec3 m_listenerForward;
    glm::vec3 m_listenerUp;
    
    float m_masterVolume;
    bool m_initialized;
};

} // namespace Demoph
