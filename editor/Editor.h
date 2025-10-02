#pragma once
#pragma clang diagnostic ignored "-Wshadow-field"
#include <JuceHeader.h>
#include "Processor.h"
#include "EventEmitter.hpp"

class Editor : public AudioProcessorEditor, public EventEmitter::Listener {
public:
    Editor(Processor& p);
    ~Editor() override;
    
    auto resized() -> void override;

    auto getResource(const String& url) -> std::optional<WebBrowserComponent::Resource>;
    auto webviewOptions() -> WebBrowserComponent::Options;
    auto getWebviewFileBytes(const String& resourceStr) -> std::vector<std::byte>;

    auto handleEvent(const String& name, const var& payload) -> void override;
        
private:
    Processor& processor;
    ComponentBoundsConstrainer constrainer;

    WebSliderRelay gainRelay {Parameters::paramIDs.gain.getParamID()};
    WebSliderParameterAttachment gainAttachment {*this->processor.parameters.gainParam, gainRelay, nullptr};
    WebComboBoxRelay gainCurveRelay {Parameters::paramIDs.gainCurve.getParamID()};
    WebComboBoxParameterAttachment gainCurveAttachment {*this->processor.parameters.gainCurveParam, gainCurveRelay, nullptr};
    
    WebSliderRelay boostRelay {Parameters::paramIDs.boost.getParamID()};
    WebSliderParameterAttachment boostAttachment {*this->processor.parameters.boostParam, boostRelay, nullptr};
    WebComboBoxRelay boostCurveRelay {Parameters::paramIDs.boostCurve.getParamID()};
    WebComboBoxParameterAttachment boostCurveAttachment {*this->processor.parameters.boostCurveParam, boostCurveRelay, nullptr};

    WebSliderRelay panRelay {Parameters::paramIDs.pan.getParamID()};
    WebSliderParameterAttachment panAttachment {*this->processor.parameters.panParam, panRelay, nullptr};
    WebComboBoxRelay panningLawRelay {Parameters::paramIDs.panningLaw.getParamID()};
    WebComboBoxParameterAttachment panningLawAttachment {*this->processor.parameters.panningLawParam, panningLawRelay, nullptr};
    
    WebComboBoxRelay gainLFOTypeRelay {Parameters::paramIDs.gainLFOType.getParamID()};
    WebComboBoxParameterAttachment gainLFOTypeAttachment {*this->processor.parameters.gainLFOTypeParam, gainLFOTypeRelay, nullptr};
    WebSliderRelay gainLFORateRelay {Parameters::paramIDs.gainLFORate.getParamID()};
    WebSliderParameterAttachment gainLFORateAttachment {*this->processor.parameters.gainLFORateParam, gainLFORateRelay, nullptr};
    WebSliderRelay gainLFOAmountRelay {Parameters::paramIDs.gainLFOAmount.getParamID()};
    WebSliderParameterAttachment gainLFOAmountAttachment {*this->processor.parameters.gainLFOAmountParam, gainLFOAmountRelay, nullptr};
    
    WebComboBoxRelay panLFOTypeRelay {Parameters::paramIDs.panLFOType.getParamID()};
    WebComboBoxParameterAttachment panLFOTypeAttachment {*this->processor.parameters.panLFOTypeParam, panLFOTypeRelay, nullptr};
    WebSliderRelay panLFORateRelay {Parameters::paramIDs.panLFORate.getParamID()};
    WebSliderParameterAttachment panLFORateAttachment {*this->processor.parameters.panLFORateParam, panLFORateRelay, nullptr};
    WebSliderRelay panLFOAmountRelay {Parameters::paramIDs.panLFOAmount.getParamID()};
    WebSliderParameterAttachment panLFOAmountAttachment {*this->processor.parameters.panLFOAmountParam, panLFOAmountRelay, nullptr};

    WebBrowserComponent webview;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)
};