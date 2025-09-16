#include "Processor.h"
#include "Editor.h"
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

auto Processor::getHostInfo() noexcept -> std::tuple<double, double, TimeSignature> {
    double bpm = 150.0;
    double ppq = 0.0;
    TimeSignature timeSignature{4, 4};

    if (auto* playhead = this->getPlayHead()) {
        auto info = playhead->getPosition().orFallback(juce::AudioPlayHead::PositionInfo{});
        bpm = info.getBpm().orFallback(150.0);
        ppq = info.getPpqPosition().orFallback(0.0);
        timeSignature = info.getTimeSignature().orFallback(TimeSignature{4, 4});
    }

    return {bpm, ppq, timeSignature};
}

auto Processor::processBlock(juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages) -> void {
    juce::ScopedNoDenormals noDenormals;

    auto mainInput = this->getBusBuffer(buffer, true, 0);
    auto mainOutput = this->getBusBuffer(buffer, false, 0);

    const float* inputL = mainInput.getReadPointer(0);
    const float* inputR = mainInput.getNumChannels() > 1 ? mainInput.getReadPointer(1) : inputL;

    float* outputL = mainOutput.getWritePointer(0);
    float* outputR = mainOutput.getNumChannels() > 1 ? mainOutput.getWritePointer(1) : outputL;

    auto [bpm, ppq, timeSignature] = this->getHostInfo();
    this->parameters.setHostInfo(bpm, ppq, timeSignature);
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
    return static_cast<int>(this->presetManager.factoryPresets.size());
}

auto Processor::getCurrentProgram() -> int {
    return this->presetManager.presetIndex;
}

auto Processor::setCurrentProgram(int index) -> void {
    this->presetManager.presetFolder = "factory";
    auto presetName = this->presetManager.setPreset(index);
    if (auto* activeEditor = this->getActiveEditor()) {
        auto* editor = dynamic_cast<Editor*>(activeEditor);
        editor->getWebview().emitEventIfBrowserIsVisible(juce::Identifier{"presetChanged"}, presetName);
    }
}

auto Processor::getProgramName(int index) -> const juce::String {
    int safeIndex = juce::jlimit(0, this->getNumPrograms() - 1, index);
    auto presetName = this->presetManager.factoryPresetNames[static_cast<size_t>(safeIndex)];
    return Functions::replaceChar(presetName, '/', '-');
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