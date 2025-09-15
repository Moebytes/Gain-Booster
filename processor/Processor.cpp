#include "Processor.h"
#include "Editor.h"
#include "BPM.hpp"
#include "Functions.hpp"

Processor::Processor() : AudioProcessor(
    BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ), params(tree) {
}

Processor::~Processor() {}

auto Processor::prepareToPlay(double sampleRate, [[maybe_unused]] int samplesPerBlock) -> void {
    params.prepareToPlay(sampleRate);
    params.reset();
}

auto Processor::releaseResources() -> void {}

auto Processor::processBlock(juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages) -> void {
    juce::ScopedNoDenormals noDenormals;

    params.init();

    auto mainInput  = getBusBuffer(buffer, true, 0);
    auto mainOutput = getBusBuffer(buffer, false, 0);

    const float* inputL = mainInput.getReadPointer(0);
    const float* inputR = mainInput.getNumChannels() > 1 ? mainInput.getReadPointer(1) : inputL;

    float* outputL = mainOutput.getWritePointer(0);
    float* outputR = mainOutput.getNumChannels() > 1 ? mainOutput.getWritePointer(1) : outputL;

    auto [bpm, ppq, hostRunning] = BPM::getBPMAndPPQ(getPlayHead());
    params.setHostInfo(bpm, ppq, hostRunning);

    for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
        params.update();

        float gain = params.gain * params.boost;

        outputL[sample] = inputL[sample] * gain * params.panL;
        outputR[sample] = inputR[sample] * gain * params.panR;
    }
 
    #if JUCE_DEBUG
        Functions::checkAudioSafety(buffer);
    #endif
}

auto Processor::isBusesLayoutSupported(const BusesLayout& layouts) const -> bool {
    auto mono = juce::AudioChannelSet::mono();
    auto stereo = juce::AudioChannelSet::stereo();
    auto mainIn = layouts.getMainInputChannelSet();
    auto mainOut = layouts.getMainOutputChannelSet();

    if (mainIn == mono && mainOut == mono) return true;
    if (mainIn == mono && mainOut == stereo) return true;
    if (mainIn == stereo && mainOut == stereo) return true;
    return false;
}

auto Processor::getNumPrograms() -> int {
    return 1;
}

auto Processor::getCurrentProgram() -> int {
    return 0;
}

auto Processor::setCurrentProgram([[maybe_unused]] int index) -> void {}

auto Processor::getProgramName([[maybe_unused]] int index) -> const juce::String {
    return {};
}

auto Processor::changeProgramName([[maybe_unused]] int index, [[maybe_unused]] const juce::String& newName) -> void {
}

auto Processor::createEditor() -> juce::AudioProcessorEditor* {
    return new Editor(*this);
}

auto Processor::savePreset(const juce::String& name = "", const juce::String& author = "") -> juce::String {
    auto obj = std::make_unique<juce::DynamicObject>();

    obj->setProperty("plugin", JucePlugin_Name);
    obj->setProperty("version", JucePlugin_VersionString);
    obj->setProperty("name", name);
    obj->setProperty("author", author);
    obj->setProperty("modified", juce::Time::getCurrentTime().toISO8601(true));
    obj->setProperty("presetFormat", 1);

    auto parameters = std::make_unique<juce::DynamicObject>();

    for (const auto& id : params.paramIDs.getParamStringIDs()) {
        auto* param = tree.getParameter(id);

        if (id.containsIgnoreCase("LFORate") || id.containsIgnoreCase("LFOType")) {
            parameters->setProperty(id, param->getCurrentValueAsText());
        } else {
            parameters->setProperty(id, param->getValue());
        }
    }

    obj->setProperty("parameters", juce::var(parameters.release()));
    auto json = juce::var{obj.release()};

    return juce::JSON::toString(json);
}

auto Processor::loadPreset(const juce::String& jsonStr) -> juce::String {
    auto parsed = juce::JSON::fromString(jsonStr);
    auto* obj = parsed.getDynamicObject();
    if (obj == nullptr) return "";

    juce::String presetName = "";

    if (obj->hasProperty("name")) {
        presetName = obj->getProperty("name").toString();
    }

    auto parameters = juce::var{obj->getProperty("parameters")};
    auto* paramObj = parameters.getDynamicObject();
    if (paramObj == nullptr) return "";

    for (const auto& property : paramObj->getProperties()) {
        auto id = property.name.toString();
        auto* param = tree.getParameter(id);
        if (param) {
            if ((id.containsIgnoreCase("LFORate") || id.containsIgnoreCase("LFOType")) && property.value.isString()) {
                auto value = param->getValueForText(property.value.toString());
                param->setValueNotifyingHost(value);
            } else {
                param->setValueNotifyingHost(property.value);
            }
        }
    }

    return presetName;
}

auto Processor::initPreset() -> void {
    for (const auto& id : params.paramIDs.getParamStringIDs()) {
        auto* param = tree.getParameter(id);
        param->setValueNotifyingHost(param->getDefaultValue());
    }
}

auto Processor::getStateInformation(juce::MemoryBlock& destData) -> void {
    auto jsonStr = savePreset();
    destData.replaceAll(jsonStr.toUTF8(), jsonStr.getNumBytesAsUTF8());
}

auto Processor::setStateInformation(const void* data, int sizeInBytes) -> void {
    auto jsonStr = juce::String::fromUTF8(static_cast<const char*>(data), sizeInBytes);
    loadPreset(jsonStr);
}

auto JUCE_CALLTYPE createPluginFilter() -> juce::AudioProcessor* {
    return new Processor();
}