#pragma once
#include <JuceHeader.h>
#include "Parameters.h"
#include "BPM.h"

class Processor : public juce::AudioProcessor {
public:
  Processor();
  ~Processor() override;

  auto prepareToPlay(double sampleRate, int samplesPerBlock) -> void override;
  auto releaseResources() -> void override;
  auto processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) -> void override;

  auto isBusesLayoutSupported (const BusesLayout& layouts) const -> bool override;
  auto createEditor() -> juce::AudioProcessorEditor* override;

  inline auto hasEditor() const -> bool override { return true; }
  inline auto getName() const -> const juce::String override { return JucePlugin_Name; }
  inline auto acceptsMidi() const -> bool override { return false; }
  inline auto producesMidi() const -> bool override { return false; }
  inline auto isMidiEffect() const -> bool override { return false; }
  inline auto getTailLengthSeconds() const -> double override { return 0.0; }

  auto getNumPrograms() -> int override;
  auto getCurrentProgram() -> int override;
  auto setCurrentProgram(int index) -> void override;
  auto getProgramName(int index) -> const juce::String override;
  auto changeProgramName(int index, const juce::String& newName) -> void override;

  auto savePreset(const juce::String& name, const juce::String& author) -> juce::String;
  auto loadPreset(const juce::String& jsonStr) -> void;
  auto getStateInformation(juce::MemoryBlock& destData) -> void override;
  auto setStateInformation(const void* data, int sizeInBytes) -> void override;

  juce::AudioProcessorValueTreeState tree {
    *this, nullptr, "Parameters", Parameters::createParameterLayout()
  };

  Parameters params;

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Processor)
};