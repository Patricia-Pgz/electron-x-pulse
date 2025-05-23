#pragma once

constexpr float pixelsPerMeter = 100.0f;

struct UINeonColors
{
    static constexpr auto windowBgColor = ImVec4(0.36f, 0.09f, 0.45f, 0.9f);

    static constexpr auto softPastelPink = ImVec4(1.0f, 0.7f, 0.85f, 1.0f);
    static constexpr auto pastelNeonViolet = ImVec4(0.7f, 0.6f, 1.0f, 1.0f);
    static constexpr auto pastelNeonViolet2 = ImVec4(0.48f, 0.3f, 1.0f, 1.0f);

    static constexpr auto Cyan = ImVec4(0.0f, 1.0f, 1.0f, 1.0f);
    static constexpr auto Pink = ImVec4(1.0f, 0.2f, 0.8f, 1.0f);
    static constexpr auto Orange = ImVec4(1.0f, 0.5f, 0.0f, 1.0f);
    static constexpr auto Green = ImVec4(0.2f, 1.0f, 0.2f, 1.0f);
};
