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

Parameters::Parameters(juce::AudioProcessorValueTreeState& tree) : tree(tree) {
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
        castParameter(tree, paramID, param);
    }

    for (auto& [param, paramID] : choiceParameters) {
        castParameter(tree, paramID, param);
    }
}

auto Parameters::createParameterLayout() -> juce::AudioProcessorValueTreeState::ParameterLayout {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.gain, "Gain", juce::NormalisableRange<float>{0.0f, 1.0f, 0.01f}, 1.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayPercent)
        .withValueFromStringFunction(Functions::parsePercent)
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.gainCurve, "Gain Curve", juce::StringArray{"logarithmic", "linear", "exponential"}, 1
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.boost, "Boost", juce::NormalisableRange<float>{0.0f, 12.0f, 0.01f}, 0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayDecibels)
        .withValueFromStringFunction(Functions::parseDecibels)
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.boostCurve, "Boost Curve", juce::StringArray{"logarithmic", "linear", "exponential"}, 1
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.pan, "Pan", juce::NormalisableRange<float>{-1.0f, 1.0f, 0.01f}, 0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayPan)
        .withValueFromStringFunction(Functions::parsePan)
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
        .withValueFromStringFunction(Functions::parseLFORate)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.gainLFOAmount, "Gain LFO Amount", juce::NormalisableRange<float>{0.0f, 1.0f, 0.01f}, 0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayPercent)
        .withValueFromStringFunction(Functions::parsePercent)
    ));

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        paramIDs.panLFOType, "Pan LFO Type", juce::StringArray{"square", "saw", "triangle", "sine"}, 0
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.panLFORate, "Pan LFO Rate", juce::NormalisableRange<float>{0.03125f, 4.0f, 0.0001f}, 0.25f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayLFORate)
        .withValueFromStringFunction(Functions::parseLFORate)
    ));

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        paramIDs.panLFOAmount, "Pan LFO Amount", juce::NormalisableRange<float>{0.0f, 1.0f, 0.01f}, 0.0f,
        juce::AudioParameterFloatAttributes().withStringFromValueFunction(Functions::displayPercent)
        .withValueFromStringFunction(Functions::parsePercent)
    ));

    return layout;
}

auto Parameters::getDefaultParameter(const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {

    auto paramID = args[0].toString();
    auto* param = this->tree.getParameter(paramID);
    float defaultValue = param->convertFrom0to1(param->getDefaultValue());

    completion(defaultValue);
}

auto Parameters::prepareToPlay(double sampleRate, int blockSize) noexcept -> void {
    this->sampleRate = sampleRate;
    this->blockSize = blockSize;
    double duration = 0.001;

    auto smoothers = std::vector{
        &gainSmoother,
        &boostSmoother,
        &panSmoother,
        &gainLFOAmountSmoother,
        &panLFOAmountSmoother
    };

    for (const auto& smoother : smoothers) {
        smoother->reset(this->sampleRate, duration);
    }

    this->gainLFO.prepareToPlay(this->sampleRate);
    this->panLFO.prepareToPlay(this->sampleRate);
}

auto Parameters::reset() noexcept -> void {
    auto paramFloats = std::vector{
        std::pair{gainParam, &gain},
        std::pair{boostParam, &boost},
        std::pair{panParam, &pan}
    };

    for (auto& [param, value] : paramFloats) {
        resetParameter(tree, param, value);
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

    this->gainLFO.reset();
    this->panLFO.reset();
}

auto Parameters::setHostInfo(double bpm, double ppq, const juce::AudioPlayHead::TimeSignature& timeSignature) noexcept -> void {
    this->bpm = bpm;
    this->ppq = ppq;
    this->timeSignature = timeSignature;

    if (ppq > 0.0) {
        this->ppq = ppq;
        this->internalPPQ = ppq;
    } else {
        double ppqPerSample = (this->bpm / 60.0) / this->sampleRate;
        this->internalPPQ += ppqPerSample * this->blockSize; 
        this->ppq = this->internalPPQ;
    }

    this->gainLFO.syncToHost(this->bpm, this->ppq, this->timeSignature);
    this->panLFO.syncToHost(this->bpm, this->ppq, this->timeSignature);
}

auto Parameters::blockUpdate() noexcept -> void {
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

    this->gainLFO.setType(this->gainLFOTypeParam->getCurrentChoiceName());
    this->panLFO.setType(this->panLFOTypeParam->getCurrentChoiceName());

    float gainLFOSyncedTime = this->gainLFORateParam->get();
    float panLFOSyncedTime = this->panLFORateParam->get();

    this->gainLFO.setSyncedRate(gainLFOSyncedTime);
    this->panLFO.setSyncedRate(panLFOSyncedTime);
}

auto Parameters::update() noexcept -> void {
    const auto& gainCurve = this->gainCurveParam->getCurrentChoiceName();
    this->gain = gainSmoother.getNextValue();

    if (gainCurve == "logarithmic") {
        this->gain = std::pow(this->gain, 0.5f);
    } else if (gainCurve == "exponential") {
        this->gain = std::pow(this->gain, 2.0f);
    }

    float gainLFOValue = this->gainLFO.getSample();
    float gainLFOAmount = this->gainLFOAmountSmoother.getNextValue();
    this->gain *= juce::jmap(gainLFOValue, -1.0f, 1.0f, 1.0f - gainLFOAmount, 1.0f);

    const auto& boostCurve = this->boostCurveParam->getCurrentChoiceName();
    float boostdB = this->boostSmoother.getNextValue();

    if (boostCurve == "logarithmic") {
        boostdB = std::pow(boostdB / 12.0f, 0.5f) * 12.0f;
    } else if (boostCurve == "exponential") {
        boostdB = std::pow(boostdB / 12.0f, 2.0f) * 12.0f;
    }

    this->boost = juce::Decibels::decibelsToGain(boostdB);

    const auto& panningLaw = panningLawParam->getCurrentChoiceName();
    this->pan = this->panSmoother.getNextValue();

    float panLFOValue = this->panLFO.getSample();
    float panLFOAmount = this->panLFOAmountSmoother.getNextValue();
    this->pan = juce::jlimit(-1.0f, 1.0f, this->pan + panLFOValue * panLFOAmount * 0.5f);

    if (panningLaw == "triangle") {
        DSP::trianglePanning(this->pan, this->panL, this->panR);
    } else if (panningLaw == "linear") {
        DSP::linearPanning(this->pan, this->panL, this->panR);
    } else {
        DSP::constantPowerPanning(this->pan, this->panL, this->panR);
    }
}