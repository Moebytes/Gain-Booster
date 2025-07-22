#pragma once
#include <JuceHeader.h>

const juce::ParameterID gainParamID {"gain", 1};

class Parameters {
public:
    Parameters(juce::AudioProcessorValueTreeState& apvts);

    static auto createParameterLayout() -> juce::AudioProcessorValueTreeState::ParameterLayout;

    auto update() noexcept -> void;
    auto prepareToPlay(double sampleRate) noexcept -> void;
    auto reset() noexcept -> void;
    auto smoothen() noexcept -> void;

    std::atomic<float> gain = 0.0f;
    juce::AudioParameterFloat* gainParam;
private:
    juce::LinearSmoothedValue<float> gainSmoother;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};