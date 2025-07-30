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

    this->loadFactoryPresets();
    this->loadUserPresets();
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
    })
    .withNativeFunction("prevPreset", [this](auto args, auto completion){ 
        return this->prevPreset(args, completion); 
    })
    .withNativeFunction("nextPreset", [this](auto args, auto completion){ 
        return this->nextPreset(args, completion); 
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
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {

        std::map<int, std::string> items = {
            {1, "Init Preset"},
            {2, "Load Preset"},
            {3, "Save Preset"},
            {4, "Add User Folder"}
        };

        juce::String rawUserFolder = Editor::getSettingKey("userFolder", "").toString();
        std::string userFolder = "";
        if (!rawUserFolder.isEmpty()) {
            userFolder = juce::File{rawUserFolder}.getFileName().toStdString();
            items.insert(std::make_pair(5, "Remove User Folder"));
        }

        int factoryID = static_cast<int>(items.size()) + 1;
        std::map<int, std::string> factoryItems;
        for (int i = 0; i < static_cast<int>(factoryPresetNames.size()); i++) {
            factoryItems[factoryID + i] = this->factoryPresetNames[static_cast<size_t>(i)].toStdString();
        }

        int userContentID = factoryID + static_cast<int>(factoryItems.size()) + 1;
        std::map<int, std::string> userItems;
        for (int i = 0; i < static_cast<int>(userPresetNames.size()); i++) {
            userItems[userContentID + i] = this->userPresetNames[static_cast<size_t>(i)].toStdString();
        }

        auto menuClick = [this, completion](std::string action){
            if (action == "Init Preset") {
                this->processorRef.initPreset();
                this->currentPresetName = "Default";
                this->presetFolder = "none";
                this->presetIndex = 0;
                completion(this->currentPresetName);
            } else if (action == "Load Preset") {
                this->loadPresetFromFile([this, completion](){
                    completion(this->currentPresetName);
                });
            } else if (action == "Save Preset") {
                this->savePresetToFile();
            } else if (action == "Add User Folder") {
                this->addUserFolder();
            } else if (action == "Remove User Folder") {
                this->removeUserFolder();
            }
        };

        auto presetClick = [this, completion](int presetIdx) {
            if (presetIdx >= 0 && presetIdx < static_cast<int>(this->factoryPresets.size())) {
                auto presetName = this->factoryPresetNames[static_cast<size_t>(presetIdx)];
                auto json = this->factoryPresets[presetName];
                this->processorRef.loadPreset(json);
                this->currentPresetName = presetName;
                this->presetIndex = presetIdx;
                this->presetFolder = "factory";
                completion(this->currentPresetName);
            }
        };

        auto userPresetClick = [this, completion](int presetIdx) {
            if (presetIdx >= 0 && presetIdx < static_cast<int>(this->userPresets.size())) {
                auto presetName = this->userPresetNames[static_cast<size_t>(presetIdx)];
                auto json = this->userPresets[presetName];
                this->processorRef.loadPreset(json);
                this->currentPresetName = presetName;
                this->presetIndex = presetIdx;
                this->presetFolder = "user";
                completion(this->currentPresetName);
            }
        };

        #if JUCE_MAC
            showNativeMacMenu(items, factoryItems, userItems, userFolder,
                this->currentPresetName.toStdString(), this->presetFolder.toStdString(), [items, menuClick, presetClick, 
                userPresetClick, factoryID, userContentID](int resultID) {
                if (resultID == 0) return;

                if (resultID < factoryID) {
                    auto it = items.find(resultID);
                    if (it != items.end()) {
                        menuClick(it->second);
                    }
                } else if (resultID >= factoryID && resultID < userContentID) {
                    int presetIdx = resultID - factoryID;
                    presetClick(presetIdx);
                } else if (resultID >= userContentID) {
                    int presetIdx = resultID - userContentID;
                    userPresetClick(presetIdx);
                }
            });
        #elif _WIN32
            showNativeWinMenu(items, factoryItems, userItems, userFolder,
                this->currentPresetName.toStdString(), this->presetFolder.toStdString(),
                [items, menuClick, presetClick, userPresetClick, factoryID, userContentID](int resultID) {
                    if (resultID == 0) return;
        
                    if (resultID < factoryID) {
                        auto it = items.find(resultID);
                        if (it != items.end()) {
                            menuClick(it->second);
                        }
                    } else if (resultID >= factoryID && resultID < userContentID) {
                        int presetIdx = resultID - factoryID;
                        presetClick(presetIdx);
                    } else if (resultID >= userContentID) {
                        int presetIdx = resultID - userContentID;
                        userPresetClick(presetIdx);
                    }
                });
        #else
            juce::PopupMenu menu;
            for (const auto& [id, label] : items) {
                menu.addItem(id, label);
            }

            juce::PopupMenu factoryMenu;
            for (int i = 0; i < static_cast<int>(factoryPresetNames.size()); i++) {
                int itemID = factoryID + i;
                auto name = this->factoryPresetNames[static_cast<size_t>(i)];
                bool isTicked = (this->presetFolder == "factory" && this->currentPresetName == name);
                factoryMenu.addItem(itemID, this->factoryPresetNames[static_cast<size_t>(i)], true, isTicked);
            }
            menu.addSubMenu("Factory", factoryMenu);

            if (!userFolder.empty()) {
                juce::PopupMenu userMenu;
                for (int i = 0; i < static_cast<int>(userPresetNames.size()); i++) {
                    int itemID = userContentID + i;
                    auto name = this->userPresetNames[static_cast<size_t>(i)];
                    bool isTicked = (this->presetFolder == "user" && this->currentPresetName == name);
                    userMenu.addItem(itemID, this->userPresetNames[static_cast<size_t>(i)], true, isTicked);
                }
                menu.addSubMenu(userFolder, userMenu);
            }

            auto mousePos = juce::Desktop::getMousePosition();
            
            auto options = juce::PopupMenu::Options()
                .withPreferredPopupDirection(juce::PopupMenu::Options::PopupDirection::upwards)
                .withTargetScreenArea(juce::Rectangle<int> {mousePos.x - 60, mousePos.y - 5, 1, 1});

            juce::PopupMenu::dismissAllActiveMenus();
            menu.showMenuAsync(options, [items, menuClick, presetClick, userPresetClick, factoryID, 
                userContentID, completion](int resultID) {
                if (resultID == 0) return;

                if (resultID < factoryID) {
                    auto it = items.find(resultID);
                    if (it != items.end()) {
                        auto action = it->second;
                        menuClick(action);
                    }
                } else if (resultID >= factoryID && resultID < userContentID) {
                    int presetIdx = resultID - factoryID;
                    presetClick(presetIdx);
                } else if (resultID >= userContentID) {
                    int presetIdx = resultID - userContentID;
                    userPresetClick(presetIdx);
                }
            });
        #endif
}

auto Editor::prevPreset([[maybe_unused]] const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {
        if (this->presetFolder == "factory") {
            if (this->factoryPresetNames.empty()) return;
            this->presetIndex = (presetIndex - 1 + static_cast<int>(factoryPresetNames.size())) % static_cast<int>(factoryPresetNames.size());
            auto presetName = factoryPresetNames[static_cast<size_t>(presetIndex)];
    
            auto it = factoryPresets.find(presetName);
            if (it != factoryPresets.end()) {
                auto jsonString = it->second;
                this->processorRef.loadPreset(jsonString);
                this->currentPresetName = presetName;
                return completion(presetName);
            }
        }

        if (this->presetFolder == "user") {
            if (this->userPresetNames.empty()) return;
            this->presetIndex = (presetIndex - 1 + static_cast<int>(userPresetNames.size())) % static_cast<int>(userPresetNames.size());
            auto presetName = userPresetNames[static_cast<size_t>(presetIndex)];
    
            auto it = userPresets.find(presetName);
            if (it != userPresets.end()) {
                auto jsonString = it->second;
                this->processorRef.loadPreset(jsonString);
                this->currentPresetName = presetName;
                return completion(presetName);
            }
        }
}

auto Editor::nextPreset([[maybe_unused]] const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {
        if (this->presetFolder == "factory") {
            if (this->factoryPresetNames.empty()) return;

            this->presetIndex = (presetIndex + 1) % static_cast<int>(factoryPresetNames.size());
            auto presetName = factoryPresetNames[static_cast<size_t>(presetIndex)];

            auto it = factoryPresets.find(presetName);
            if (it != factoryPresets.end()) {
                auto jsonString = it->second;
                this->processorRef.loadPreset(jsonString);
                this->currentPresetName = presetName;
                return completion(presetName);
            }
        }

        if (this->presetFolder == "user") {
            if (this->userPresetNames.empty()) return;

            this->presetIndex = (presetIndex + 1) % static_cast<int>(userPresetNames.size());
            auto presetName = userPresetNames[static_cast<size_t>(presetIndex)];

            auto it = userPresets.find(presetName);
            if (it != userPresets.end()) {
                auto jsonString = it->second;
                this->processorRef.loadPreset(jsonString);
                this->currentPresetName = presetName;
                return completion(presetName);
            }
        }
}

auto Editor::resized() -> void {
    webview.setBounds(getLocalBounds());
    Editor::setSettingKey("windowWidth", getWidth());
    Editor::setSettingKey("windowHeight", getHeight());
}

auto Editor::getWebviewFileBytes(const juce::String& resourceStr) -> std::vector<std::byte> {
    juce::MemoryInputStream zipStream(BinaryData::webview_files_zip, BinaryData::webview_files_zipSize, false);
    juce::ZipFile zip{zipStream};

    auto* entry = zip.getEntry(resourceStr);
    if (entry != nullptr) {
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

auto Editor::loadPresetFromFile(std::function<void()> onComplete) -> void {
    auto directoryPath = Editor::getSettingKey("loadDirectory", Functions::getDownloadsFolder().getFullPathName());
    juce::File directory{directoryPath};

    auto loadDialog = std::make_shared<juce::FileChooser>(
        "Load Preset", directory, "*.json"
    );

    loadDialog->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, loadDialog, onComplete](const juce::FileChooser& picker) {
            auto file = picker.getResult();
            if (file.existsAsFile()) {
                Editor::setSettingKey("loadDirectory", file.getParentDirectory().getFullPathName());
                juce::String jsonString = file.loadFileAsString();
                auto presetName = processorRef.loadPreset(jsonString);

                this->currentPresetName = presetName;
                this->presetIndex = 0;
                this->presetFolder = "none";
                onComplete();
            }
        }
    );
}

auto Editor::savePresetToFile() -> void {
    auto* infoDialog = new juce::AlertWindow("Save Preset", "Preset Information:", juce::AlertWindow::NoIcon);

    juce::String defaultName = this->currentPresetName;
    if (defaultName == "Default") defaultName.clear();
    auto defaultAuthor = Editor::getSettingKey("saveAuthor", "").toString();

    infoDialog->addTextEditor("name", defaultName, "Name:");
    infoDialog->addTextEditor("author", defaultAuthor, "Author:");
    infoDialog->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
    infoDialog->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    auto saveCallback = [this](juce::String name, juce::String author) {
        auto cleanName = Functions::cleanFilename(name);

        auto directoryPath = Editor::getSettingKey("saveDirectory", Functions::getDownloadsFolder().getFullPathName());
        juce::File directory{directoryPath};

        auto saveDialog = std::make_shared<juce::FileChooser>(
            "Save Preset", directory.getChildFile(cleanName + ".json")
        );

        saveDialog->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
            [this, saveDialog, name, author](const juce::FileChooser& picker) {
                auto file = picker.getResult();
                if (file != juce::File{}) {
                    Editor::setSettingKey("saveAuthor", author);
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

auto Editor::loadFactoryPresets() -> void {
    juce::MemoryInputStream zipStream(BinaryData::presets_zip, BinaryData::presets_zipSize, false);
    juce::ZipFile zip{zipStream};

    this->factoryPresetNames.clear();
    this->factoryPresets.clear();

    for (int i = 0; i < zip.getNumEntries(); i++) {
        auto* entry = zip.getEntry(i);

        if (entry == nullptr || !entry->filename.endsWithIgnoreCase(".json")) continue;

        std::unique_ptr<juce::InputStream> inputStream{zip.createStreamForEntry(i)};
        if (inputStream == nullptr) continue;
        
        auto content = inputStream->readEntireStreamAsString();
        auto json = juce::JSON::parse(content);

        auto* obj = json.getDynamicObject();
        if (obj != nullptr) {
            auto presetName = obj->getProperty("name").toString();
            if (presetName.isEmpty()) {
                auto entryFile = juce::File::createFileWithoutCheckingPath(entry->filename);
                presetName = entryFile.getFileNameWithoutExtension();
            }

            this->factoryPresets[presetName] = content;
            this->factoryPresetNames.push_back(presetName);
        }
    }
}

auto Editor::addUserFolder() -> void {
    auto directoryPath = Editor::getSettingKey("userFolder", Functions::getDownloadsFolder().getFullPathName());
    juce::File directory{directoryPath};

    auto openDialog = std::make_shared<juce::FileChooser>(
        "Add User Folder", directory, "*.json"
    );

    openDialog->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
        [this, openDialog](const juce::FileChooser& picker) {
            auto folder = picker.getResult();
            if (folder.exists() && folder.isDirectory()) {
                Editor::setSettingKey("userFolder", folder.getFullPathName());
                this->loadUserPresets();
            }
        }
    );
}

auto Editor::removeUserFolder() -> void {
    Editor::setSettingKey("userFolder", "");
    this->userPresetNames.clear();
    this->userPresets.clear();
}

auto Editor::loadUserPresets() -> void {
    this->userPresetNames.clear();
    this->userPresets.clear();

    auto userFolder = Editor::getSettingKey("userFolder", "").toString();
    if (userFolder.isEmpty()) return;

    juce::File userFolderDir{userFolder};
    if (!userFolderDir.exists() || !userFolderDir.isDirectory()) return;

    auto jsonFiles = userFolderDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.json");

    for (const auto& file : jsonFiles) {
        auto content = file.loadFileAsString();
        auto json = juce::JSON::parse(content);

        auto* obj = json.getDynamicObject();
        if (obj != nullptr) {
            auto presetName = obj->getProperty("name").toString();
            if (presetName.isEmpty()) presetName = file.getFileNameWithoutExtension();

            this->userPresets[presetName] = content;
            this->userPresetNames.push_back(presetName);
        }
    }
}