#include "Processor.h"
#include "Editor.h"
#include "Functions.hpp"
#include "Settings.hpp"
#include "BinaryData.h"

Editor::Editor(Processor& p) : AudioProcessorEditor(&p), processorRef(p),
    webview(webviewOptions()) {

    webview.goToURL(webview.getResourceProviderRoot());

    int width = static_cast<int>(Settings::getSettingKey("windowWidth", 510));
    int height = static_cast<int>(Settings::getSettingKey("windowHeight", 580));
    float aspectRatio = static_cast<float>(width) / height;

    int minWidth = 240;
    int minHeight = static_cast<int>(minWidth / aspectRatio); 

    constrainer.setFixedAspectRatio(aspectRatio);
    constrainer.setMinimumSize(minWidth, minHeight);
    constrainer.setMaximumSize(10000, 10000);
    
    this->setConstrainer(&constrainer);
    this->setResizable(true, true);
    this->setSize(width, height);

    this->addAndMakeVisible(webview);
}

Editor::~Editor() {}

auto Editor::webviewOptions() -> juce::WebBrowserComponent::Options {
    return juce::WebBrowserComponent::Options{}
    .withBackend(juce::WebBrowserComponent::Options::Backend::webview2)
    .withWinWebView2Options(juce::WebBrowserComponent::Options::WinWebView2{}
    .withUserDataFolder(juce::File::getSpecialLocation(juce::File::tempDirectory)))
    .withResourceProvider([this](const auto& url) { return getResource(url); })
    .withNativeIntegrationEnabled()
    .withKeepPageLoadedWhenBrowserIsHidden()
    .withOptionsFrom(gainRelay)
    .withOptionsFrom(boostRelay)
    .withOptionsFrom(panRelay)
    .withOptionsFrom(gainCurveRelay)
    .withOptionsFrom(boostCurveRelay)
    .withOptionsFrom(panningLawRelay)
    .withOptionsFrom(gainLFOTypeRelay)
    .withOptionsFrom(gainLFORateRelay)
    .withOptionsFrom(gainLFOAmountRelay)
    .withOptionsFrom(panLFOTypeRelay)
    .withOptionsFrom(panLFORateRelay)
    .withOptionsFrom(panLFOAmountRelay)
    .withNativeFunction("getDefaultParameter", [this](auto args, auto completion){ 
        return this->processorRef.parameters.getDefaultParameter(args, completion); 
    })
    .withNativeFunction("openPresetMenu", [this](auto args, auto completion){ 
        return this->processorRef.presetManager.openPresetMenu(args, completion); 
    })
    .withNativeFunction("prevPreset", [this](auto args, auto completion){ 
        return this->processorRef.presetManager.prevPreset(args, completion); 
    })
    .withNativeFunction("nextPreset", [this](auto args, auto completion){ 
        return this->processorRef.presetManager.nextPreset(args, completion); 
    });
}

auto Editor::resized() -> void {
    webview.setBounds(getLocalBounds());
    Settings::setSettingKey("windowWidth", getWidth());
    Settings::setSettingKey("windowHeight", getHeight());
}

auto Editor::getWebviewFileBytes(const juce::String& resourceStr) -> std::vector<std::byte> {
    juce::MemoryInputStream zipStream(BinaryData::webview_files_zip, BinaryData::webview_files_zipSize, false);
    juce::ZipFile zip{zipStream};

    if (auto* entry = zip.getEntry(resourceStr)) {
        std::unique_ptr<juce::InputStream> entryStream{zip.createStreamForEntry(*entry)};
        if (entryStream == nullptr) {
            jassertfalse;
            return {};
        }
        return Functions::streamToVector(*entryStream);
    }
    return {};
}

auto Editor::getResource(const juce::String& url) -> std::optional<juce::WebBrowserComponent::Resource> {
    static auto fileRoot = juce::File::getCurrentWorkingDirectory().getChildFile("dist");
    auto resourceStr = url == "/" ? "index.html" : url.fromFirstOccurrenceOf("/", false, false);
    auto ext = resourceStr.fromLastOccurrenceOf(".", false, false);

    #if WEBVIEW_DEV_MODE
        auto stream = fileRoot.getChildFile(resourceStr).createInputStream();
        if (stream) {
            return juce::WebBrowserComponent::Resource(Functions::streamToVector(*stream), Functions::getMimeForExtension(ext));
        }
    #else
        auto resource = Editor::getWebviewFileBytes(resourceStr);
        if (!resource.empty()) {
            return juce::WebBrowserComponent::Resource(std::move(resource), Functions::getMimeForExtension(ext));
        }
    #endif
    return std::nullopt;
}