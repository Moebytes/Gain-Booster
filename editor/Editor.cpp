#include "Processor.h"
#include "Editor.h"
#include "Functions.h"
#include "BinaryData.h"

Editor::Editor(Processor& p) : AudioProcessorEditor(&p), processorRef(p),
    webview(webviewOptions()) {
    webview.goToURL(webview.getResourceProviderRoot());

    setResizable(true, true);
    addAndMakeVisible(webview);

    setSize(510, 550);
}

Editor::~Editor() {
}

auto Editor::visibilityChanged() -> void {
    if (isVisible()) webview.goToURL(webview.getResourceProviderRoot());
}

auto Editor::webviewOptions() -> juce::WebBrowserComponent::Options {
    return juce::WebBrowserComponent::Options{}
    .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
    .withWinWebView2Options(juce::WebBrowserComponent::Options::WinWebView2{}
    .withUserDataFolder(juce::File::getSpecialLocation(juce::File::tempDirectory))
    .withBackgroundColour(juce::Colours::white))
    .withResourceProvider([this](const auto& url) { return getResource(url); })
    .withNativeIntegrationEnabled()
    .withOptionsFrom(gainRelay)
    .withOptionsFrom(boostRelay)
    .withOptionsFrom(panRelay)
    .withOptionsFrom(gainSkewRelay)
    .withOptionsFrom(boostSkewRelay)
    .withOptionsFrom(panningLawRelay)
    .withNativeFunction("getDefaultParameter", [this](auto args, auto completion){ 
        return getDefaultParameter(args, completion); 
    });
}

auto Editor::getDefaultParameter(const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {

    juce::String paramID = args[0].toString();
    float defaultValue = processorRef.tree.getParameter(paramID)->getDefaultValue();

    completion(defaultValue);
}

auto Editor::resized() -> void {
    webview.setBounds(getLocalBounds());
}

auto Editor::getWebviewFileBytes(const juce::String& resourceStr) -> std::vector<std::byte> {
    juce::MemoryInputStream zipStream(BinaryData::webview_files_zip, BinaryData::webview_files_zipSize, false);
    juce::ZipFile zip{zipStream};

    if (auto* entry = zip.getEntry(resourceStr)) {
        const std::unique_ptr<juce::InputStream> entryStream{zip.createStreamForEntry(*entry)};
        if (entryStream == nullptr) {
            jassertfalse;
            return {};
        }
        return Functions::streamToVector(*entryStream);
    }
    return {};
}

auto Editor::getResource(const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource> {
    static const auto fileRoot = juce::File::getCurrentWorkingDirectory().getChildFile("dist");
    const auto resourceStr = url == "/" ? "index.html" : url.fromFirstOccurrenceOf("/", false, false);
    const auto ext = resourceStr.fromLastOccurrenceOf(".", false, false);

    #if WEBVIEW_DEV_MODE
        const auto stream = fileRoot.getChildFile(resourceStr).createInputStream();
        if (stream) {
            return juce::WebBrowserComponent::Resource(Functions::streamToVector(*stream), Functions::getMimeForExtension(ext));
        }
    #else
        const auto resource = Editor::getWebviewFileBytes(resourceStr);
        if (!resource.empty()) {
            return juce::WebBrowserComponent::Resource(std::move(resource), Functions::getMimeForExtension(ext));
        }
    #endif
    return std::nullopt;
}