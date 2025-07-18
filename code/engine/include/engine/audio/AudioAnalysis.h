#pragma once
#include <vector>
#include <xstring>


namespace gl3::engine
{
    /**
     * Provides methods to analyze the beats per minute of an audio track and generate beat time stamps.
     */
    class AudioAnalysis
    {
    public:
        /**
    *Returns average bpm of an audio file.
         * @param audioFilePath The path to the audio file to analyze
         * @param hopSize The number of samples processed per analysis step
         * @param bufferSize The size of the analysis window, in samples, used for each analysis step.
         * @return The beats per minute (bpm) of the analyzed song
         */
        static float analyzeAudioTempo(const std::string& audioFilePath, unsigned hopSize = 512,
                                       unsigned bufferSize = 2048);

        /**
         * @brief Analyze an audio file and detect onset (transient) positions.
         *
         * This method uses the Aubio library to detect onsets (sudden changes in the audio signal,
         * such as drum hits, note attacks, or other transients). It processes the audio in blocks,
         * runs the selected onset detection algorithm, and returns the timestamps (in seconds)
         * of all detected onsets.
         *
         * @param audioFilePath Path to the input audio file (e.g., WAV or MP3).
         * @param hopSize Number of samples to process per analysis hop (controls how often analysis is done).
         * @param bufferSize Number of samples in the analysis window (controls frequency/time resolution trade-off).
         * @param onsetMethod Onset detection method to use (e.g., "default", "energy", "complex", "hfc", "specflux").
         * @param onsetThreshold Sensitivity threshold for onset detection. Higher values = fewer detections.
         * @param minInterOnsetInterval Minimum allowed time between consecutive onsets in seconds (suppresses double triggers).
         *
         * @return A vector of onset timestamps in seconds, relative to the start of the audio file.
         *
         * Example:
         * @code
         * std::vector<float> onsets = analyzeAudioOnsets("beat.wav", 512, 2048, "complex", 0.3f, 0.02f);
         * @endcode
        */
        static std::vector<float> analyzeAudioOnsets(const std::string& audioFilePath, unsigned int hopSize,
                                                     unsigned int bufferSize, const std::string& onsetMethod,
                                                     float onsetThreshold, float minInterOnsetInterval);
        /**
    *Returns vector with general beat time stamps in seconds, according to song length, beatInterval and an optional xOffset/delay.
         * @param songLength The length of the (analyzed) audio in seconds
         * @param beatInterval The time, each beat takes in seconds
         * @param xOffset An "x-offset" in seconds to add to each beat -> shifts them right
         * @return A vector with timestamps for each beat in seconds
         */
        static std::vector<float> generateBeatTimestamps(float songLength, float beatInterval, const float& xOffset);
    };
}
