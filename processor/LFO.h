#pragma once
#include <cmath>
#include <JuceHeader.h>

class LFO {
public:
    LFO() = default;

    auto prepareToPlay(double newSampleRate) -> void {
        sampleRate = newSampleRate;
        reset();
    }

    auto reset() -> void {
        phase = 0.0f;
    }

    auto setType(const juce::String& newType) -> void {
        type = newType.toLowerCase();
    }

    auto setHzRate(float newFrequencyHz) -> void {
        frequencyHz = newFrequencyHz * 0.5f;
        phaseIncrement = frequencyHz / static_cast<float>(sampleRate);
        syncedToHost = false;
    }

    auto setSyncedRate(float syncedRate) -> void {
        syncedBeatsPerCycle = syncedRate * 0.5f;
        syncedToHost = true;

        const auto secondsPerBeat = 60.0f / bpm;
        const auto samplesPerCycle = syncedBeatsPerCycle * static_cast<float>(sampleRate) * secondsPerBeat;
        syncedPhaseIncrement = 1.0f / static_cast<float>(samplesPerCycle);
    }

    auto setBPM(double newBPM) -> void {
        bpm = newBPM;
        if (syncedToHost) {
            setSyncedRate(syncedBeatsPerCycle);
        }
    }

    auto syncFromHost(double ppq) -> void {
        const double beatsIntoCycle = std::fmod(ppq, syncedBeatsPerCycle);
        phase = static_cast<float>(beatsIntoCycle / syncedBeatsPerCycle);
    }

    auto getSample() -> float {
        float value = renderWaveform(phase);

        phase += syncedToHost ? syncedPhaseIncrement : phaseIncrement;
        if (phase >= 1.0f) {
            phase -= 1.0f;
        }

        return value;
    }

    auto renderWaveform(float phase) -> float {
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

        return value;
    }

private:
    juce::String type{"square"};

    double sampleRate = 44100.0;
    double bpm = 150.0;

    float frequencyHz = 1.0f;
    float phaseIncrement = 0.0f;
    float syncedPhaseIncrement = 0.0f;

    float phase = 0.0f;
    bool syncedToHost = false;
    float syncedBeatsPerCycle = 1.0f;
};