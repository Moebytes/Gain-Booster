#include "Processor.h"
#include "Editor.h"
#include "BPM.hpp"
#include "Functions.hpp"

Processor::Processor() : AudioProcessor(
    BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ), parameters(tree), presetManager(tree) {
}

Processor::~Processor() {}

auto Processor::prepareToPlay(double sampleRate, int samplesPerBlock) -> void {
    this->parameters.prepareToPlay(sampleRate, samplesPerBlock);
    this->parameters.reset();
}

auto Processor::releaseResources() -> void {}

auto Processor::processBlock(juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages) -> void {
    juce::ScopedNoDenormals noDenormals;

    auto mainInput = this->getBusBuffer(buffer, true, 0);
    auto mainOutput = this->getBusBuffer(buffer, false, 0);

    const float* inputL = mainInput.getReadPointer(0);
    const float* inputR = mainInput.getNumChannels() > 1 ? mainInput.getReadPointer(1) : inputL;

    float* outputL = mainOutput.getWritePointer(0);
    float* outputR = mainOutput.getNumChannels() > 1 ? mainOutput.getWritePointer(1) : outputL;

    auto [bpm, ppq, timeSignature, isPlaying] = BPM::getHostInfo(getPlayHead());
    this->parameters.setHostInfo(bpm, ppq, timeSignature, isPlaying);
    this->parameters.blockUpdate();

    for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
        this->parameters.update();

        float gain = this->parameters.gain * this->parameters.boost;

        outputL[sample] = inputL[sample] * gain * this->parameters.panL;
        outputR[sample] = inputR[sample] * gain * this->parameters.panR;
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
    if (this->presetManager.presetFolder == "factory") {
        return static_cast<int>(this->presetManager.factoryPresets.size());
    } else if (this->presetManager.presetFolder == "user") {
        return static_cast<int>(this->presetManager.userPresets.size());
    }
    return 1;
}

auto Processor::getCurrentProgram() -> int {
    if (this->presetManager.presetFolder != "none") {
        return this->presetManager.presetIndex;
    }
    return 0;
}

auto Processor::setCurrentProgram(int index) -> void {
    this->presetManager.setPreset(index);
}

auto Processor::getProgramName(int index) -> const juce::String {
    if (this->presetManager.presetFolder == "factory") {
        return this->presetManager.factoryPresetNames[static_cast<size_t>(index)];
    } else if (this->presetManager.presetFolder == "user") {
        return this->presetManager.userPresetNames[static_cast<size_t>(index)];
    }
    return "Default";
}

auto Processor::changeProgramName([[maybe_unused]] int index, [[maybe_unused]] const juce::String& newName) -> void {}

auto Processor::createEditor() -> juce::AudioProcessorEditor* {
    return new Editor(*this);
}

auto Processor::getStateInformation(juce::MemoryBlock& destData) -> void {
    auto jsonStr = this->presetManager.savePreset();
    destData.replaceAll(jsonStr.toUTF8(), jsonStr.getNumBytesAsUTF8());
}

auto Processor::setStateInformation(const void* data, int sizeInBytes) -> void {
    auto jsonStr = juce::String::fromUTF8(static_cast<const char*>(data), sizeInBytes);
    this->presetManager.loadPreset(jsonStr);
}

auto JUCE_CALLTYPE createPluginFilter() -> juce::AudioProcessor* {
    return new Processor();
}