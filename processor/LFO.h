#pragma once
#include <cmath>
#include <JuceHeader.h>

class LFO {
public:
    LFO() = default;

    auto prepareToPlay(double newSampleRate) -> void {
        this->sampleRate = newSampleRate;
        this->reset();
    }

    auto reset() -> void {
        phase = 0.0;
    }

    auto setType(const juce::String& newType) -> void {
        type = newType.toLowerCase();
    }

    auto setRate(float newFrequency) -> void {
        jassert(newFrequency >= 0.0f);
        frequency = newFrequency;
        phaseIncrement = frequency / static_cast<float>(sampleRate);
    }

    auto nextSample() -> float {
        float value = 0.0f;

        if (type == "sine") {
            value = std::sin(phase * juce::MathConstants<float>::twoPi);
        } else if (type == "triangle") {
            value = 4.0f * std::abs(phase - 0.5f) - 1.0f;
        } else if (type == "square") {
            value = (phase < 0.5f) ? 1.0f : -1.0f;
        } else if (type == "saw") {
            value = 2.0f * phase - 1.0f;
        }

        phase += phaseIncrement;
        if (phase >= 1.0f)
            phase -= 1.0f;

        return value;
    }

private:
    juce::String type {"square"};
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float frequency = 0.0f;
    float phaseIncrement = 0.0f;
};