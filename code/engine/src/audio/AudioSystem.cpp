#include "engine/audio/AudioSystem.h"

#include "engine/audio/AudioAnalysis.h"
#include "engine/Assets.h"
#include "engine/ecs/EventDispatcher.h"
#include "engine/ecs/GameEvents.h"

namespace gl3::engine::audio
{
    AudioSystem::AudioSystem(Game& game) : System(game)
    {
        ecs::EventDispatcher::dispatcher.sink<ui::VolumeChangeEvent>().connect<&
            AudioSystem::onGlobalVolumeChanged>(this);
        config_.audio.init();
        config_.audio.setGlobalVolume(config_.global_volume_);
    }

    AudioSystem::~AudioSystem()
    {
        ecs::EventDispatcher::dispatcher.sink<ui::VolumeChangeEvent>().disconnect<&
            AudioSystem::onGlobalVolumeChanged>(this);
    }

    void AudioSystem::loadOneShot(const std::string& sfxName, const std::string& fileName)
    {
        const auto path = "audio/" + fileName;
        auto wav = std::make_unique<SoLoud::Wav>();
        if (wav->load(resolveAssetPath(path).c_str()) != 0)
        {
            std::cerr << "[AudioSystem] Failed to load SFX: " << fileName << std::endl;
            return;
        }

        oneShotSounds_[sfxName] = std::move(wav);
    }

    void AudioSystem::playOneShot(const std::string& sfxName)
    {
        if (const auto wav = oneShotSounds_.find(sfxName); wav != oneShotSounds_.end())
        {
            config_.audio.play(*wav->second);
        }
        else
        {
            std::cerr << "[AudioSystem] SFX not found: " << sfxName << std::endl;
        }
    }

    void AudioSystem::unloadOneShot(const std::string& sfxName)
    {
        if (const auto wav = oneShotSounds_.find(sfxName); wav != oneShotSounds_.end())
        {
            oneShotSounds_.erase(wav);
        }
    }

    AudioConfig* AudioSystem::initializeCurrentAudio(const std::string& fileName, float positionOffsetX)
    {
        const auto path = "audio/" + fileName;
        config_.backgroundMusic = std::make_unique<SoLoud::Wav>();
        config_.backgroundMusic->load(resolveAssetPath(path).c_str());
        config_.backgroundMusic->setLooping(false);
        config_.current_audio_length = static_cast<float>(config_.backgroundMusic->getLength());

        const std::string audio_file = resolveAssetPath(path);
        config_.bpm = AudioAnalysis::analyzeAudioTempo(audio_file, config_.hopSize, config_.bufferSize);
        config_.seconds_per_beat = 60 / config_.bpm;
        config_.beatPositions = AudioAnalysis::generateBeatTimestamps(
            config_.current_audio_length,
            config_.seconds_per_beat,
            positionOffsetX);
        return &config_;
    }

    void AudioSystem::playCurrentAudio()
    {
        config_.currentAudioHandle = config_.audio.playBackground(*config_.backgroundMusic);
    }

    void AudioSystem::stopCurrentAudio()
    {
        config_.audio.stopAudioSource(*config_.backgroundMusic);
    }

    void AudioSystem::onGlobalVolumeChanged(const ui::VolumeChangeEvent& event)
    {
        config_.global_volume_ = event.newVolume;
        config_.audio.setGlobalVolume(config_.global_volume_);
    }
}
