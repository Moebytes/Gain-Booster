#pragma once
#include <JuceHeader.h>

class BPM {
public:

    auto reset() noexcept -> void {
        bpm = 120.0;
    }
    
    auto update(const juce::AudioPlayHead* playhead) noexcept -> void {
        reset();
        if (playhead == nullptr) return;

        const auto info = playhead->getPosition();
        if (!info.hasValue()) return;

        const auto& pos = *info;
        if (pos.getBpm().hasValue()) {
            bpm = *pos.getBpm();
        }
    }

    auto getBPM() const noexcept -> double {
        return bpm;
    }
private:
    double bpm = 120.0;
};