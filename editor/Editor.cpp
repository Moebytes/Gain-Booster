#include "Processor.h"
#include "Editor.h"
#include "Functions.h"
#include "BinaryData.h"

Editor::Editor(Processor& p) : AudioProcessorEditor(&p), processorRef(p),
    webview(webviewOptions()) {

    webview.goToURL(webview.getResourceProviderRoot());

    const int width = static_cast<int>(Editor::getSettingKey("windowWidth", 510));
    const int height = static_cast<int>(Editor::getSettingKey("windowHeight", 580));
    const float aspectRatio = static_cast<float>(width) / height;

    const int minWidth = 240;
    const int minHeight = static_cast<int>(minWidth / aspectRatio); 

    constrainer.setFixedAspectRatio(aspectRatio);
    constrainer.setMinimumSize(minWidth, minHeight);
    constrainer.setMaximumSize(10000, 10000);
    
    setConstrainer(&constrainer);
    setResizable(true, true);
    setSize(width, height);

    addAndMakeVisible(webview);
}

Editor::~Editor() {
}

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
        return getDefaultParameter(args, completion); 
    });
}

auto Editor::getDefaultParameter(const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {

    juce::String paramID = args[0].toString();
    auto param = processorRef.tree.getParameter(paramID);
    float defaultValue = param->convertFrom0to1(param->getDefaultValue());

    completion(defaultValue);
}

auto Editor::resized() -> void {
    webview.setBounds(getLocalBounds());
    setSettingKey("windowWidth", getWidth());
    setSettingKey("windowHeight", getHeight());
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

auto Editor::getSettingsFile() -> juce::File {
    return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory)
        .getChildFile(JucePlugin_Manufacturer)
        .getChildFile(JucePlugin_Name)
        .getChildFile("settings.json");
}

auto Editor::setSettingKey(const juce::String& key, const juce::var& value) -> void {
    auto file = getSettingsFile();
    juce::var json;

    if (file.existsAsFile()) json = juce::JSON::parse(file);
    if (!json.isObject()) json = juce::var{new juce::DynamicObject()};

    auto* obj = json.getDynamicObject();
    if (obj != nullptr) {
        obj->setProperty(key, value);
        file.getParentDirectory().createDirectory();
        file.replaceWithText(juce::JSON::toString(json, true)); 
    }
}

auto Editor::getSettingKey(const juce::String& key, const juce::var& defaultValue = juce::var{}) -> juce::var {
    auto file = getSettingsFile();

    if (!file.existsAsFile()) return defaultValue;

    juce::var json = juce::JSON::parse(file);
    if (!json.isObject()) return defaultValue;

    
    auto* obj = json.getDynamicObject();
    if (obj != nullptr) {
        if (obj->hasProperty(key)) return obj->getProperty(key);
    }

    return defaultValue;
}