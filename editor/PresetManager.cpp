#include "PresetManager.h"
#include "Functions.hpp"
#include "Settings.hpp"
#include "ParameterIDs.hpp"
#include "EventEmitter.hpp"
#include "NativeMenuBridge.h"

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& tree) : tree(tree) {
    this->loadFactoryPresets();
    this->loadUserPresets();
}

auto PresetManager::openPresetMenu([[maybe_unused]] const juce::Array<juce::var>& args, 
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {

        std::map<int, std::string> items = {
            {1, "Init Preset"},
            {2, "Load Preset"},
            {3, "Save Preset"},
            {4, "Add User Folder"}
        };

        auto rawUserFolder = Settings::getSettingKey("userFolder", "").toString();
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
                this->initPreset();
                this->currentPresetName = "Default";
                this->presetFolder = "none";
                this->presetIndex = 0;
                EventEmitter::instance().emitEvent("presetChanged", this->currentPresetName);
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
                this->loadPreset(json);
                this->currentPresetName = presetName;
                this->presetIndex = presetIdx;
                this->presetFolder = "factory";
                EventEmitter::instance().emitEvent("presetChanged", this->currentPresetName);
                completion(this->currentPresetName);
            }
        };

        auto userPresetClick = [this, completion](int presetIdx) {
            if (presetIdx >= 0 && presetIdx < static_cast<int>(this->userPresets.size())) {
                auto presetName = this->userPresetNames[static_cast<size_t>(presetIdx)];
                auto json = this->userPresets[presetName];
                this->loadPreset(json);
                this->currentPresetName = presetName;
                this->presetIndex = presetIdx;
                this->presetFolder = "user";
                EventEmitter::instance().emitEvent("presetChanged", this->currentPresetName);
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

auto PresetManager::prevPreset([[maybe_unused]] const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {
        if (this->presetFolder == "factory") {
            if (this->factoryPresetNames.empty()) return;
            this->presetIndex = (this->presetIndex - 1 + static_cast<int>(factoryPresetNames.size())) % static_cast<int>(factoryPresetNames.size());
        }

        if (this->presetFolder == "user") {
            if (this->userPresetNames.empty()) return;
            this->presetIndex = (this->presetIndex - 1 + static_cast<int>(userPresetNames.size())) % static_cast<int>(userPresetNames.size());
        }

        auto presetName = this->setPreset(this->presetIndex);
        return completion(presetName);
}

auto PresetManager::nextPreset([[maybe_unused]] const juce::Array<juce::var>& args,
    juce::WebBrowserComponent::NativeFunctionCompletion completion) -> void {
        if (this->presetFolder == "factory") {
            if (this->factoryPresetNames.empty()) return;
            this->presetIndex = (this->presetIndex + 1) % static_cast<int>(factoryPresetNames.size());
        }

        if (this->presetFolder == "user") {
            if (this->userPresetNames.empty()) return;
            this->presetIndex = (this->presetIndex + 1) % static_cast<int>(userPresetNames.size());
        }

        auto presetName = this->setPreset(this->presetIndex);
        return completion(presetName);
}

auto PresetManager::setPreset(int _presetIndex) -> juce::String {
    this->presetIndex = _presetIndex;

    if (this->presetFolder == "factory") {
        if (this->factoryPresetNames.empty()) return this->currentPresetName;
        auto presetName = factoryPresetNames[static_cast<size_t>(this->presetIndex)];

        auto it = factoryPresets.find(presetName);
        if (it != factoryPresets.end()) {
            auto jsonString = it->second;
            this->loadPreset(jsonString);
            this->currentPresetName = presetName;
        }
    }

    if (this->presetFolder == "user") {
        if (this->userPresetNames.empty()) return this->currentPresetName;
        auto presetName = userPresetNames[static_cast<size_t>(this->presetIndex)];

        auto it = userPresets.find(presetName);
        if (it != userPresets.end()) {
            auto jsonString = it->second;
            this->loadPreset(jsonString);
            this->currentPresetName = presetName;
        }
    }

    EventEmitter::instance().emitEvent("presetChanged", this->currentPresetName);
    return this->currentPresetName;
}

auto PresetManager::loadPresetFromFile(std::function<void()> onComplete) -> void {
    auto directoryPath = Settings::getSettingKey("loadDirectory", Functions::getDownloadsFolder().getFullPathName());
    juce::File directory{directoryPath};

    auto loadDialog = std::make_shared<juce::FileChooser>(
        "Load Preset", directory, "*.json"
    );

    loadDialog->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, loadDialog, onComplete](const juce::FileChooser& picker) {
            auto file = picker.getResult();
            if (file.existsAsFile()) {
                Settings::setSettingKey("loadDirectory", file.getParentDirectory().getFullPathName());
                auto jsonString = file.loadFileAsString();
                auto presetName = this->loadPreset(jsonString);

                this->currentPresetName = presetName;
                this->presetIndex = 0;
                this->presetFolder = "none";
                onComplete();
            }
        }
    );
}

auto PresetManager::savePresetToFile() -> void {
    auto* infoDialog = new juce::AlertWindow("Save Preset", "Preset Information:", juce::AlertWindow::NoIcon);

    auto defaultName = this->currentPresetName;
    if (defaultName == "Default") defaultName.clear();
    auto defaultAuthor = Settings::getSettingKey("saveAuthor", "").toString();

    infoDialog->addTextEditor("name", defaultName, "Name:");
    infoDialog->addTextEditor("author", defaultAuthor, "Author:");
    infoDialog->addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
    infoDialog->addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));

    auto saveCallback = [this](juce::String name, juce::String author) {
        auto cleanName = Functions::cleanFilename(name);

        auto directoryPath = Settings::getSettingKey("saveDirectory", Functions::getDownloadsFolder().getFullPathName());
        juce::File directory{directoryPath};

        auto saveDialog = std::make_shared<juce::FileChooser>(
            "Save Preset", directory.getChildFile(cleanName + ".json")
        );

        saveDialog->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles,
            [this, saveDialog, name, author](const juce::FileChooser& picker) {
                auto file = picker.getResult();
                if (file != juce::File{}) {
                    Settings::setSettingKey("saveAuthor", author);
                    Settings::setSettingKey("saveDirectory", file.getParentDirectory().getFullPathName());
                    auto jsonString = this->savePreset(name, author);
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

auto PresetManager::loadFactoryPresets() -> void {
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

        if (auto* obj = json.getDynamicObject()) {
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

auto PresetManager::addUserFolder() -> void {
    auto directoryPath = Settings::getSettingKey("userFolder", Functions::getDownloadsFolder().getFullPathName());
    juce::File directory{directoryPath};

    auto openDialog = std::make_shared<juce::FileChooser>(
        "Add User Folder", directory, "*.json"
    );

    openDialog->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
        [this, openDialog](const juce::FileChooser& picker) {
            auto folder = picker.getResult();
            if (folder.exists() && folder.isDirectory()) {
                Settings::setSettingKey("userFolder", folder.getFullPathName());
                this->loadUserPresets();
            }
        }
    );
}

auto PresetManager::removeUserFolder() -> void {
    Settings::setSettingKey("userFolder", "");
    this->userPresetNames.clear();
    this->userPresets.clear();
}

auto PresetManager::loadUserPresets() -> void {
    this->userPresetNames.clear();
    this->userPresets.clear();

    auto userFolder = Settings::getSettingKey("userFolder", "").toString();
    if (userFolder.isEmpty()) return;

    juce::File userFolderDir{userFolder};
    if (!userFolderDir.exists() || !userFolderDir.isDirectory()) return;

    auto jsonFiles = userFolderDir.findChildFiles(juce::File::TypesOfFileToFind::findFiles, false, "*.json");

    for (const auto& file : jsonFiles) {
        auto content = file.loadFileAsString();
        auto json = juce::JSON::parse(content);

        if (auto* obj = json.getDynamicObject()) {
            auto presetName = obj->getProperty("name").toString();
            if (presetName.isEmpty()) presetName = file.getFileNameWithoutExtension();

            this->userPresets[presetName] = content;
            this->userPresetNames.push_back(presetName);
        }
    }
}

auto PresetManager::savePreset(const juce::String& name, const juce::String& author) -> juce::String {
    auto obj = std::make_unique<juce::DynamicObject>();

    obj->setProperty("plugin", JucePlugin_Name);
    obj->setProperty("version", JucePlugin_VersionString);
    obj->setProperty("name", name);
    obj->setProperty("author", author);
    obj->setProperty("modified", juce::Time::getCurrentTime().toISO8601(true));
    obj->setProperty("presetFormat", 1);

    auto parameters = std::make_unique<juce::DynamicObject>();

    for (const auto& id : ParameterIDs::getStringKeys()) {
        auto* param = this->tree.getParameter(id);

        if (param) {
            parameters->setProperty(id, param->getCurrentValueAsText());
        }
    }

    obj->setProperty("parameters", juce::var(parameters.release()));
    auto json = juce::var{obj.release()};

    return juce::JSON::toString(json);
}

auto PresetManager::loadPreset(const juce::String& jsonStr) -> juce::String {
    auto parsed = juce::JSON::fromString(jsonStr);
    auto* obj = parsed.getDynamicObject();
    if (obj == nullptr) return "";

    juce::String presetName = "";

    if (obj->hasProperty("name")) {
        presetName = obj->getProperty("name").toString();
    }

    auto parameters = juce::var{obj->getProperty("parameters")};
    auto* paramObj = parameters.getDynamicObject();
    if (paramObj == nullptr) return "";

    for (const auto& property : paramObj->getProperties()) {
        auto id = property.name.toString();
        auto* param = this->tree.getParameter(id);

        float value = param->getValueForText(property.value.toString());
        param->setValueNotifyingHost(value);
    }

    return presetName;
}

auto PresetManager::initPreset() -> void {
    for (const auto& id : ParameterIDs::getStringKeys()) {
        auto* param = this->tree.getParameter(id);
        param->setValueNotifyingHost(param->getDefaultValue());
    }
}