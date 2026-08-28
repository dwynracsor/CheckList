#!/bin/bash
# ChecklistApp macOS DMG Builder
# Requires: Qt 6, create-dmg

set -e

APP_NAME="ChecklistApp"
APP_VERSION="1.0.0"
BUILD_DIR="build"
DMG_NAME="${APP_NAME}-${APP_VERSION}-macOS.dmg"

echo "=== Building DMG for ${APP_NAME} ==="

# Clean previous build
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

# Build with CMake
echo "--- Building with CMake ---"
cmake -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0

cmake --build "${BUILD_DIR}" --config Release

# Create app bundle structure
echo "--- Creating App Bundle ---"
APP_BUNDLE="${BUILD_DIR}/${APP_NAME}.app"
mkdir -p "${APP_BUNDLE}/Contents/MacOS"
mkdir -p "${APP_BUNDLE}/Contents/Resources"

# Copy executable
cp "${BUILD_DIR}/${APP_NAME}" "${APP_BUNDLE}/Contents/MacOS/"

# Copy QML modules
mkdir -p "${APP_BUNDLE}/Contents/Resources/qml"
cp -r "${BUILD_DIR}/_deps/"* "${APP_BUNDLE}/Contents/Resources/qml/" 2>/dev/null || true

# Create Info.plist
cat > "${APP_BUNDLE}/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>es</string>
    <key>CFBundleExecutable</key>
    <string>${APP_NAME}</string>
    <key>CFBundleIconFile</key>
    <string>app.icns</string>
    <key>CFBundleIdentifier</key>
    <string>com.checklistapp.desktop</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>${APP_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>${APP_VERSION}</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSMinimumSystemVersion</key>
    <string>11.0</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSHumanReadableCopyright</key>
    <string>Copyright © 2024 ChecklistApp</string>
</dict>
</plist>
EOF

# Convert icon to icns (requires iconutil)
if [ -f "resources/icons/icon.iconset" ]; then
    iconutil -c icns "resources/icons/icon.iconset" -o "${APP_BUNDLE}/Contents/Resources/app.icns"
fi

# Deploy Qt frameworks
echo "--- Deploying Qt frameworks ---"
macdeployqt "${APP_BUNDLE}"

# Create DMG
echo "--- Creating DMG ---"
if command -v create-dmg &> /dev/null; then
    create-dmg \
        --volname "${APP_NAME}" \
        --volicon "resources/icons/app.icns" \
        --window-pos 200 120 \
        --window-size 600 400 \
        --icon-size 100 \
        --icon "${APP_NAME}.app" 150 190 \
        --hide-extension "${APP_NAME}.app" \
        --app-drop-link 450 190 \
        "${BUILD_DIR}/${DMG_NAME}" \
        "${APP_BUNDLE}"
else
    echo "create-dmg not found. Installing via Homebrew..."
    brew install create-dmg
    create-dmg \
        --volname "${APP_NAME}" \
        --volicon "resources/icons/app.icns" \
        --window-pos 200 120 \
        --window-size 600 400 \
        --icon-size 100 \
        --icon "${APP_NAME}.app" 150 190 \
        --hide-extension "${APP_NAME}.app" \
        --app-drop-link 450 190 \
        "${BUILD_DIR}/${DMG_NAME}" \
        "${APP_BUNDLE}"
fi

echo "=== DMG created: ${BUILD_DIR}/${DMG_NAME} ==="
