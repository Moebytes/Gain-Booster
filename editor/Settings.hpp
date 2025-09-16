#pragma once
#include <JuceHeader.h>

class Settings {
public:
    static auto getSettingsFile() -> juce::File {
        return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
            .getChildFile(JucePlugin_Manufacturer)
            .getChildFile(JucePlugin_Name)
            .getChildFile("settings.json");
    }

    static auto setSettingKey(const juce::String& key, const juce::var& value) -> void {
        auto file = getSettingsFile();
        juce::var json;
    
        if (file.existsAsFile()) json = juce::JSON::parse(file);
        if (!json.isObject()) json = juce::var{new juce::DynamicObject()};
    
        if (auto* obj = json.getDynamicObject()) {
            obj->setProperty(key, value);
            file.getParentDirectory().createDirectory();
            file.replaceWithText(juce::JSON::toString(json, true)); 
        }
    }

    static auto getSettingKey(const juce::String& key, const juce::var& defaultValue) -> juce::var {
        auto file = getSettingsFile();
    
        if (!file.existsAsFile()) return defaultValue;
    
        auto json = juce::JSON::parse(file);
        if (!json.isObject()) return defaultValue;
        
        if (auto* obj = json.getDynamicObject()) {
            if (obj->hasProperty(key)) return obj->getProperty(key);
        }
    
        return defaultValue;
    }
};