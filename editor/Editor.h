#pragma once
#pragma clang diagnostic ignored "-Wshadow-field"
#include <JuceHeader.h>
#include "Processor.h"
#include "EventEmitter.hpp"

class Editor : public juce::AudioProcessorEditor, public EventEmitter::Listener {
public:
    Editor(Processor& p);
    ~Editor() override;
    
    auto resized() -> void override;

    auto getResource(const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource>;
    auto webviewOptions() -> juce::WebBrowserComponent::Options;
    auto getWebviewFileBytes(const juce::String& resourceStr) -> std::vector<std::byte>;

    auto handleEvent(const juce::String& name, const juce::var& payload) -> void override;
        
private:
    Processor& processor;
    juce::ComponentBoundsConstrainer constrainer;

    juce::WebSliderRelay gainRelay {Parameters::paramIDs.gain.getParamID()};
    juce::WebSliderParameterAttachment gainAttachment {*this->processor.parameters.gainParam, gainRelay, nullptr};
    juce::WebComboBoxRelay gainCurveRelay {Parameters::paramIDs.gainCurve.getParamID()};
    juce::WebComboBoxParameterAttachment gainCurveAttachment {*this->processor.parameters.gainCurveParam, gainCurveRelay, nullptr};
    
    juce::WebSliderRelay boostRelay {Parameters::paramIDs.boost.getParamID()};
    juce::WebSliderParameterAttachment boostAttachment {*this->processor.parameters.boostParam, boostRelay, nullptr};
    juce::WebComboBoxRelay boostCurveRelay {Parameters::paramIDs.boostCurve.getParamID()};
    juce::WebComboBoxParameterAttachment boostCurveAttachment {*this->processor.parameters.boostCurveParam, boostCurveRelay, nullptr};

    juce::WebSliderRelay panRelay {Parameters::paramIDs.pan.getParamID()};
    juce::WebSliderParameterAttachment panAttachment {*this->processor.parameters.panParam, panRelay, nullptr};
    juce::WebComboBoxRelay panningLawRelay {Parameters::paramIDs.panningLaw.getParamID()};
    juce::WebComboBoxParameterAttachment panningLawAttachment {*this->processor.parameters.panningLawParam, panningLawRelay, nullptr};
    
    juce::WebComboBoxRelay gainLFOTypeRelay {Parameters::paramIDs.gainLFOType.getParamID()};
    juce::WebComboBoxParameterAttachment gainLFOTypeAttachment {*this->processor.parameters.gainLFOTypeParam, gainLFOTypeRelay, nullptr};
    juce::WebSliderRelay gainLFORateRelay {Parameters::paramIDs.gainLFORate.getParamID()};
    juce::WebSliderParameterAttachment gainLFORateAttachment {*this->processor.parameters.gainLFORateParam, gainLFORateRelay, nullptr};
    juce::WebSliderRelay gainLFOAmountRelay {Parameters::paramIDs.gainLFOAmount.getParamID()};
    juce::WebSliderParameterAttachment gainLFOAmountAttachment {*this->processor.parameters.gainLFOAmountParam, gainLFOAmountRelay, nullptr};
    
    juce::WebComboBoxRelay panLFOTypeRelay {Parameters::paramIDs.panLFOType.getParamID()};
    juce::WebComboBoxParameterAttachment panLFOTypeAttachment {*this->processor.parameters.panLFOTypeParam, panLFOTypeRelay, nullptr};
    juce::WebSliderRelay panLFORateRelay {Parameters::paramIDs.panLFORate.getParamID()};
    juce::WebSliderParameterAttachment panLFORateAttachment {*this->processor.parameters.panLFORateParam, panLFORateRelay, nullptr};
    juce::WebSliderRelay panLFOAmountRelay {Parameters::paramIDs.panLFOAmount.getParamID()};
    juce::WebSliderParameterAttachment panLFOAmountAttachment {*this->processor.parameters.panLFOAmountParam, panLFOAmountRelay, nullptr};

    juce::WebBrowserComponent webview;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)
};