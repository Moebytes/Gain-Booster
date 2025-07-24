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
    auto visibilityChanged() -> void override;

    auto getDefaultParameter(const juce::Array<juce::var>& args, 
        juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void;

private:
    Processor& processorRef;

    juce::WebSliderRelay gainRelay {Parameters::paramIDs.gain.getParamID()};
    juce::WebSliderParameterAttachment gainAttachment {*processorRef.params.gainParam, gainRelay, nullptr};
    
    juce::WebSliderRelay boostRelay {Parameters::paramIDs.boost.getParamID()};
    juce::WebSliderParameterAttachment boostAttachment {*processorRef.params.boostParam, boostRelay, nullptr};

    juce::WebSliderRelay panRelay {Parameters::paramIDs.pan.getParamID()};
    juce::WebSliderParameterAttachment panAttachment {*processorRef.params.panParam, panRelay, nullptr};

    juce::WebBrowserComponent webview;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Editor)
};
