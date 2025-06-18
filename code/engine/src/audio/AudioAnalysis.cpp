#include "engine/audio/AudioAnalysis.h"
#include "../aubio/src/aubio.h"
#include <iostream>

namespace gl3::engine
{
    /**
*Returns average bpm of an audio file.
*/
    float AudioAnalysis::analyzeAudioTempo(const std::string& audioFile, const unsigned int hopSize,
                                           const unsigned int bufferSize)
    {
        unsigned int sampleRate = 0;
        // Create aubio source object
        aubio_source_t* source = new_aubio_source(audioFile.c_str(), sampleRate, hopSize);
        if (!source)
        {
            std::cerr << "Error: Failed to open audio source!" << std::endl;
        }

        // Get actual sample rate
        sampleRate = aubio_source_get_samplerate(source);

        // Create aubio tempo object
        aubio_tempo_t* tempo = new_aubio_tempo("complex", bufferSize, hopSize, sampleRate);
        if (!tempo)
        {
            std::cerr << "Error: Failed to create tempo object!" << std::endl;
            del_aubio_source(source);
        }

        fvec_t* audio_frame = new_fvec(hopSize); // Audio input frame buffer
        fvec_t* beat_output = new_fvec(1); // Beat detection output buffer

        uint_t read = 0;
        while (true)
        {
            // Read a frame of audio
            aubio_source_do(source, audio_frame, &read);

            // Stop if no more frames
            if (read == 0) break;

            aubio_tempo_set_threshold(tempo, 0.9);

            // Analyze for beats
            aubio_tempo_do(tempo, audio_frame, beat_output);

            /*// Check if a beat was detected TODO als andere beatdetection anbieten
            if (fvec_get_sample(beat_output, 0) > 0.0f) {
                float beatTime = aubio_tempo_get_last_s(tempo);
                float beatPosition = initialPlayerPositionX + beatTime * -scrollSpeed;
                beatPositions.push_back(beatPosition);
                std::cout << "Beat detected at X position: " << beatPosition << std::endl;
            }*/
        }
        const auto bpm = aubio_tempo_get_bpm(tempo);
        del_fvec(beat_output);
        del_fvec(audio_frame);
        del_aubio_tempo(tempo);
        del_aubio_source(source);

        aubio_cleanup();

        return bpm;
    }

    /**
*Returns vector with general beat time stamps in seconds, according to songlength, beatInterval and an optional xOffset/delay.
*/
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
