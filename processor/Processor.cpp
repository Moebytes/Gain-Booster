#include "Processor.h"
#include "Editor.h"
#include "BPM.h"
#include "CheckAudioSafety.h"

Processor::Processor() : AudioProcessor(
    BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
    ), params(tree) {
}

Processor::~Processor() {
}

auto Processor::prepareToPlay(double sampleRate, [[maybe_unused]] int samplesPerBlock) -> void {
    params.prepareToPlay(sampleRate);
    params.reset();
}

auto Processor::releaseResources() -> void {
}

auto Processor::processBlock(juce::AudioBuffer<float>& buffer, [[maybe_unused]] juce::MidiBuffer& midiMessages) -> void {
    juce::ScopedNoDenormals noDenormals;

    params.init();

    auto mainInput  = getBusBuffer(buffer, true, 0);
    auto mainOutput = getBusBuffer(buffer, false, 0);

    const float* inputL = mainInput.getReadPointer(0);
    [[maybe_unused]] const float* inputR = mainInput.getNumChannels() > 1 ? mainInput.getReadPointer(1) : inputL;

    float* outputL = mainOutput.getWritePointer(0);
    float* outputR = mainOutput.getNumChannels() > 1 ? mainOutput.getWritePointer(1) : outputL;

    auto [bpm, ppq, hostRunning] = BPM::getBPMAndPPQ(getPlayHead());

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
        double samplePPQ = ppq + (sample / (getSampleRate() * 60.0 / bpm));

        params.setHostInfo(samplePPQ, bpm, hostRunning);
        params.update();

        float gain = params.gain * params.boost;

        outputL[sample] = inputL[sample] * gain * params.panL;
        outputR[sample] = inputR[sample] * gain * params.panR;
    }
 
    #if JUCE_DEBUG
        checkAudioSafety(buffer);
    #endif
}

auto Processor::isBusesLayoutSupported(const BusesLayout& layouts) const -> bool {
    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();
    const auto mainIn = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();

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

auto Processor::setCurrentProgram([[maybe_unused]] int index) -> void {
}

auto Processor::getProgramName([[maybe_unused]] int index) -> const juce::String {
    return {};
}

auto Processor::changeProgramName([[maybe_unused]] int index, [[maybe_unused]] const juce::String& newName) -> void {
}

auto Processor::createEditor() -> juce::AudioProcessorEditor* {
    return new Editor(*this);
}

auto Processor::getStateInformation(juce::MemoryBlock& destData) -> void {
    copyXmlToBinary(*tree.copyState().createXml(), destData);
}

auto Processor::setStateInformation(const void* data, int sizeInBytes) -> void {
    std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));

    if (xml.get() != nullptr && xml->hasTagName(tree.state.getType())) {
        tree.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

auto JUCE_CALLTYPE createPluginFilter() -> juce::AudioProcessor* {
    return new Processor();
}