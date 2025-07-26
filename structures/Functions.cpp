#include "Functions.h"

auto Functions::streamToVector(juce::InputStream& stream) -> std::vector<std::byte> {
    std::vector<std::byte> result(static_cast<size_t>(stream.getTotalLength()));
    stream.setPosition(0);
    [[maybe_unused]] const auto bytesRead = stream.read (result.data(), result.size());
    jassert (bytesRead == static_cast<ssize_t>(result.size()));
    return result;
}

auto Functions::getMimeForExtension(const String& extension) -> const char* {
    static const std::unordered_map<String, const char*> mimeMap = {
        {"html", "text/html"       },
        {"css",  "text/css"        },
        {"js",   "text/javascript" },
        {"txt",  "text/plain"      },
        {"jpg",  "image/jpeg"      },
        {"png",  "image/png"       },
        {"jpeg", "image/jpeg"      },
        {"svg",  "image/svg+xml"   },
        {"json", "application/json"},
        {"map",  "application/json"},
        {"ttf",  "font/ttf"        },
        {"otf",  "font/otf"        },
        {"woff2","font/woff2"      }
    };

    if (const auto it = mimeMap.find(extension.toLowerCase()); it != mimeMap.end()) {
        return it->second;
    }

    jassertfalse;
    return "";
}

auto Functions::checkAudioSafety(juce::AudioBuffer<float>& buffer) -> void {
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < buffer.getNumSamples(); ++sample) {
            float value = channelData[sample];
            if (std::isnan(value)) {
                DBG("NaN detected");
                return buffer.clear();
            } else if (std::isinf(value)) {
                DBG("Inf detected");
                return buffer.clear();
            } else if (value < -2.0f || value > 2.0f) {
                DBG("Sample out of range");
                return buffer.clear();
            }
        }
    }
}

auto Functions::displayPercent(float value, int) -> juce::String {
    return juce::String::formatted("%.0f%%", value * 100.0f);
}

auto Functions::displayDecibels(float value, int) -> juce::String  {
    return juce::String::formatted("%.1f dB", value);
}

auto Functions::displayPan(float value, int) -> juce::String {
    int position = static_cast<int>(value * 50.0f);
    if (position < 0) {
        return juce::String::formatted("%dL", -position);
    } else {
        return juce::String::formatted("%dR", position);
    }
}

auto Functions::displayLFORate(float value, int) -> juce::String {
    const float epsilon = 0.0001f;

    for (int numerator = 1; numerator <= 4; ++numerator) {
        for (int denominator = 1; denominator <= 32; ++denominator) {
            float candidate = static_cast<float>(numerator) / static_cast<float>(denominator);
            if (std::abs(value - candidate) < epsilon) {
                return juce::String::formatted("%d/%d", numerator, denominator);
            }
        }
    }

    return juce::String(value);
}