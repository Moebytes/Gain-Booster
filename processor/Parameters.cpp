#include "Parameters.h"

template<typename T>
static auto castParameter(const juce::AudioProcessorValueTreeState& tree, 
    const juce::ParameterID* id, T*& dest) -> void {
    dest = dynamic_cast<T*>(tree.getParameter(id->getParamID()));
}

template <typename T>
static auto resetParameter(const juce::AudioProcessorValueTreeState& tree, 
    const juce::AudioParameterFloat* param, T*& dest) -> void {
    *dest = tree.getParameter(param->getParameterID())->getDefaultValue();
}

ParameterIDs Parameters::paramIDs = ParameterIDs::loadFromJSON();

Parameters::Parameters(juce::AudioProcessorValueTreeState& tree) : treeRef(tree) {
    using ParamPair = std::pair<juce::AudioParameterFloat*&, const juce::ParameterID*>;

    auto parameters = std::vector<ParamPair>{
        {gainParam, &paramIDs.gain},
        {boostParam, &paramIDs.boost}
    };

    for (auto& [param, paramID] : parameters) {
        castParameter(treeRef, paramID, param);
    }
}

auto Parameters::createParameterLayout() -> juce::AudioProcessorValueTreeState::ParameterLayout {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.gain, "Gain", juce::NormalisableRange<float>{0.0f, 1.0f, 0.01f, 0.7f}, 1.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.boost, "Boost", juce::NormalisableRange<float>{0.0f, 12.0f, 0.01f}, 0.0f
    ));

    return layout;
}

auto Parameters::prepareToPlay(double sampleRate) noexcept -> void {
    const double duration = 0.02;

    const auto smoothers = std::vector{
        &gainSmoother,
        &boostSmoother
    };

    for (const auto& smoother : smoothers) {
        smoother->reset(sampleRate, duration);
    }
}

auto Parameters::reset() noexcept -> void {
    auto paramFloats = std::vector{
        std::pair{gainParam, &gain},
        std::pair{boostParam, &boost}
    };

    for (auto& [param, value] : paramFloats) {
        resetParameter(treeRef, param, value);
    }

    const auto smoothers = std::vector{
        std::pair{gainParam, &gainSmoother},
        std::pair{boostParam, &boostSmoother}
    };

    for (const auto& [param, smoother] : smoothers) {
        smoother->setCurrentAndTargetValue(param->get());
    }
}

auto Parameters::init() noexcept -> void {
    const auto smoothers = std::vector{
        std::pair{gainParam, &gainSmoother},
        std::pair{boostParam, &boostSmoother},
    };

    for (const auto& [param, smoother] : smoothers) {
        smoother->setTargetValue(param->get());
    }
}

auto Parameters::update() noexcept -> void {
    gain = gainSmoother.getNextValue();
    boost = juce::Decibels::decibelsToGain(boostSmoother.getNextValue());
}