#pragma once
#include <JuceHeader.h>

inline auto checkAudioSafety(juce::AudioBuffer<float>& buffer) -> void {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {

        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {

            float value = channelData[sample];
            if (std::isnan(value)) {
                DBG("NaN detected");
                return buffer.clear();
            } else if (std::isinf(value)) {
                DBG("Inf detected");
                return buffer.clear();
            } else if (value < -2.0f || value > 2.0f) {
                DBG("Sample out of range");
                return buffer.clear();
            }
        }
    }
}