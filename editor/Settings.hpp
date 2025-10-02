#pragma once
#include <JuceHeader.h>

class Settings {
public:
    static auto getSettingsFile() -> File {
        return File::getSpecialLocation(File::userApplicationDataDirectory)
            .getChildFile(JucePlugin_Manufacturer)
            .getChildFile(JucePlugin_Name)
            .getChildFile("settings.json");
    }

    static auto setSettingKey(const String& key, const var& value) -> void {
        auto file = getSettingsFile();
        var json;
    
        if (file.existsAsFile()) json = JSON::parse(file);
        if (!json.isObject()) json = var{new DynamicObject()};
    
        if (auto* obj = json.getDynamicObject()) {
            obj->setProperty(key, value);
            file.getParentDirectory().createDirectory();
            file.replaceWithText(JSON::toString(json, true)); 
        }
    }

    static auto getSettingKey(const String& key, const var& defaultValue) -> var {
        auto file = getSettingsFile();
    
        if (!file.existsAsFile()) return defaultValue;
    
        auto json = JSON::parse(file);
        if (!json.isObject()) return defaultValue;
        
        if (auto* obj = json.getDynamicObject()) {
            if (obj->hasProperty(key)) return obj->getProperty(key);
        }
    
        return defaultValue;
    }
};