#pragma once
#include <JuceHeader.h>
#include "ParameterIDs.h"

class Parameters {
public:
    Parameters(juce::AudioProcessorValueTreeState& tree);

    static auto createParameterLayout() -> juce::AudioProcessorValueTreeState::ParameterLayout;

    auto prepareToPlay(double sampleRate) noexcept -> void;
    auto reset() noexcept -> void;
    auto init() noexcept -> void;
    auto update() noexcept -> void;

    static ParameterIDs paramIDs;

    std::atomic<float> gain = 1.0f;
    juce::AudioParameterFloat* gainParam;

    std::atomic<float> boost = 0.0f;
    juce::AudioParameterFloat* boostParam;
private:
    juce::AudioProcessorValueTreeState& treeRef;
    
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::LinearSmoothedValue<float> boostSmoother;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};