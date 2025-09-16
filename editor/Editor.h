#pragma once
#include <JuceHeader.h>
#include "Processor.h"

class Editor : public juce::AudioProcessorEditor {
public:
    Editor(Processor& p);
    ~Editor() override;
    
    auto resized() -> void override;

    auto getResource(const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource>;
    auto webviewOptions() -> juce::WebBrowserComponent::Options;
    auto getWebviewFileBytes(const juce::String& resourceStr) -> std::vector<std::byte>;

    inline auto getWebview() -> juce::WebBrowserComponent& { return this->webview; }
        
private:
    Processor& processorRef;
    juce::ComponentBoundsConstrainer constrainer;

    juce::WebSliderRelay gainRelay {Parameters::paramIDs.gain.getParamID()};
    juce::WebSliderParameterAttachment gainAttachment {*processorRef.parameters.gainParam, gainRelay, nullptr};

    juce::WebComboBoxRelay gainCurveRelay {Parameters::paramIDs.gainCurve.getParamID()};
    juce::WebComboBoxParameterAttachment gainCurveAttachment {*processorRef.parameters.gainCurveParam, gainCurveRelay, nullptr};
    
    juce::WebSliderRelay boostRelay {Parameters::paramIDs.boost.getParamID()};
    juce::WebSliderParameterAttachment boostAttachment {*processorRef.parameters.boostParam, boostRelay, nullptr};

    juce::WebComboBoxRelay boostCurveRelay {Parameters::paramIDs.boostCurve.getParamID()};
    juce::WebComboBoxParameterAttachment boostCurveAttachment {*processorRef.parameters.boostCurveParam, boostCurveRelay, nullptr};

    juce::WebSliderRelay panRelay {Parameters::paramIDs.pan.getParamID()};
    juce::WebSliderParameterAttachment panAttachment {*processorRef.parameters.panParam, panRelay, nullptr};

    juce::WebComboBoxRelay panningLawRelay {Parameters::paramIDs.panningLaw.getParamID()};
    juce::WebComboBoxParameterAttachment panningLawAttachment {*processorRef.parameters.panningLawParam, panningLawRelay, nullptr};
    
    juce::WebComboBoxRelay gainLFOTypeRelay {Parameters::paramIDs.gainLFOType.getParamID()};
    juce::WebComboBoxParameterAttachment gainLFOTypeAttachment {*processorRef.parameters.gainLFOTypeParam, gainLFOTypeRelay, nullptr};

    juce::WebSliderRelay gainLFORateRelay {Parameters::paramIDs.gainLFORate.getParamID()};
    juce::WebSliderParameterAttachment gainLFORateAttachment {*processorRef.parameters.gainLFORateParam, gainLFORateRelay, nullptr};

    juce::WebSliderRelay gainLFOAmountRelay {Parameters::paramIDs.gainLFOAmount.getParamID()};
    juce::WebSliderParameterAttachment gainLFOAmountAttachment {*processorRef.parameters.gainLFOAmountParam, gainLFOAmountRelay, nullptr};
    
    juce::WebComboBoxRelay panLFOTypeRelay {Parameters::paramIDs.panLFOType.getParamID()};
    juce::WebComboBoxParameterAttachment panLFOTypeAttachment {*processorRef.parameters.panLFOTypeParam, panLFOTypeRelay, nullptr};

    juce::WebSliderRelay panLFORateRelay {Parameters::paramIDs.panLFORate.getParamID()};
    juce::WebSliderParameterAttachment panLFORateAttachment {*processorRef.parameters.panLFORateParam, panLFORateRelay, nullptr};

    juce::WebSliderRelay panLFOAmountRelay {Parameters::paramIDs.panLFOAmount.getParamID()};
    juce::WebSliderParameterAttachment panLFOAmountAttachment {*processorRef.parameters.panLFOAmountParam, panLFOAmountRelay, nullptr};

    juce::WebBrowserComponent webview;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)
};
