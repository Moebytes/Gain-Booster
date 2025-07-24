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