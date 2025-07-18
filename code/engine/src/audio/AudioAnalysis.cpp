#include "engine/audio/AudioAnalysis.h"
#include "../aubio/src/aubio.h"
#include <iostream>

namespace gl3::engine
{
    float AudioAnalysis::analyzeAudioTempo(const std::string& audioFilePath, const unsigned int hopSize,
                                           const unsigned int bufferSize)
    {
        //aubio detects sampleRate automatically if 0
        unsigned int sampleRate = 0;
        // Create aubio source object
        aubio_source_t* source = new_aubio_source(audioFilePath.c_str(), sampleRate, hopSize);
        if (!source)
        {
            std::cerr << "Error: Failed to open audio source!" << std::endl;
            return {};
        }

        // Get actual sample rate
        sampleRate = aubio_source_get_samplerate(source);

        // Create aubio tempo object
        aubio_tempo_t* tempo = new_aubio_tempo("complex", bufferSize, hopSize, sampleRate);
        if (!tempo)
        {
            std::cerr << "Error: Failed to create tempo object!" << std::endl;
            del_aubio_source(source);
            return {};
        }

        fvec_t* audio_frame = new_fvec(hopSize); // Audio input frame buffer
        fvec_t* beat_output = new_fvec(1); // Beat detection output buffer
        aubio_tempo_set_threshold(tempo, 0.9);

        uint_t read = 0;
        while (true)
        {
            // Read a frame of audio
            aubio_source_do(source, audio_frame, &read);

            // Stop if no more frames
            if (read == 0) break;

            // Analyze for beats
            aubio_tempo_do(tempo, audio_frame, beat_output);
        }
        const auto bpm = aubio_tempo_get_bpm(tempo);
        del_fvec(beat_output);
        del_fvec(audio_frame);
        del_aubio_tempo(tempo);
        del_aubio_source(source);

        aubio_cleanup();

        return bpm;
    }

    std::vector<float> AudioAnalysis::analyzeAudioOnsets(
        const std::string& audioFilePath,
        const unsigned int hopSize,
        const unsigned int bufferSize,
        const std::string& onsetMethod,
        const float onsetThreshold,
        const float minInterOnsetInterval)
    {
        //aubio detects sampleRate automatically if 0
        unsigned int sampleRate = 0;

        aubio_source_t* source = new_aubio_source(audioFilePath.c_str(), sampleRate, hopSize);
        if (!source)
        {
            std::cerr << "Error: Failed to open audio source!" << std::endl;
            return {};
        }

        sampleRate = aubio_source_get_samplerate(source);

        aubio_onset_t* onset = new_aubio_onset(onsetMethod.c_str(), bufferSize, hopSize, sampleRate);
        if (!onset)
        {
            std::cerr << "Error: Failed to create onset object!" << std::endl;
            del_aubio_source(source);
            return {};
        }

        aubio_onset_set_threshold(onset, onsetThreshold);
        aubio_onset_set_minioi_s(onset, minInterOnsetInterval);

        std::vector<float> onsetPositions;
        fvec_t* audio_frame = new_fvec(hopSize);
        fvec_t* onset_output = new_fvec(1);

        uint_t read = 0;
        while (true)
        {
            aubio_source_do(source, audio_frame, &read);
            if (read == 0) break;

            aubio_onset_do(onset, audio_frame, onset_output);

            if (fvec_get_sample(onset_output, 0) > 0.0f)
            {
                onsetPositions.push_back(aubio_onset_get_last_s(onset));
            }
        }

        del_fvec(onset_output);
        del_fvec(audio_frame);
        del_aubio_onset(onset);
        del_aubio_source(source);
        aubio_cleanup();

        return onsetPositions;
    }


    std::vector<float> AudioAnalysis::generateBeatTimestamps(const float songLength, const float beatInterval,
                                                             const float& xOffset)
    {
        std::vector<float> beatTimestamps;

        if (songLength <= 0.0f || beatInterval <= 0.0)
        {
            std::cerr << "Invalid song length or beat interval!" << std::endl;
            return beatTimestamps;
        }

        // Convert song length to number of beats
        const int totalBeats = static_cast<int>(songLength / beatInterval);

        // Generate timestamps
        for (int i = 0; i <= totalBeats; ++i)
        {
            beatTimestamps.push_back(static_cast<float>(i) * beatInterval + xOffset);
        }

        return beatTimestamps;
    }
}
