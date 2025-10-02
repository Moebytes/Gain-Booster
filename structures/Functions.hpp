#pragma once
#include <JuceHeader.h>

#if JUCE_WINDOWS
  #include <windows.h>
  #include <shlobj.h>
  #pragma comment(lib, "Shell32.lib")
#endif

class Functions {
public:
    static auto streamToVector(InputStream& stream) -> std::vector<std::byte> {
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

    static auto checkAudioSafety(AudioBuffer<float>& buffer) -> void {
        for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
            float* channelData = buffer.getWritePointer(channel);
            for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
                float value = channelData[sample];
                if (std::isnan(value)) {
                    Logger::outputDebugString("NaN detected");
                    return buffer.clear();
                } else if (std::isinf(value)) {
                    Logger::outputDebugString("Inf detected");
                    return buffer.clear();
                } else if (value < -2.0f || value > 2.0f) {
                    Logger::outputDebugString("Sample out of range");
                    return buffer.clear();
                }
            }
        }
    }

    static auto displayPercent(float value, int) -> String {
        return String::formatted("%.0f%%", value * 100.0f);
    }

    static auto parsePercent(const String& text) -> float {
        auto clean = text.trim();
        if (clean.endsWithChar('%')) {
            clean = clean.dropLastCharacters(1).trim();
        }
        return clean.getFloatValue() / 100.0f;
    }

    static auto displayDecibels(float value, int) -> String {
        return String::formatted("%.1fdB", value);
    }

    static auto parseDecibels(const String& text) -> float {
        auto clean = text.trim();
        if (clean.endsWithIgnoreCase("dB")) {
            clean = clean.dropLastCharacters(2).trim();
        }
        return clean.getFloatValue();
    }

    static auto displayPan(float value, int) -> String {
        int position = static_cast<int>(value * 50.0f);
        if (position < 0) {
            return String::formatted("%dL", -position);
        } else {
            return String::formatted("%dR", position);
        }
    }

    static auto parsePan(const String& text) -> float {
        auto clean = text.trim().toUpperCase();
        if (clean.endsWithChar('L')) {
            clean = clean.dropLastCharacters(1);
            float percent = clean.getFloatValue();
            return jlimit(-1.0f, 0.0f, -percent / 50.0f);
        }
        if (clean.endsWithChar('R')) {
            clean = clean.dropLastCharacters(1);
            float percent = clean.getFloatValue();
            return jlimit(0.0f, 1.0f, percent / 50.0f);
        }
        return clean.getFloatValue();
    }

    static auto displayLFORate(float value, int) -> String {
        float epsilon = 0.0001f;
    
        for (int numerator = 1; numerator <= 4; numerator++) {
            for (int denominator = 1; denominator <= 32; denominator++) {
                float candidate = static_cast<float>(numerator) / static_cast<float>(denominator);
                if (std::abs(value - candidate) < epsilon) {
                    return String::formatted("%d/%d", numerator, denominator);
                }
            }
        }
    
        return String(value);
    }

    static auto parseLFORate(const String& text) -> float {
        if (text.containsChar('/')) {
            auto parts = StringArray::fromTokens(text, "/", "");
            if (parts.size() == 2) {
                auto numerator = parts[0].getFloatValue();
                auto denominator = parts[1].getFloatValue();
                if (denominator != 0.0f) {
                    return numerator / denominator;
                }
            }
        }
        return text.getFloatValue();
    }

    static auto getDownloadsFolder() -> File {
        #if JUCE_WINDOWS
            PWSTR path = nullptr;
            if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Downloads, 0, nullptr, &path))) {
                int utf8Size = WideCharToMultiByte(CP_UTF8, 0, path, -1, nullptr, 0, nullptr, nullptr);
                if (utf8Size > 0) {
                    auto utf8Path = std::unique_ptr<char[]>(new char[utf8Size]);
                    WideCharToMultiByte(CP_UTF8, 0, path, -1, utf8Path.get(), utf8Size, nullptr, nullptr);
                    String downloadsPath = String::fromUTF8(utf8Path.get());
                    CoTaskMemFree(path);
                    return File(downloadsPath);
                }
                CoTaskMemFree(path);
            }
            return File::getSpecialLocation(File::userDocumentsDirectory);
        #else
            auto downloadsPath = File::getSpecialLocation(File::userHomeDirectory).getChildFile("Downloads");
            if (downloadsPath.exists() && downloadsPath.isDirectory()) {
                return downloadsPath;
            }
            return File::getSpecialLocation(File::userDocumentsDirectory);
        #endif
    }

    static auto cleanFilename(const String& input) -> String {
        static String illegalChars = "\"*/\\:<>?|";
    
        String result;
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
    
    static auto replaceChar(const String& input, juce_wchar oldChar, juce_wchar newChar) -> String {
        String result;
    
        for (int i = 0; i < input.length(); i++) {
            result += (input[i] == oldChar) ? newChar : input[i];
        }
    
        return result;
    }

    static auto roundFloat(float value, int decimals = 6) {
        float scale = std::pow(10.0f, static_cast<float>(decimals));
        float rounded = std::round(value * scale) / scale;
        return (rounded == 0.0f ? 0.0f : rounded);
    }
};