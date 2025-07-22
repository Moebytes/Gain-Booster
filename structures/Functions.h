#pragma once
#include <JuceHeader.h>

class Functions {
public:
    static auto streamToVector(juce::InputStream& stream) -> std::vector<std::byte>;
    static auto getMimeForExtension(const String& extension) -> const char*;
};