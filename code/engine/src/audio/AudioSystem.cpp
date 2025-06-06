#include "engine/audio/AudioSystem.h"
#include "engine/audio/AudioAnalysis.h"
#include "engine/Assets.h"
#include "engine/ecs/EventDispatcher.h"

namespace gl3::engine::audio
{
    AudioSystem::AudioSystem(Game& game) : game_(game)
    {
        ecs::EventDispatcher::dispatcher.sink<ui::VolumeChange>().connect<&
            AudioSystem::onGlobalVolumeChanged>(this);
        ecs::EventDispatcher::dispatcher.sink<ecs::GameStateChange>().connect<&
            AudioSystem::onGameStateChange>(this);
        ecs::EventDispatcher::dispatcher.sink<ecs::PlayerDeath>().connect<&
            AudioSystem::onPlayerDeath>(this);
        config_.audio_.init();
        config_.audio_.setGlobalVolume(config_.global_volume_);
    }

    AudioSystem::~AudioSystem()
    {
        ecs::EventDispatcher::dispatcher.sink<ui::VolumeChange>().disconnect<&
            AudioSystem::onGlobalVolumeChanged>(this);
        ecs::EventDispatcher::dispatcher.sink<ecs::PlayerDeath>().disconnect<&
            AudioSystem::onPlayerDeath>(this);
    }

    void AudioSystem::initializeCurrentAudio()
    {
        config_.backgroundMusic = std::make_unique<SoLoud::Wav>();
        config_.backgroundMusic->load(resolveAssetPath("audio/SensesShort.wav").c_str());
        //TODO pfad aus json lesen
        config_.backgroundMusic->setLooping(false);
        config_.current_audio_length = static_cast<float>(config_.backgroundMusic->getLength());

        const std::string audio_file = resolveAssetPath("audio/SensesShort.wav");
        config_.bpm = AudioAnalysis::analyzeAudioTempo(audio_file, config_.hopSize, config_.bufferSize);
        config_.seconds_per_beat = 60 / config_.bpm;
        config_.beatPositions = AudioAnalysis::generateBeatTimestamps(
            config_.current_audio_length,
            config_.seconds_per_beat,
            game_.getCurrentConfig().initial_player_position_x);
        game_.getCurrentConfig().level_length = config_.current_audio_length;
    }

    void AudioSystem::playCurrentAudio()
    {
        config_.audio_.playBackground(*config_.backgroundMusic);
    }

    void AudioSystem::resetCurrentAudio()
    {
        config_.audio_.stopAudioSource(*config_.backgroundMusic);
        config_.audio_.playBackground(*config_.backgroundMusic); //TODO maybe das weglassen
    }

    void AudioSystem::onGlobalVolumeChanged(const ui::VolumeChange& event)
    {
        config_.global_volume_ = event.newVolume;
        config_.audio_.setGlobalVolume(config_.global_volume_);
    }

    void AudioSystem::onGameStateChange(ecs::GameStateChange& event) //TODO lieber in Subclass?
    {
        if (event.newGameState != previousState)
        {
            switch (event.newGameState) //TODO
            {
            case GameState::LevelSelect: //Evtl lvl select music spielen
                break;
            case GameState::Menu: //TODO musik stoppen/Lvl anhalten
                break;
            case GameState::Level: //TODO config neu generieren/analysieren
            case GameState::PreviewWithTesting:
                initializeCurrentAudio();
                playCurrentAudio(); //TODO evtl warten
                break;
            case GameState::PreviewWithScrolling: //TODO nix / musik stoppen
                break;
            default: ;
            }
        }
    }

    void AudioSystem::onPlayerDeath(ecs::PlayerDeath& event)
    {
        //TODO evtl nur bool setzen + warten bis alles fertig mit resetten -> dann musik starten, dann physics
        resetCurrentAudio();
    }
}
