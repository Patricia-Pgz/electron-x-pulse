#pragma once
#include <memory>
#include <soloud.h>
#include <soloud_wav.h>

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
        float current_audio_length = 0.f; //TODO wird neu berechnet, wenn anderes lvl = song ausgewählt
        float global_volume_ = 1.0f;
        std::vector<float> beatPositions;
    };

    class AudioSystem final : public ecs::System
    {
    public:
        explicit AudioSystem(Game& game);
        ~AudioSystem() override;
        AudioConfig* initializeCurrentAudio(const std::string& fileName, float positionOffsetX = 0.f);
        void playCurrentAudio();
        void stopCurrentAudio();

    private:
        void onGlobalVolumeChanged(const ui::VolumeChangeEvent& event);
        AudioConfig config_;
        GameState previous_state_ = GameState::None;
    };
}

