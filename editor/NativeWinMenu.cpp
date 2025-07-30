#ifdef _WIN32
#include <windows.h>
#include <uxtheme.h>
#include <dwmapi.h>
#include <map>
#include <string>
#include <functional>
#include <Shlwapi.h>
#include <winreg.h>

#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "dwmapi.lib")
#pragma comment(lib, "Shlwapi.lib")

typedef enum _PreferredAppMode {
    Default,
    AllowDark,
    ForceDark,
    ForceLight,
    Max
} PreferredAppMode;

using fnSetPreferredAppMode = PreferredAppMode(__stdcall*)(PreferredAppMode);
using fnAllowDarkModeForWindow = bool(__stdcall*)(HWND hwnd, bool allow);

bool enableDarkMode(HWND hwnd) {
    HMODULE hUxTheme = LoadLibraryW(L"uxtheme.dll");
    if (!hUxTheme) return false;

    auto SetPreferredAppMode = reinterpret_cast<fnSetPreferredAppMode>(
        GetProcAddress(hUxTheme, MAKEINTRESOURCEA(135)));
    auto AllowDarkModeForWindow = reinterpret_cast<fnAllowDarkModeForWindow>(
        GetProcAddress(hUxTheme, MAKEINTRESOURCEA(133)));

    if (SetPreferredAppMode) SetPreferredAppMode(AllowDark);
    if (AllowDarkModeForWindow && hwnd) AllowDarkModeForWindow(hwnd, true);

    return true;
}

bool isSystemInDarkMode() {
    DWORD value = 1;
    DWORD size = sizeof(value);
    if (RegGetValueW(HKEY_CURRENT_USER,
        L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
        L"AppsUseLightTheme", RRF_RT_REG_DWORD, nullptr, &value, &size) == ERROR_SUCCESS) {
        return value == 0;
    }
    return false;
}

void showNativeWinMenu(const std::map<int, std::string>& items,
    const std::map<int, std::string>& factoryItems,
    const std::map<int, std::string>& userItems, const std::string& userFolder,
    const std::string& currentPresetName,
    const std::string& presetFolder,
    std::function<void(int)> callback) {
    
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

    HWND hwnd = GetForegroundWindow();

    if (isSystemInDarkMode()) {
        enableDarkMode(hwnd);
    }

    int cmd = TrackPopupMenu(hMenu,
        TPM_RETURNCMD | TPM_TOPALIGN | TPM_LEFTALIGN,
        pt.x, pt.y, 0, hwnd, NULL
    );

    if (cmd != 0 && callback) {
        callback(cmd);
    }

    DestroyMenu(hMenu);
}

#endif