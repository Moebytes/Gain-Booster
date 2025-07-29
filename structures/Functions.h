#pragma once
#include <JuceHeader.h>

class Functions {
public:
    static auto streamToVector(juce::InputStream& stream) -> std::vector<std::byte>;
    static auto getMimeForExtension(const String& extension) -> const char*;
    static auto checkAudioSafety(juce::AudioBuffer<float>& buffer) -> void;
    static auto displayPercent(float value, int) -> juce::String;
    static auto displayDecibels(float value, int) -> juce::String;
    static auto displayPan(float value, int) -> juce::String;
    static auto displayLFORate(float value, int) -> juce::String;
    static auto getDownloadsFolder() -> juce::File;
    static auto cleanFilename(const juce::String& input) -> juce::String;
};