#pragma once
#include <JuceHeader.h>
#include "ParameterIDs.hpp"
#include "LFO.hpp"

class Parameters {
public:
    Parameters(juce::AudioProcessorValueTreeState& tree);
    ~Parameters() = default;

    static auto createParameterLayout() -> juce::AudioProcessorValueTreeState::ParameterLayout;

    auto prepareToPlay(double sampleRate, int blockSize) noexcept -> void;
    auto reset() noexcept -> void;
    auto init() noexcept -> void;
    auto blockUpdate() noexcept -> void;
    auto update() noexcept -> void;
    auto setHostInfo(double bpm, double ppq, const juce::AudioPlayHead::TimeSignature& timeSignature) noexcept -> void;

    auto getDefaultParameter(const juce::Array<juce::var>& args, 
        juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void;

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
    juce::AudioProcessorValueTreeState& tree;
    
    juce::LinearSmoothedValue<float> gainSmoother;
    juce::LinearSmoothedValue<float> boostSmoother;
    juce::LinearSmoothedValue<float> panSmoother;
    juce::LinearSmoothedValue<float> gainLFOAmountSmoother;
    juce::LinearSmoothedValue<float> panLFOAmountSmoother;

    LFO gainLFO;
    LFO panLFO;
    double sampleRate = 44100.0;
    int blockSize = 512;
    double bpm = 150.0;
    double ppq = 0.0;
    double internalPPQ = 0.0;
    juce::AudioPlayHead::TimeSignature timeSignature{4, 4};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Parameters)
};