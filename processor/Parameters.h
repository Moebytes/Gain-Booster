#pragma once
#include <JuceHeader.h>
#include "ParameterIDs.h"
#include "LFO.h"
#include "BPM.h"

class Parameters {
public:
    Parameters(juce::AudioProcessorValueTreeState& tree);

    static auto createParameterLayout() -> juce::AudioProcessorValueTreeState::ParameterLayout;

    auto prepareToPlay(double sampleRate) noexcept -> void;
    auto reset() noexcept -> void;
    auto init(const juce::AudioPlayHead* playhead) noexcept -> void;
    auto update() noexcept -> void;
    auto updateBPM(const juce::AudioPlayHead* playhead) noexcept -> void;

    static ParameterIDs paramIDs;

    float gain = 1.0f;
    juce::AudioParameterFloat* gainParam;
    juce::AudioParameterChoice* gainSkewParam;

    float boost = 0.0f;
    juce::AudioParameterFloat* boostParam;
    juce::AudioParameterChoice* boostSkewParam;

    float pan = 0.0f;
    float panL = 0.0f;
    float panR = 1.0f;
    juce::AudioParameterFloat* panParam;
    juce::AudioParameterChoice* panningLawParam;

    juce::AudioParameterChoice* gainLFOTypeParam;
    juce::AudioParameterFloat* gainLFORateParam;
    juce::AudioParameterFloat*  gainLFOAmountParam;

    juce::AudioParameterChoice* panLFOTypeParam;
    juce::AudioParameterFloat* panLFORateParam;
    juce::AudioParameterFloat*  panLFOAmountParam;

private:
    juce::AudioProcessorValueTreeState& treeRef;
    
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::LinearSmoothedValue<float> boostSmoother;
    juce::LinearSmoothedValue<float> panSmoother;

    LFO gainLFO;
    LFO panLFO;
    BPM bpmInfo;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};