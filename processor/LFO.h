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
        this->freePhase = 0.0f;
    }

    auto setType(const juce::String& newType) -> void {
        this->type = newType.toLowerCase();
    }

    auto setRateHz(float newFrequencyHz) -> void {
        jassert(newFrequencyHz >= 0.0f);
        this->frequencyHz = newFrequencyHz;
        this->phaseIncrement = frequencyHz / static_cast<float>(sampleRate);
        this->syncedToHost = false;
    }

    auto setSyncedRate(float beatsPerCycle) -> void {
        jassert(beatsPerCycle > 0.0f);
        this->syncedBeatsPerCycle = beatsPerCycle;
        this->syncedToHost = true;
    }

    auto syncFromHost(double ppq, double bpm) -> void {
        if (bpm <= 0.0 || syncedBeatsPerCycle <= 0.0) {
            syncedToHost = false;
            return;
        }
    
        const double beatsIntoCycle = std::fmod(ppq, syncedBeatsPerCycle);
        syncedPhase = static_cast<float>(beatsIntoCycle / syncedBeatsPerCycle);
    }

    auto getSample() -> float {
        float currentPhase = syncedToHost ? syncedPhase : freePhase;
        float value = renderWaveform(currentPhase);
    
        if (!syncedToHost) {
            this->freePhase += phaseIncrement;
            if (freePhase >= 1.0f)
                freePhase -= 1.0f;
        }
    
        return value;
    }

private:
    auto renderWaveform(float phase) const -> float {
        if (type == "sine") {
            return std::sin(phase * juce::MathConstants<float>::twoPi);
        } else if (type == "triangle") {
            return 4.0f * std::abs(phase - 0.5f) - 1.0f;
        } else if (type == "square") {
            return (phase < 0.5f) ? 1.0f : -1.0f;
        } else if (type == "saw") {
            return 2.0f * phase - 1.0f;
        }
        return 0.0f;
    }

    juce::String type{"square"};

    double sampleRate = 44100.0;

    float frequencyHz = 1.0f;
    float phaseIncrement = 0.0f;
    float freePhase = 0.0f;

    bool syncedToHost = false;
    float syncedBeatsPerCycle = 1.0f;
    float syncedPhase = 0.0f;
};