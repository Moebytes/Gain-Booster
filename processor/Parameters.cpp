#include "Parameters.h"

template<typename T>
static auto castParameter(juce::AudioProcessorValueTreeState& apvts, 
    const juce::ParameterID& id, T& dest) -> void {
    dest = dynamic_cast<T>(apvts.getParameter(id.getParamID()));
    jassert(dest);
}

Parameters::Parameters(juce::AudioProcessorValueTreeState& apvts) {
    castParameter(apvts, gainParamID, gainParam);
}

static auto stringFromPercent(float value, int) -> juce::String {
    return juce::String::formatted("%.0f%%", value * 100);
}

static auto stringFromDecibels(float value, int) -> juce::String {
    if (value == 0.0f) return CharPointer_UTF8("-∞ dB");
    return juce::String::formatted("%.1f dB", juce::Decibels::gainToDecibels(value));
}

auto Parameters::createParameterLayout() -> juce::AudioProcessorValueTreeState::ParameterLayout {
    auto layout = juce::AudioProcessorValueTreeState::ParameterLayout{};

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        gainParamID, "Gain", juce::NormalisableRange<float> {0.0f, 1.0f, 0.01f, 0.7f}, 1.0f,
        juce::AudioParameterFloatAttributes()
            .withStringFromValueFunction(stringFromDecibels)
    ));
    return layout;
}


auto Parameters::prepareToPlay(double sampleRate) noexcept -> void {
    double duration = 0.02;
    gainSmoother.reset(sampleRate, duration);
}

auto Parameters::reset() noexcept -> void {
    gain = 0.0f;
    gainSmoother.setCurrentAndTargetValue(gainParam->get());
}

auto Parameters::update() noexcept -> void {
    gainSmoother.setTargetValue(gainParam->get());
}

auto Parameters::smoothen() noexcept -> void {
    gain = gainSmoother.getNextValue();
}