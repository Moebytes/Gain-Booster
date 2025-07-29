cmake_minimum_required(VERSION 3.31)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(GainBooster VERSION 0.0.3)

option(WEBVIEW_DEV_MODE "Enable webview dev mode (load from disk)" OFF)

include(FetchContent)

FetchContent_Declare(
  juce
  GIT_REPOSITORY https://github.com/juce-framework/JUCE
  GIT_TAG        8.0.8
)
FetchContent_MakeAvailable(juce)

juce_add_plugin(${PROJECT_NAME}
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon_big.ico"
    ICON_SMALL "${CMAKE_CURRENT_SOURCE_DIR}/assets/icon_small.ico"
    COMPANY_NAME Moebytes
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
    PLUGIN_MANUFACTURER_CODE Mbit
    PLUGIN_CODE Gain
    FORMATS VST3 AU Standalone
    PRODUCT_NAME "Gain Booster"
    NEEDS_WEB_BROWSER TRUE
    NEEDS_WEBVIEW2 TRUE
)

if (APPLE)
    set(MACOSX_BUNDLE_ICON_FILE "icon_big.icns")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/assets/icon_big.icns PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources")
    
    set_target_properties(${PROJECT_NAME} PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_ICON_FILE ${MACOSX_BUNDLE_ICON_FILE}
    )

    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/assets/icon_big.icns)
endif()

if (WIN32)
    target_sources(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/assets/icon.rc)
endif()

juce_generate_juce_header(${PROJECT_NAME})

file(GLOB SRC_FILES "editor/*.cpp" "processor/*.cpp" "structures/*.cpp")

if (APPLE)
    file(GLOB OBJC_SRC_FILES "editor/*.mm")
endif()

target_sources(${PROJECT_NAME} PRIVATE ${SRC_FILES} ${OBJC_SRC_FILES})

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
        JUCE_WEB_BROWSER=1
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_USE_WIN_WEBVIEW2_WITH_STATIC_LINKING=1
        $<$<BOOL:${WEBVIEW_DEV_MODE}>:WEBVIEW_DEV_MODE=1>)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/editor
    ${CMAKE_CURRENT_SOURCE_DIR}/processor
    ${CMAKE_CURRENT_SOURCE_DIR}/structures
)

set(WEBVIEW_FILES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist")
set(WEBVIEW_ZIP "${CMAKE_BINARY_DIR}/webview_files.zip")

file(GLOB_RECURSE WEBVIEW_FILES RELATIVE ${WEBVIEW_FILES_DIR} "${WEBVIEW_FILES_DIR}/*")
file(
    ARCHIVE_CREATE
    OUTPUT ${WEBVIEW_ZIP}
    PATHS ${WEBVIEW_FILES}
    WORKING_DIRECTORY ${WEBVIEW_FILES_DIR}
    FORMAT zip VERBOSE
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_dsp
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags)