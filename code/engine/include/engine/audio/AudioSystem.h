#pragma once
#include <memory>
#include <soloud.h>
#include <soloud_wav.h>

#include "engine/Game.h"
#include "engine/ecs/GameEvents.h"

namespace gl3::engine::audio
{
    struct AudioConfig
    {
        SoLoud::Soloud audio_;
        std::unique_ptr<SoLoud::Wav> backgroundMusic;
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

    private:
        void initializeCurrentAudio();
        void playCurrentAudio();
        void resetCurrentAudio();
        void onGlobalVolumeChanged(const ui::VolumeChange& event);
        void onGameStateChange(ecs::GameStateChange& event);
        void onPlayerDeath(ecs::PlayerDeath& event);
        Game& game_;
        AudioConfig config_;
        GameState previousState = GameState::None;
    };
}

