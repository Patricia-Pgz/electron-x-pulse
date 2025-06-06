#pragma once
#include <vector>
#include <xstring>


namespace gl3::engine
{
    class AudioAnalysis
    {
    public:
        static float analyzeAudioTempo(const std::string& audioFile, unsigned hopSize = 512,
                                unsigned bufferSize = 2048);
        static std::vector<float> generateBeatTimestamps(float songLength, float beatInterval, const float& xOffset);
    };
}
