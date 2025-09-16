#pragma once
#include <JuceHeader.h>

using TimeSignature = juce::AudioPlayHead::TimeSignature;

class BPM {
public:
    static auto getHostInfo(const juce::AudioPlayHead* playhead) noexcept -> std::tuple<double, double, TimeSignature, bool> {
        double bpm = 150.0;
        double ppq = 0.0;
        TimeSignature timeSignature{4, 4};
        bool isPlaying = false;

        if (playhead != nullptr) {
            juce::AudioPlayHead::PositionInfo info;
            if (playhead->getPosition()) {
                bpm = info.getBpm().orFallback(150.0);
                ppq = info.getPpqPosition().orFallback(0.0);
                timeSignature = info.getTimeSignature().orFallback(TimeSignature{4, 4});
                isPlaying = info.getIsPlaying();
            }
        }

        return {bpm, ppq, timeSignature, isPlaying};
    }
};
