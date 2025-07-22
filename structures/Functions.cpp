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
        {"htm",  "text/html"               },
        {"html", "text/html"               },
        {"txt",  "text/plain"              },
        {"jpg",  "image/jpeg"              },
        {"jpeg", "image/jpeg"              },
        {"svg",  "image/svg+xml"           },
        {"ico",  "image/vnd.microsoft.icon"},
        {"json", "application/json"        },
        {"png",  "image/png"               },
        {"css",  "text/css"                },
        {"map",  "application/json"        },
        {"js",   "text/javascript"         },
        {"woff2","font/woff2"              }
    };

    if (const auto it = mimeMap.find(extension.toLowerCase()); it != mimeMap.end()) {
        return it->second;
    }

    jassertfalse;
    return "";
}