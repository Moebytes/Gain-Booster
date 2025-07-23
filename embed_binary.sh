#!/bin/bash

BINARY_BUILDER=~/Documents/JUCE/extras/BinaryBuilder/Builds/MacOSX/build/Debug/BinaryBuilder

PARAMETERS_JSON="processor/parameters.json"
WEBVIEW_ZIP="build/webview_files.zip"
OUTPUT_DIR="editor"
CLASS_NAME="BinaryData"

mkdir -p "$OUTPUT_DIR"

TMP_DIR=$(mktemp -d)
cp "$PARAMETERS_JSON" "$TMP_DIR/"
cp "$WEBVIEW_ZIP" "$TMP_DIR/"

"$BINARY_BUILDER" "$TMP_DIR" "$OUTPUT_DIR" "$CLASS_NAME"

rm -rf "$TMP_DIR"

echo " - $OUTPUT_DIR/${CLASS_NAME}.cpp"
echo " - $OUTPUT_DIR/${CLASS_NAME}.h"