#include "Parameters.h"
#include "DSP.h"

template<typename T>
static auto castParameter(const juce::AudioProcessorValueTreeState& tree, 
    const juce::ParameterID* id, T*& dest) -> void {
    dest = dynamic_cast<T*>(tree.getParameter(id->getParamID()));
    jassert(dest != nullptr);
}

template <typename T>
static auto resetParameter(const juce::AudioProcessorValueTreeState& tree, 
    const juce::AudioParameterFloat* param, T*& dest) -> void {
    if (auto* paramObj = tree.getParameter(param->getParameterID())) {
        *dest = paramObj->getDefaultValue();
    }
}

template <typename T>
static auto resetParameter(const juce::AudioProcessorValueTreeState& tree, 
    const juce::AudioParameterChoice* param, T*& dest) -> void {
    if (auto* paramObj = tree.getParameter(param->getParameterID())) {
        *dest = static_cast<T>(paramObj->getDefaultValue());
    }
}

ParameterIDs Parameters::paramIDs = ParameterIDs::loadFromJSON();

Parameters::Parameters(juce::AudioProcessorValueTreeState& tree) : treeRef(tree) {
    using FloatPair = std::pair<juce::AudioParameterFloat*&, const juce::ParameterID*>;
    using ChoicePair = std::pair<juce::AudioParameterChoice*&, const juce::ParameterID*>;

    auto floatParameters = std::vector<FloatPair>{
        {gainParam, &paramIDs.gain},
        {boostParam, &paramIDs.boost},
        {panParam, &paramIDs.pan}
    };

    auto choiceParameters = std::vector<ChoicePair>{
        {gainSkewParam, &paramIDs.gainSkew},
        {boostSkewParam, &paramIDs.boostSkew},
        {panningLawParam, &paramIDs.panningLaw}
    };

    for (auto& [param, paramID] : floatParameters) {
        castParameter(treeRef, paramID, param);
    }

    for (auto& [param, paramID] : choiceParameters) {
        castParameter(treeRef, paramID, param);
    }
}

auto Parameters::createParameterLayout() -> juce::AudioProcessorValueTreeState::ParameterLayout {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.gain, "Gain", juce::NormalisableRange<float>{0.0f, 1.0f, 0.01f}, 1.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.gainSkew, "Gain Skew", juce::StringArray{"logarithmic", "linear", "exponential"}, 1
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.boost, "Boost", juce::NormalisableRange<float>{0.0f, 12.0f, 0.01f}, 0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.boostSkew, "Boost Skew", juce::StringArray{"logarithmic", "linear", "exponential"}, 1
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.pan, "Pan", juce::NormalisableRange<float>{-1.0f, 1.0f, 0.01f}, 0.0f
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.panningLaw, "Panning Law", juce::StringArray{"constant", "triangle", "linear"}, 0
    ));

    return layout;
}

auto Parameters::prepareToPlay(double sampleRate) noexcept -> void {
    const double duration = 0.001;

    const auto smoothers = std::vector{
        &gainSmoother,
        &boostSmoother,
        &panSmoother
    };

    for (const auto& smoother : smoothers) {
        smoother->reset(sampleRate, duration);
    }
}

auto Parameters::reset() noexcept -> void {
    auto paramFloats = std::vector{
        std::pair{gainParam, &gain},
        std::pair{boostParam, &boost},
        std::pair{panParam, &pan}
    };

    for (auto& [param, value] : paramFloats) {
        resetParameter(treeRef, param, value);
    }

    const auto smoothers = std::vector{
        std::pair{gainParam, &gainSmoother},
        std::pair{boostParam, &boostSmoother},
        std::pair{panParam, &panSmoother}
    };

    for (const auto& [param, smoother] : smoothers) {
        smoother->setCurrentAndTargetValue(param->get());
    }
}

auto Parameters::init() noexcept -> void {
    const auto smoothers = std::vector{
        std::pair{gainParam, &gainSmoother},
        std::pair{boostParam, &boostSmoother},
        std::pair{panParam, &panSmoother},
    };

    for (const auto& [param, smoother] : smoothers) {
        smoother->setTargetValue(param->get());
    }
}

auto Parameters::update() noexcept -> void {

    const auto& gainSkew = gainSkewParam->getCurrentChoiceName();
    gain = gainSmoother.getNextValue();

    if (gainSkew == "logarithmic") {
        gain = std::pow(gain, 0.5f);
    } else if (gainSkew == "exponential") {
        gain = std::pow(gain, 2.0f);
    }

    const auto& boostSkew = boostSkewParam->getCurrentChoiceName();
    float boostdB = boostSmoother.getNextValue();
    float boostNorm = juce::jmap(boostdB, 0.0f, 12.0f, 0.0f, 1.0f);

    if (boostSkew == "logarithmic") {
        boostNorm = std::pow(boostNorm, 0.5f);
    } else if (boostSkew == "exponential") {
        boostNorm = std::pow(boostNorm, 2.0f);
    }

    boostdB = juce::jmap(boostNorm, 0.0f, 1.0f, 0.0f, 12.0f);
    boost = juce::Decibels::decibelsToGain(boostdB);

    const auto& panningLaw = panningLawParam->getCurrentChoiceName();
    pan = panSmoother.getNextValue();

    if (panningLaw == "triangle") {
        trianglePanning(pan, panL, panR);
    } else if (panningLaw == "linear") {
        linearPanning(pan, panL, panR);
    } else {
        constantPowerPanning(pan, panL, panR);
    }
}