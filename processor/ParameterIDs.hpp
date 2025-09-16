#pragma once
#include <JuceHeader.h>
#include "BinaryData.h"

class ParameterIDs {
public:
    juce::ParameterID gain;
    juce::ParameterID gainCurve;
    juce::ParameterID boost;
    juce::ParameterID boostCurve;
    juce::ParameterID pan;
    juce::ParameterID panningLaw;
    juce::ParameterID gainLFOType;
    juce::ParameterID gainLFORate;
    juce::ParameterID gainLFOAmount;
    juce::ParameterID panLFOType;
    juce::ParameterID panLFORate;
    juce::ParameterID panLFOAmount;

    static inline juce::var parsedJSON;

    static auto getJSON() -> juce::DynamicObject* {
        if (!ParameterIDs::parsedJSON) {
            auto jsonStr = juce::String::fromUTF8(BinaryData::parameters_json, BinaryData::parameters_jsonSize);
            ParameterIDs::parsedJSON = juce::JSON::parse(jsonStr);
        }
        return ParameterIDs::parsedJSON.getDynamicObject();
    }

    static auto loadFromJSON() -> ParameterIDs {
        ParameterIDs parameterIDs;

        auto* json = ParameterIDs::getJSON();

        auto getParameter = [&json](const juce::String& key) -> juce::ParameterID {
            auto value = json->getProperty(key);
            auto* obj = value.getDynamicObject();
            jassert(obj != nullptr);

            juce::String id = obj->getProperty("id").toString();
            int version = static_cast<int>(obj->getProperty("version"));

            return juce::ParameterID{id, version};
        };

        parameterIDs.gain = getParameter("gain");
        parameterIDs.gainCurve = getParameter("gainCurve");
        parameterIDs.boost = getParameter("boost");
        parameterIDs.boostCurve = getParameter("boostCurve");
        parameterIDs.pan = getParameter("pan");
        parameterIDs.panningLaw = getParameter("panningLaw");

        parameterIDs.gainLFOType = getParameter("gainLFOType");
        parameterIDs.gainLFORate = getParameter("gainLFORate");
        parameterIDs.gainLFOAmount = getParameter("gainLFOAmount");
        parameterIDs.panLFOType = getParameter("panLFOType");
        parameterIDs.panLFORate = getParameter("panLFORate");
        parameterIDs.panLFOAmount = getParameter("panLFOAmount");

        return parameterIDs;
    }

    static auto getStringKeys() -> std::vector<juce::String> {
        auto* json = ParameterIDs::getJSON();
        std::vector<juce::String> keys;
        keys.reserve(static_cast<size_t>(json->getProperties().size()));

        for (const auto& prop : json->getProperties()) {
            keys.push_back(prop.name.toString());
        }

        return keys;
    }

    static auto isStringValue(const juce::String& key) -> bool {
        auto* json = ParameterIDs::getJSON();
        if (json->hasProperty(key)) {
            auto* paramDef = json->getProperty(key).getDynamicObject();
            if (paramDef->hasProperty("stringValue") && paramDef->getProperty("stringValue")) {
                return true;
            }
        }

        return false;
    }
};