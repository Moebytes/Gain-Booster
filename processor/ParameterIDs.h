#pragma once
#include <JuceHeader.h>
#include "BinaryData.h"

struct ParameterIDs {
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
};