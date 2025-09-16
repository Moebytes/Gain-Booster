#pragma once
#include <JuceHeader.h>

class PresetManager {
public:
    PresetManager(juce::AudioProcessorValueTreeState& tree);
    ~PresetManager() = default;

    auto savePresetToFile() -> void;
    auto loadPresetFromFile(std::function<void()> onComplete) -> void;
    auto loadFactoryPresets() -> void;
    auto addUserFolder() -> void;
    auto removeUserFolder() -> void;
    auto loadUserPresets() -> void;
    auto setPreset(int presetIndex) -> juce::String;
    auto savePreset(const juce::String& name = "", const juce::String& author = "") -> juce::String;
    auto loadPreset(const juce::String& jsonStr) -> juce::String;
    auto initPreset() -> void;
        
    auto openPresetMenu(const juce::Array<juce::var>& args, 
        juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void;

    auto prevPreset(const juce::Array<juce::var>& args, 
        juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void;

    auto nextPreset(const juce::Array<juce::var>& args, 
        juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void;

    juce::String currentPresetName = "Default";
    std::map<juce::String, juce::String> factoryPresets;
    std::vector<juce::String> factoryPresetNames;
    std::map<juce::String, juce::String> userPresets;
    std::vector<juce::String> userPresetNames;
    int presetIndex = 0;
    juce::String presetFolder = "none";

private:
    juce::AudioProcessorValueTreeState& tree;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};