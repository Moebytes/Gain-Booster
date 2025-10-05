@echo off
setlocal enabledelayedexpansion

set BINARY_BUILDER=%USERPROFILE%\Documents\JUCE\extras\BinaryBuilder\Builds\VisualStudio2022\x64\Debug\ConsoleApp\BinaryBuilder.exe

set PARAMETERS_JSON=processor\parameters.json
set WEBVIEW_ZIP=build\webview_files.zip
set PRESETS_ZIP=build\presets.zip
set OUTPUT_DIR=editor
set CLASS_NAME=BinaryData

if not exist "%OUTPUT_DIR%" (
    mkdir "%OUTPUT_DIR%"
)

for /f "delims=" %%I in ('powershell -command "New-Item -ItemType Directory -Path ([System.IO.Path]::GetTempPath()) -Name ([System.Guid]::NewGuid().ToString())"') do set TMP_DIR=%%I

copy "%PARAMETERS_JSON%" "%TMP_DIR%\"
copy "%WEBVIEW_ZIP%" "%TMP_DIR%\"
copy "%PRESETS_ZIP%" "%TMP_DIR%\"

"%BINARY_BUILDER%" "%TMP_DIR%" "%OUTPUT_DIR%" "%CLASS_NAME%"

rmdir /s /q "%TMP_DIR%"

echo  - %OUTPUT_DIR%\%CLASS_NAME%.cpp
echo  - %OUTPUT_DIR%\%CLASS_NAME%.h

endlocal