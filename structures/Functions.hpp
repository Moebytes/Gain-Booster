#pragma once
#include <JuceHeader.h>

#if JUCE_WINDOWS
  #include <windows.h>
  #include <shlobj.h>
  #pragma comment(lib, "Shell32.lib")
#endif

class Functions {
public:
    static auto streamToVector(juce::InputStream& stream) -> std::vector<std::byte> {
        std::vector<std::byte> result(static_cast<size_t>(stream.getTotalLength()));
        stream.setPosition(0);
        [[maybe_unused]] auto bytesRead = stream.read (result.data(), result.size());
        jassert (bytesRead == static_cast<ssize_t>(result.size()));
        return result;
    }

    static auto getMimeForExtension(const String& extension) -> const char* {
        static std::unordered_map<String, const char*> mimeMap = {
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
    
        auto it = mimeMap.find(extension.toLowerCase());
        if (it != mimeMap.end()) {
            return it->second;
        }
    
        jassertfalse;
        return "";
    }

    static auto checkAudioSafety(juce::AudioBuffer<float>& buffer) -> void {
        for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
            float* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
                float value = channelData[sample];
                if (std::isnan(value)) {
                    juce::Logger::outputDebugString("NaN detected");
                    return buffer.clear();
                } else if (std::isinf(value)) {
                    juce::Logger::outputDebugString("Inf detected");
                    return buffer.clear();
                } else if (value < -2.0f || value > 2.0f) {
                    juce::Logger::outputDebugString("Sample out of range");
                    return buffer.clear();
                }
            }
        }
    }

    static auto displayPercent(float value, int) -> juce::String {
        return juce::String::formatted("%.0f%%", value * 100.0f);
    }

    static auto displayDecibels(float value, int) -> juce::String {
        return juce::String::formatted("%.1f dB", value);
    }

    static auto displayPan(float value, int) -> juce::String {
        int position = static_cast<int>(value * 50.0f);
        if (position < 0) {
            return juce::String::formatted("%dL", -position);
        } else {
            return juce::String::formatted("%dR", position);
        }
    }

    static auto displayLFORate(float value, int) -> juce::String {
        float epsilon = 0.0001f;
    
        for (int numerator = 1; numerator <= 4; numerator++) {
            for (int denominator = 1; denominator <= 32; denominator++) {
                float candidate = static_cast<float>(numerator) / static_cast<float>(denominator);
                if (std::abs(value - candidate) < epsilon) {
                    return juce::String::formatted("%d/%d", numerator, denominator);
                }
            }
        }
    
        return juce::String(value);
    }

    static auto getDownloadsFolder() -> juce::File {
        #if JUCE_WINDOWS
            PWSTR path = nullptr;
            if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &path))) {
                int utf8Size = WideCharToMultiByte(CP_UTF8, 0, path, -1, nullptr, 0, nullptr, nullptr);
                if (utf8Size > 0) {
                    auto utf8Path = std::unique_ptr<char[]>(new char[utf8Size]);
                    WideCharToMultiByte(CP_UTF8, 0, path, -1, utf8Path.get(), utf8Size, nullptr, nullptr);
                    juce::String downloadsPath = juce::String::fromUTF8(utf8Path.get());
                    CoTaskMemFree(path);
                    return juce::File(downloadsPath);
                }
                CoTaskMemFree(path);
            }
            return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
        #else
            auto downloadsPath = juce::File::getSpecialLocation(juce::File::userHomeDirectory).getChildFile("Downloads");
            if (downloadsPath.exists() && downloadsPath.isDirectory()) {
                return downloadsPath;
            }
            return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
        #endif
    }

    static auto cleanFilename(const juce::String& input) -> juce::String {
        static juce::String illegalChars = "\"*/\\:<>?|";
    
        juce::String result;
        for (auto c : input) {
            if (illegalChars.containsChar(c)) {
                continue;
            } else {
                result += c;
            }
        }
        result = result.trim();
    
        if (result.isEmpty()) {
            return "Untitled";
        }
        
        return result;
    }    
};