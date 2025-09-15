#include "Parameters.h"
#include "DSP.hpp"
#include "Functions.hpp"

template<typename T>
static auto castParameter(const juce::AudioProcessorValueTreeState& tree, 
    const juce::ParameterID* id, T*& dest) -> void {
    dest = dynamic_cast<T*>(tree.getParameter(id->getParamID()));
    jassert(dest != nullptr);
}

template <typename T>
static auto resetParameter(const juce::AudioProcessorValueTreeState& tree, 
    const juce::AudioParameterFloat* param, T*& dest) -> void {
    auto* paramObj = tree.getParameter(param->getParameterID());
    if (paramObj) *dest = paramObj->getDefaultValue();
}

template <typename T>
static auto resetParameter(const juce::AudioProcessorValueTreeState& tree, 
    const juce::AudioParameterChoice* param, T*& dest) -> void {
    auto* paramObj = tree.getParameter(param->getParameterID());
    if (paramObj) *dest = static_cast<T>(paramObj->getDefaultValue());
}

ParameterIDs Parameters::paramIDs = ParameterIDs::loadFromJSON();

Parameters::Parameters(juce::AudioProcessorValueTreeState& tree) : treeRef(tree) {
    using FloatPair = std::pair<juce::AudioParameterFloat*&, const juce::ParameterID*>;
    using ChoicePair = std::pair<juce::AudioParameterChoice*&, const juce::ParameterID*>;

    auto floatParameters = std::vector<FloatPair>{
        {gainParam, &paramIDs.gain},
        {boostParam, &paramIDs.boost},
        {panParam, &paramIDs.pan},
        {gainLFORateParam, &paramIDs.gainLFORate},
        {gainLFOAmountParam, &paramIDs.gainLFOAmount},
        {panLFORateParam, &paramIDs.panLFORate},
        {panLFOAmountParam, &paramIDs.panLFOAmount}
    };

    auto choiceParameters = std::vector<ChoicePair>{
        {gainCurveParam, &paramIDs.gainCurve},
        {boostCurveParam, &paramIDs.boostCurve},
        {panningLawParam, &paramIDs.panningLaw},
        {gainLFOTypeParam, &paramIDs.gainLFOType},
        {panLFOTypeParam, &paramIDs.panLFOType}
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
        paramIDs.gain, "Gain", juce::NormalisableRange<float>{0.0f, 1.0f, 0.01f}, 1.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayPercent)
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.gainCurve, "Gain Curve", juce::StringArray{"logarithmic", "linear", "exponential"}, 1
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.boost, "Boost", juce::NormalisableRange<float>{0.0f, 12.0f, 0.01f}, 0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayDecibels)
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.boostCurve, "Boost Curve", juce::StringArray{"logarithmic", "linear", "exponential"}, 1
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.pan, "Pan", juce::NormalisableRange<float>{-1.0f, 1.0f, 0.01f}, 0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayPan)
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.panningLaw, "Panning Law", juce::StringArray{"constant", "triangle", "linear"}, 1
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.gainLFOType, "Gain LFO Type", juce::StringArray{"square", "saw", "triangle", "sine"}, 0
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.gainLFORate, "Gain LFO Rate", juce::NormalisableRange<float>{0.03125f, 4.0f, 0.0001f}, 0.25f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayLFORate)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.gainLFOAmount, "Gain LFO Amount", juce::NormalisableRange<float>{0.0f, 1.0f, 0.01f}, 0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayPercent)
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.panLFOType, "Pan LFO Type", juce::StringArray{"square", "saw", "triangle", "sine"}, 0
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.panLFORate, "Pan LFO Rate", juce::NormalisableRange<float>{0.03125f, 4.0f, 0.0001f}, 0.25f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayLFORate)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.panLFOAmount, "Pan LFO Amount", juce::NormalisableRange<float>{0.0f, 1.0f, 0.01f}, 0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayPercent)
    ));

    return layout;
}

auto Parameters::prepareToPlay(double sampleRate) noexcept -> void {
    double duration = 0.001;

    auto smoothers = std::vector{
        &gainSmoother,
        &boostSmoother,
        &panSmoother,
        &gainLFOAmountSmoother,
        &panLFOAmountSmoother
    };

    for (const auto& smoother : smoothers) {
        smoother->reset(sampleRate, duration);
    }

    gainLFO.prepareToPlay(sampleRate);
    panLFO.prepareToPlay(sampleRate);
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
    
    auto smoothers = std::vector{
        std::pair{gainParam, &gainSmoother},
        std::pair{boostParam, &boostSmoother},
        std::pair{panParam, &panSmoother},
        std::pair{gainLFOAmountParam, &gainLFOAmountSmoother},
        std::pair{panLFOAmountParam, &panLFOAmountSmoother}
    };

    for (const auto& [param, smoother] : smoothers) {
        smoother->setCurrentAndTargetValue(param->get());
    }

    gainLFO.reset();
    panLFO.reset();
}

auto Parameters::init() noexcept -> void {
    auto smoothers = std::vector{
        std::pair{gainParam, &gainSmoother},
        std::pair{boostParam, &boostSmoother},
        std::pair{panParam, &panSmoother},
        std::pair{gainLFOAmountParam, &gainLFOAmountSmoother},
        std::pair{panLFOAmountParam, &panLFOAmountSmoother}
    };

    for (const auto& [param, smoother] : smoothers) {
        smoother->setTargetValue(param->get());
    }
}

auto Parameters::setHostInfo(double _bpm, double _ppq, bool _hostRunning) noexcept -> void {
    this->bpm = _bpm;
    this->ppq = _ppq;
    this->hostRunning = _hostRunning;

    if (hostRunning) {
        gainLFO.setBPM(bpm);
        panLFO.setBPM(bpm);
        gainLFO.syncFromHost(ppq);
        panLFO.syncFromHost(ppq);
    }
}

auto Parameters::update() noexcept -> void {
    gainLFO.setType(gainLFOTypeParam->getCurrentChoiceName());
    panLFO.setType(panLFOTypeParam->getCurrentChoiceName());

    float gainLFOSyncedTime = gainLFORateParam->get();
    float panLFOSyncedTime = panLFORateParam->get();

    if (hostRunning) {
        gainLFO.setSyncedRate(gainLFOSyncedTime);
        panLFO.setSyncedRate(panLFOSyncedTime);
    } else {
        gainLFO.setHzRate(static_cast<float>(bpm / 60.0 / gainLFOSyncedTime));
        panLFO.setHzRate(static_cast<float>(bpm / 60.0 / panLFOSyncedTime));
    }

    float gainLFOValue = gainLFO.getSample();
    float panLFOValue = panLFO.getSample();

    float gainLFOAmount = gainLFOAmountSmoother.getNextValue();
    float panLFOAmount = panLFOAmountSmoother.getNextValue();

    const auto& gainCurve = gainCurveParam->getCurrentChoiceName();
    gain = gainSmoother.getNextValue();

    if (gainCurve == "logarithmic") {
        gain = std::pow(gain, 0.5f);
    } else if (gainCurve == "exponential") {
        gain = std::pow(gain, 2.0f);
    }

    gain *= juce::jmap(gainLFOValue, -1.0f, 1.0f, 1.0f - gainLFOAmount, 1.0f);

    const auto& boostCurve = boostCurveParam->getCurrentChoiceName();

    float boostdB = boostSmoother.getNextValue();

    if (boostCurve == "logarithmic") {
        boostdB = std::pow(boostdB / 12.0f, 0.5f) * 12.0f;
    } else if (boostCurve == "exponential") {
        boostdB = std::pow(boostdB / 12.0f, 2.0f) * 12.0f;
    }

    boost = juce::Decibels::decibelsToGain(boostdB);

    const auto& panningLaw = panningLawParam->getCurrentChoiceName();
    pan = panSmoother.getNextValue();
    pan = juce::jlimit(-1.0f, 1.0f, pan + panLFOValue * panLFOAmount * 0.5f);

    if (panningLaw == "triangle") {
        trianglePanning(pan, panL, panR);
    } else if (panningLaw == "linear") {
        linearPanning(pan, panL, panR);
    } else {
        constantPowerPanning(pan, panL, panR);
    }
}