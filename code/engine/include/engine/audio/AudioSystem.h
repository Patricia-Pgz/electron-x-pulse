#pragma once
#include <memory>
#include <soloud.h>
#include <soloud_wav.h>

#include "engine/Game.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::engine::audio
{
    struct AudioConfig
    {
        SoLoud::Soloud audio_;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;
        std::string filePath;
        unsigned int hopSize = 512; // Size of each hop
        unsigned int bufferSize = 2048; // Size of the analysis buffer
        float bpm = 0.0f; //update this based on audio analysis
        float seconds_per_beat = 1.f;
        float current_audio_length = 0.f; //TODO wird neu berechnet, wenn anderes lvl = song ausgewählt
        float global_volume_ = 1.0f;
        std::vector<float> beatPositions;
    };

    class AudioSystem
    {
    public:
        explicit AudioSystem(Game& game);
        ~AudioSystem();
        void initializeCurrentAudio(const std::string& fileName);
        void playCurrentAudio();
        void resetCurrentAudio();
    private:
        void onGlobalVolumeChanged(const ui::VolumeChange& event);
        Game& game_;
        AudioConfig config_;
        GameState previousState = GameState::None;
    };
}

