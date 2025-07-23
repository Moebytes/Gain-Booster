#pragma once
#include <JuceHeader.h>
#include "BinaryData.h"

struct ParameterIDs {
    juce::ParameterID gain;
    juce::ParameterID boost;
    juce::ParameterID pan;

    static auto loadFromJSON() -> ParameterIDs {
        ParameterIDs parameterIDs;

        juce::String jsonStr = juce::String::fromUTF8(BinaryData::parameters_json, BinaryData::parameters_jsonSize);

        juce::var parsed = juce::JSON::parse(jsonStr);
        auto* json = parsed.getDynamicObject();
        jassert(json != nullptr);

        auto getParameter = [json](const juce::String& key) -> juce::ParameterID {
            auto value = json->getProperty(key);
            auto *obj = value.getDynamicObject();
            jassert(obj != nullptr);

            juce::String id = obj->getProperty("id").toString();
            int version = static_cast<int>(obj->getProperty("version"));

            return juce::ParameterID{id, version};
        };

        parameterIDs.gain = getParameter("gain");
        parameterIDs.boost = getParameter("boost");
        parameterIDs.pan = getParameter("pan");

        return parameterIDs;
    }
};