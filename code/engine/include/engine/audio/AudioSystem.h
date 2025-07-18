/**
* @file AudioSystem.h
 * @brief Defines the audio subsystem for the game engine using SoLoud for playback and basic beat tracking.
 */
#pragma once
#include <memory>
#include <soloud.h>
#include <soloud_wav.h>

#include "engine/Game.h"
#include "engine/ecs/System.h"
#include "engine/userInterface/UIEvents.h"

namespace gl3::engine::audio
{
    /**
     * @struct AudioConfig
     * @brief Holds configuration and runtime data for the main audio track.
     *
     * Stores the SoLoud audio engine instance, loaded background music,
     * analysis parameters for beat detection, playback state,
     * and user-defined audio settings like global volume.
     */
    struct AudioConfig
    {
        SoLoud::Soloud audio; /**< SoLoud audio engine instance. */
        std::unique_ptr<SoLoud::Wav> backgroundMusic; /**< Loaded background music asset. */
        SoLoud::handle currentAudioHandle; /**< Handle for the currently playing audio. */
        std::string filePath; /**< Path to the loaded audio file. */
        unsigned int hopSize = 512; /**< Hop size for audio analysis. */
        unsigned int bufferSize = 2048; /**< Buffer size for audio analysis. */
        float bpm = 0.0f; /**< Estimated beats per minute (BPM). */
        float seconds_per_beat = 1.f; /**< Duration of a beat in seconds. */
        float current_audio_length = 0.f; /**< Total length of the current audio track in seconds. */
        float global_volume = 1.0f; /**< Master volume multiplier. */
        std::vector<float> beatPositions; /**< Detected beat timestamps (in seconds). */
    };

    /**
 * @class AudioSystem
 * @brief ECS system for managing audio playback and sound effects.
 *
 * The AudioSystem handles background music playback, beat tracking setup,
 * one-shot sound effects, and user-controlled volume changes using the SoLoud audio library.
 */
    class AudioSystem final : public ecs::System
    {
    public:
        /**
         * @brief Construct a new AudioSystem.
         * @param game Reference to the main Game instance.
         */
        explicit AudioSystem(Game& game);

        /**
         * @brief Destroy the AudioSystem and release resources.
         */
        ~AudioSystem() override;

        /**
         * @brief Update the audio system (e.g., delete unused handles).
         */
        void update();

        /**
         * @brief Load a one-shot sound effect.
         * @param sfxName Identifier for the sound effect.
         * @param fileName Path to the audio file.
         */
        void loadOneShot(const std::string& sfxName, const std::string& fileName);

        /**
         * @brief Play a previously loaded one-shot sound effect.
         * @param sfxName Identifier of the sound effect to play.
         */
        void playOneShot(const std::string& sfxName);

        /**
         * @brief Unload a previously loaded one-shot sound effect.
         * @param sfxName Identifier of the sound effect to unload.
         */
        void unloadOneShot(const std::string& sfxName);

        /**
         * @brief Initialize and load the main background audio track.
         * @param fileName Path to the audio file.
         * @param positionOffsetX Optional position x offset for beat positions (shift them to the right).
         */
        void initializeCurrentAudio(const std::string& fileName, float positionOffsetX = 0.f);

        /**
         * @brief Get a pointer to the current AudioConfig.
         * @return Pointer to the AudioConfig.
         */
        AudioConfig* getConfig() const;

        /**
         * @brief Start playback of the current background audio track.
         */
        void playCurrentAudio() const;

        /**
         * @brief Stop playback of the current background audio track.
         */
        void stopCurrentAudio() const;

        /**
         * @brief Stop all currently playing one-shot sounds.
         */
        void stopAllOneShots();

        /**
         * @brief Reset the AudioConfig to its default state.
         */
        void resetConfig();

    private:
        /**
         * @brief Callback for when the global volume is changed via the UI.
         * @param event UI event containing the new volume.
         */
        void onGlobalVolumeChanged(const ui::VolumeChangeEvent& event) const;

        std::unique_ptr<AudioConfig> config; /**< Current audio configuration and state. */
        std::unordered_map<std::string, std::unique_ptr<SoLoud::Wav>> one_shot_sounds; /**< Loaded one-shot SFX. */
        std::vector<SoLoud::handle> active_handles; /**< Handles of currently playing one-shots. */
    };
}

