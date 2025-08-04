#include "engine/audio/AudioSystem.h"

#include <iostream>

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
        if (!config)
        {
            config = std::make_unique<AudioConfig>();
            config->audio.init();
            config->audio.setGlobalVolume(config->global_volume);
        }
    }

    AudioSystem::~AudioSystem()
    {
        ecs::EventDispatcher::dispatcher.sink<ui::VolumeChangeEvent>().disconnect<&
            AudioSystem::onGlobalVolumeChanged>(this);
        if (config)
        {
            config->audio.deinit();
        }
    }

    void AudioSystem::update()
    {
        active_handles.erase(
            std::ranges::remove_if(active_handles,
                                   [this](const SoLoud::handle h)
                                   {
                                       return !config->audio.isValidVoiceHandle(h);
                                   }).begin(),
            active_handles.end());
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

        one_shot_sounds[sfxName] = std::move(wav);
    }

    void AudioSystem::playOneShot(const std::string& sfxName)
    {
        if (const auto wav = one_shot_sounds.find(sfxName); wav != one_shot_sounds.end())
        {
            active_handles.push_back(config->audio.play(*wav->second));
        }
        else
        {
            std::cerr << "[AudioSystem] SFX not found: " << sfxName << std::endl;
        }
    }

    void AudioSystem::unloadOneShot(const std::string& sfxName)
    {
        if (const auto wav = one_shot_sounds.find(sfxName); wav != one_shot_sounds.end())
        {
            one_shot_sounds.erase(wav);
        }
    }

    void AudioSystem::stopAllOneShots()
    {
        for (const auto handle : active_handles)
        {
            if (config->audio.isValidVoiceHandle(handle))
            {
                config->audio.stop(handle);
            }
        }
        active_handles.clear();
    }

    void AudioSystem::resetConfig()
    {
        config = nullptr;
    }

    void AudioSystem::initializeCurrentAudio(const std::string& fileName, const float positionOffsetX)
    {
        const auto path = "audio/" + fileName;
        if (!config)
        {
            config = std::make_unique<AudioConfig>();
            config->audio.init();
        }
        config->backgroundMusic = std::make_unique<SoLoud::Wav>();
        config->backgroundMusic->load(resolveAssetPath(path).c_str());
        config->backgroundMusic->setLooping(false);
        config->current_audio_length = static_cast<float>(config->backgroundMusic->getLength());

        const std::string audio_file = resolveAssetPath(path);
        config->bpm = AudioAnalysis::analyzeAudioTempo(audio_file, config->hopSize, config->bufferSize);
        config->seconds_per_beat = 60 / config->bpm;
        config->beatPositions = AudioAnalysis::generateBeatTimestamps(
            config->current_audio_length,
            config->seconds_per_beat,
            positionOffsetX);
    }

    AudioConfig* AudioSystem::getConfig() const
    {
        return config.get();
    }

    void AudioSystem::playCurrentAudio() const
    {
        config->currentAudioHandle = config->audio.playBackground(*config->backgroundMusic);
    }

    void AudioSystem::stopCurrentAudio() const
    {
        config->audio.stopAudioSource(*config->backgroundMusic);
    }

    void AudioSystem::onGlobalVolumeChanged(const ui::VolumeChangeEvent& event) const
    {
        config->global_volume = event.newVolume;
        config->audio.setGlobalVolume(config->global_volume);
    }
}
