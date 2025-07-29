#include "Processor.h"
#include "Editor.h"
#include "Functions.h"
#include "BinaryData.h"
#include "NativeMenuBridge.h"

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
        return this->getDefaultParameter(args, completion); 
    })
    .withNativeFunction("openPresetMenu", [this](auto args, auto completion){ 
        return this->openPresetMenu(args, completion); 
    });
}

auto Editor::getDefaultParameter(const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {

    juce::String paramID = args[0].toString();
    auto param = processorRef.tree.getParameter(paramID);
    float defaultValue = param->convertFrom0to1(param->getDefaultValue());

    completion(defaultValue);
}


auto Editor::openPresetMenu([[maybe_unused]] const juce::Array<juce::var>& args, 
    [[maybe_unused]] juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {

        static const std::map<int, std::string> items = {
            {1, "Init Preset"},
            {2, "Load Preset"},
            {3, "Save Preset"}
        };

        auto menuClick = [this](std::string action){
            if (action == "Init Preset") {
                this->processorRef.initPreset();
                std::println("init preset called");
            } else if (action == "Load Preset") {
                this->loadPresetFromFile();
            } else if (action == "Save Preset") {
                this->savePresetToFile();
            }
        };

        #if JUCE_MAC
            showNativeMacMenu(items, [menuClick](int resultID) {
                auto it = items.find(resultID);
                if (it != items.end()) {
                    auto action = it->second;
                    menuClick(action);
                }
            });
        #else
            juce::PopupMenu menu;
            for (const auto& [id, label] : items) {
                menu.addItem(id, label);
            }

            auto mousePos = juce::Desktop::getMousePosition();
            
            auto options = juce::PopupMenu::Options()
                .withPreferredPopupDirection(juce::PopupMenu::Options::PopupDirection::upwards)
                .withTargetScreenArea(juce::Rectangle<int> {mousePos.x - 60, mousePos.y - 5, 1, 1});

            juce::PopupMenu::dismissAllActiveMenus();
            menu.showMenuAsync(options, [menuClick](int resultID) {
                auto it = items.find(resultID);
                if (it != items.end()) {
                    auto action = it->second;
                    menuClick(action);
                }
            });
        #endif
}

auto Editor::resized() -> void {
    webview.setBounds(getLocalBounds());
    Editor::setSettingKey("windowWidth", getWidth());
    Editor::setSettingKey("windowHeight", getHeight());
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

auto Editor::loadPresetFromFile() -> void {
    auto directoryPath = Editor::getSettingKey("loadDirectory", Functions::getDownloadsFolder().getFullPathName());
    juce::File directory{directoryPath};

    auto loadDialog = std::make_shared<juce::FileChooser>(
        "Load Preset", directory, "*.json"
    );

    loadDialog->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, loadDialog](const juce::FileChooser& picker) {
            auto file = picker.getResult();
            if (file.existsAsFile()) {
                Editor::setSettingKey("loadDirectory", file.getParentDirectory().getFullPathName());
                juce::String jsonString = file.loadFileAsString();
                processorRef.loadPreset(jsonString);
            }
        }
    );
}

auto Editor::savePresetToFile() -> void {
    auto* infoDialog = new juce::AlertWindow("Save Preset", "Preset Information:", juce::AlertWindow::NoIcon);
    infoDialog->addTextEditor("name", "", "Name:");
    infoDialog->addTextEditor("author", "", "Author:");
    infoDialog->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
    infoDialog->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    auto saveCallback = [this](juce::String name, juce::String author) {
        auto cleanName = Functions::cleanFilename(name);

        auto directoryPath = Editor::getSettingKey("saveDirectory", Functions::getDownloadsFolder().getFullPathName());
        juce::File directory{directoryPath};

        auto saveDialog = std::make_shared<juce::FileChooser>(
            "Save Preset As", directory.getChildFile(cleanName + ".json")
        );

        saveDialog->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
            [this, saveDialog, name, author](const juce::FileChooser& picker) {
                auto file = picker.getResult();
                if (file != juce::File{}) {
                    Editor::setSettingKey("saveDirectory", file.getParentDirectory().getFullPathName());
                    auto jsonString = processorRef.savePreset(name, author);
                    file.replaceWithText(jsonString);
                    file.revealToUser();
                }
            }
        );
    };

    auto nameCallback = juce::ModalCallbackFunction::create([infoDialog, saveCallback](int result) {
        if (result == 1) {
            auto name = infoDialog->getTextEditor("name")->getText();
            auto author = infoDialog->getTextEditor("author")->getText();

            saveCallback(name, author);
        }
        delete infoDialog;
    });

    infoDialog->enterModalState(true, nameCallback, true);
}