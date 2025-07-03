#pragma once
#include <memory>
#include <soloud.h>
#include <soloud_wav.h>
#include <stdexcept>

#include "engine/Game.h"
#include "engine/ecs/System.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::engine::audio
{
    struct AudioConfig
    {
        SoLoud::Soloud audio;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;
        SoLoud::handle currentAudioHandle;
        std::string filePath;
        unsigned int hopSize = 512; // Size of each hop
        unsigned int bufferSize = 2048; // Size of the analysis buffer
        float bpm = 0.0f; //update this based on audio analysis
        float seconds_per_beat = 1.f;
        float current_audio_length = 0.f;
        float global_volume_ = 1.0f;
        std::vector<float> beatPositions;
    };

    class AudioSystem final : public ecs::System
    {
    public:
        explicit AudioSystem(Game& game);
        ~AudioSystem() override;
        void update();
        void loadOneShot(const std::string& sfxName, const std::string& fileName);
        void playOneShot(const std::string& sfxName);
        void unloadOneShot(const std::string& sfxName);
        void initializeCurrentAudio(const std::string& fileName, float positionOffsetX = 0.f);
        AudioConfig* getConfig();
        void playCurrentAudio();
        void stopCurrentAudio();
        void stopAllOneShots();
        void resetConfig();

    private:
        void onGlobalVolumeChanged(const ui::VolumeChangeEvent& event);
        std::unique_ptr<AudioConfig> config_;
        std::unordered_map<std::string, std::unique_ptr<SoLoud::Wav>> oneShotSounds_;
        std::vector<SoLoud::handle> activeHandles;
    };
}

