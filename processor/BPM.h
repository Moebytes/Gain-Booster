#pragma once
#include <JuceHeader.h>

class BPM {
public:
    static auto getBPMAndPPQ(const juce::AudioPlayHead* playhead) noexcept -> std::tuple<double, double, bool> {
        double bpm = 120.0;
        double ppq = 0.0;
        bool hostRunning = false;

        if (playhead != nullptr) {
            juce::AudioPlayHead::PositionInfo posInfo;
            if (playhead->getPosition()) {
                bpm = posInfo.getBpm().orFallback(120.0);
                ppq = posInfo.getPpqPosition().orFallback(0.0);
                hostRunning = posInfo.getIsPlaying();
            }
        }

        return {bpm, ppq, hostRunning};
    }
};
