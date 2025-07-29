#pragma once
#include <JuceHeader.h>
#include "ParameterIDs.h"
#include "LFO.h"

class Parameters {
public:
    Parameters(juce::AudioProcessorValueTreeState& tree);
    ~Parameters() = default;

    static auto createParameterLayout() -> juce::AudioProcessorValueTreeState::ParameterLayout;

    auto prepareToPlay(double sampleRate) noexcept -> void;
    auto reset() noexcept -> void;
    auto init() noexcept -> void;
    auto update() noexcept -> void;
    auto setHostInfo(double bpm, double ppq, bool hostRunning) noexcept -> void;

    static ParameterIDs paramIDs;

    float gain = 1.0f;
    juce::AudioParameterFloat* gainParam;
    juce::AudioParameterChoice* gainCurveParam;

    float boost = 0.0f;
    juce::AudioParameterFloat* boostParam;
    juce::AudioParameterChoice* boostCurveParam;

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
    juce::LinearSmoothedValue<float> gainLFOAmountSmoother;
    juce::LinearSmoothedValue<float> panLFOAmountSmoother;

    LFO gainLFO;
    LFO panLFO;
    double bpm = 120.0;
    double ppq = 0.0;
    bool hostRunning = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};