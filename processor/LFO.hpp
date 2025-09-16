#pragma once
#include <cmath>
#include <JuceHeader.h>

class LFO {
public:
    LFO() = default;
    virtual ~LFO() = default;

    auto prepareToPlay(double _sampleRate) -> void {
        this->sampleRate = _sampleRate;
        reset();
    }

    auto reset() -> void {
        this->phase = 0.0f;
    }

    auto setType(const juce::String& _type) -> void {
        this->type = _type.toLowerCase();
    }

    auto setBPM(double _bpm) -> void {
        this->bpm = _bpm;
    }

    auto setHzRate(float _frequencyHz) -> void {
        this->frequencyHz = _frequencyHz;
        this->phaseIncrement = this->frequencyHz / static_cast<float>(this->sampleRate);
    }

    auto setSyncedRate(float noteLength, const juce::AudioPlayHead::TimeSignature& timeSignature) -> void {
        float timeScale = static_cast<float>(timeSignature.numerator) / static_cast<float>(timeSignature.denominator);
        this->beatsPerCycle = static_cast<float>(noteLength) * 4.0f * timeScale;

        double secondsPerBeat = 60.0 / this->bpm;
        double samplesPerCycle = this->beatsPerCycle * secondsPerBeat * this->sampleRate;
        this->phaseIncrement = 1.0f / static_cast<float>(samplesPerCycle);
    }

    auto syncToHost(double ppq) -> void {
        if (this->retrigger) {
            double fractionalCycle = std::fmod(ppq / this->beatsPerCycle, 1.0);
            this->phase = static_cast<float>(fractionalCycle);
        }
    }

    auto getSample() -> float {
        float value = renderWaveform(this->phase);
        if (this->phaseInvert) value *= -1;

        if (!this->retrigger) {
            this->phase += this->phaseIncrement;
            if (this->phase >= 1.0f) this->phase -= 1.0f;
        }

        return value;
    }

    auto renderWaveform(float pos) -> float {
        if (this->type == "sine") {
            return std::sin(pos * juce::MathConstants<float>::twoPi);
        } else if (this->type == "triangle") {
            return 4.0f * std::abs(pos - 0.5f) - 1.0f;
        } else if (this->type == "square") {
            return (pos < 0.5f) ? 1.0f : -1.0f;
        } else if (this->type == "saw") {
            return 2.0f * pos - 1.0f;
        }

        return 0.0f;
    }

private:
    juce::String type = "square";

    double sampleRate = 44100.0;
    double bpm = 150.0;

    float frequencyHz = 1.0f;
    float phaseIncrement = 0.0f;
    float beatsPerCycle = 1.0f;
    float phase = 0.0f;

    bool retrigger = true;
    bool phaseInvert = true;
};