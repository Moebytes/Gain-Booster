#ifdef _WIN32
#include <windows.h>
#include "NativeMenuBridge.h"

void showNativeWinMenu(const std::map<int, std::string>& items,
    const std::map<int, std::string>& factoryItems,
    const std::map<int, std::string>& userItems, const std::string& userFolder,
    const std::string& currentPresetName,
    const std::string& presetFolder,
    std::function<void(int)> callback)
{
    HMENU hMenu = CreatePopupMenu();

    for (const auto& [id, label] : items) {
        AppendMenuA(hMenu, MF_STRING, id, label.c_str());
    }

    if (!factoryItems.empty()) {
        HMENU factorySub = CreatePopupMenu();
        for (const auto& [id, name] : factoryItems) {
            UINT flags = MF_STRING;
            if (presetFolder == "factory" && name == currentPresetName) {
                flags |= MF_CHECKED;
            }
            AppendMenuA(factorySub, flags, id, name.c_str());
        }
        AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)factorySub, "Factory");
    }

    if (!userItems.empty()) {
        HMENU userSub = CreatePopupMenu();
        for (const auto& [id, name] : userItems) {
            UINT flags = MF_STRING;
            if (presetFolder == "user" && name == currentPresetName) {
                flags |= MF_CHECKED;
            }
            AppendMenuA(userSub, flags, id, name.c_str());
        }
        AppendMenuA(hMenu, MF_POPUP, (UINT_PTR)userSub, userFolder.c_str());
    }

    POINT pt;
    GetCursorPos(&pt);

    int cmd = TrackPopupMenu(hMenu,
        TPM_RETURNCMD | TPM_TOPALIGN | TPM_LEFTALIGN,
        pt.x, pt.y, 0, GetForegroundWindow(), NULL
    );

    if (cmd != 0 && callback) {
        callback(cmd);
    }

    DestroyMenu(hMenu);
}
#endif