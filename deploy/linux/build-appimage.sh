#!/bin/bash
# ChecklistApp AppImage Builder
# Requires: linuxdeployqt, Qt 6

set -e

APP_NAME="ChecklistApp"
APP_VERSION="1.0.0"
BUILD_DIR="build"
APPDIR="${BUILD_DIR}/AppDir"

echo "=== Building AppImage for ${APP_NAME} ==="

# Clean previous build
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

# Build with CMake
echo "--- Building with CMake ---"
cmake -B "${BUILD_DIR}" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=/usr

cmake --build "${BUILD_DIR}" --config Release

# Install to AppDir
echo "--- Installing to AppDir ---"
cmake --install "${BUILD_DIR}" --prefix "${APPDIR}/usr"

# Create desktop entry
mkdir -p "${APPDIR}/usr/share/applications"
cat > "${APPDIR}/usr/share/applications/checklistapp.desktop" << EOF
[Desktop Entry]
Type=Application
Name=ChecklistApp
GenericName=Checklist Application
Comment=Gestión de correcciones y checklists
Exec=checklistapp %f
Icon=checklistapp
Categories=Utility;Office;
Terminal=false
StartupWMClass=ChecklistApp
EOF

# Create icon directory
mkdir -p "${APPDIR}/usr/share/icons/hicolor/scalable/apps"
cp "resources/icons/app.svg" "${APPDIR}/usr/share/icons/hicolor/scalable/apps/checklistapp.svg"

# Copy AppRun script
cat > "${APPDIR}/AppRun" << 'EOF'
#!/bin/bash
SELF=$(readlink -f "$0")
HERE=${SELF%/*}
export PATH="${HERE}/usr/bin:${PATH}"
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"
exec "${HERE}/usr/bin/checklistapp" "$@"
EOF
chmod +x "${APPDIR}/AppRun"

# Download linuxdeployqt if not available
if ! command -v linuxdeployqt &> /dev/null; then
    echo "--- Downloading linuxdeployqt ---"
    wget -c "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage" -O linuxdeployqt
    chmod +x linuxdeployqt
    ./linuxdeployqt "${APPDIR}/usr/bin/checklistapp" -appimage
else
    linuxdeployqt "${APPDIR}/usr/bin/checklistapp" -appimage
fi

# Create AppImage
echo "--- Creating AppImage ---"
APPIMAGE_NAME="${APP_NAME}-${APP_VERSION}-x86_64.AppImage"
mv "${APP_NAME}"-*.AppImage "${BUILD_DIR}/${APPIMAGE_NAME}"

echo "=== AppImage created: ${BUILD_DIR}/${APPIMAGE_NAME} ==="
