#pragma once
#include <JuceHeader.h>

inline auto constantPowerPanning(float pan, float& panL, float& panR) -> void {
    float angle = (pan + 1.0f) * juce::MathConstants<float>::pi * 0.25f;
    panL = std::cos(angle);
    panR = std::sin(angle);
}

inline auto trianglePanning(float pan, float& panL, float& panR) -> void {
    float panPos = (pan + 1.0f) * 0.5f;
    
    if (panPos <= 0.5f) {
        panL = 1.0f;
        panR = 2.0f * panPos;
    } else {
        panL = 2.0f * (1.0f - panPos);
        panR = 1.0f;
    }
}

inline auto linearPanning(float pan, float& panL, float& panR) -> void {
    panL = 1.0f - (pan + 1.0f) * 0.5f;
    panR = (pan + 1.0f) * 0.5f;
}