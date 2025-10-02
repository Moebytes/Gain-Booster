#pragma once
#include <JuceHeader.h>

class PresetManager {
public:
    PresetManager(AudioProcessorValueTreeState& tree);
    ~PresetManager() = default;

    auto savePresetToFile() -> void;
    auto loadPresetFromFile(std::function<void()> onComplete) -> void;
    auto loadFactoryPresets() -> void;
    auto addUserFolder() -> void;
    auto removeUserFolder() -> void;
    auto loadUserPresets() -> void;
    auto setPreset(int presetIndex) -> String;
    auto savePreset(const String& name = "", const String& author = "") -> String;
    auto loadPreset(const String& jsonStr) -> String;
    auto initPreset() -> void;
        
    auto openPresetMenu(const Array<var>& args, 
        WebBrowserComponent::NativeFunctionCompletion completion) -> void;

    auto prevPreset(const Array<var>& args, 
        WebBrowserComponent::NativeFunctionCompletion completion) -> void;

    auto nextPreset(const Array<var>& args, 
        WebBrowserComponent::NativeFunctionCompletion completion) -> void;

    String currentPresetName = "Default";
    std::map<String, String> factoryPresets;
    std::vector<String> factoryPresetNames;
    std::map<String, String> userPresets;
    std::vector<String> userPresetNames;
    int presetIndex = 0;
    String presetFolder = "none";

private:
    AudioProcessorValueTreeState& tree;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};