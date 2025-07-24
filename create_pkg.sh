#!/bin/bash

PLUGIN_NAME="Gain Plugin"
VERSION="0.0.1"
IDENTIFIER_BASE="com.moebytes.gain"
BUILD_DIR="./build/GainPlugin_artefacts/Release"
RELEASE_DIR="./release"

rm -rf "$RELEASE_DIR"
mkdir -p "$RELEASE_DIR"

create_pkg() {
  local component_name=$1
  local src_path=$2
  local dest_path=$3
  local identifier="${IDENTIFIER_BASE}.$(echo "$component_name" | tr '[:upper:]' '[:lower:]')"

  local pkg_root="./pkgroot_$component_name"
  rm -rf "$pkg_root"
  mkdir -p "$pkg_root$dest_path"

  cp -R "$src_path" "$pkg_root$dest_path"

  pkgbuild \
    --root "$pkg_root" \
    --identifier "$identifier" \
    --version "$VERSION" \
    --install-location / \
    --ownership recommended \
    "$RELEASE_DIR/${PLUGIN_NAME}_${component_name}.pkg"

  rm -rf "$pkg_root"
}

echo "Building component packages"

create_pkg "VST3" "$BUILD_DIR/VST3/${PLUGIN_NAME}.vst3" "/Library/Audio/Plug-Ins/VST3"

create_pkg "AU" "$BUILD_DIR/AU/${PLUGIN_NAME}.component" "/Library/Audio/Plug-Ins/Components"

create_pkg "Standalone" "$BUILD_DIR/Standalone/${PLUGIN_NAME}.app" "/Applications"

echo "Building final installer package"

productbuild \
  --distribution "./distribution.xml" \
  --package-path "$RELEASE_DIR" \
  --resources "$RELEASE_DIR/resources" \
  "$RELEASE_DIR/${PLUGIN_NAME}_Installer.pkg"

echo "Created at $RELEASE_DIR/${PLUGIN_NAME}_Installer.pkg"