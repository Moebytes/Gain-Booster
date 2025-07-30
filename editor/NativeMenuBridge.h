#pragma once
#include <functional>
#include <map>

#ifdef __APPLE__
void showNativeMacMenu(const std::map<int, std::string>& items, 
    const std::map<int, std::string>& factoryItems,
    const std::map<int, std::string>& userItems, const std::string& userFolder,
    const std::string& currentPresetName,
    const std::string& presetFolder,
    std::function<void(int)> callback);
#endif

#ifdef _WIN32
void showNativeWinMenu(const std::map<int, std::string>& items,
    const std::map<int, std::string>& factoryItems,
    const std::map<int, std::string>& userItems, const std::string& userFolder,
    const std::string& currentPresetName,
    const std::string& presetFolder,
    std::function<void(int)> callback);
#endif