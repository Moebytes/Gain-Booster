#pragma once
#include <functional>
#include <map>

#ifdef __APPLE__
void showNativeMacMenu(const std::map<int, std::string>& items, std::function<void(int)> callback);
#endif