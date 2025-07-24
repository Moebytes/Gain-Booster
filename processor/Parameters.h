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
    juce::AudioParameterChoice* gainSkewParam;

    std::atomic<float> boost = 0.0f;
    juce::AudioParameterFloat* boostParam;
    juce::AudioParameterChoice* boostSkewParam;

    std::atomic<float> pan = 0.0f;
    float panL = 0.0f;
    float panR = 1.0f;
    juce::AudioParameterFloat* panParam;
    juce::AudioParameterChoice* panningLawParam;

private:
    juce::AudioProcessorValueTreeState& treeRef;
    
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::LinearSmoothedValue<float> boostSmoother;
    juce::LinearSmoothedValue<float> panSmoother;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};