pragma Singleton
import QtQuick

QtObject {
    property bool darkMode: false

    // Primary colors
    readonly property color primary: darkMode ? "#60a5fa" : "#3b82f6"
    readonly property color primaryHover: darkMode ? "#93bbfd" : "#2563eb"

    // Success
    readonly property color success: darkMode ? "#4ade80" : "#22c55e"
    readonly property color successHover: darkMode ? "#86efac" : "#16a34a"

    // Danger
    readonly property color danger: darkMode ? "#f87171" : "#ef4444"
    readonly property color dangerHover: darkMode ? "#fca5a5" : "#dc2626"

    // Neutral / background
    readonly property color background: darkMode ? "#0f172a" : "#f5f7fb"
    readonly property color cardBackground: darkMode ? "#1e293b" : "#ffffff"
    readonly property color formBackground: darkMode ? "#1a2332" : "#f8fafc"
    readonly property color formBorder: darkMode ? "#334155" : "#e9edf2"

    // Text
    readonly property color textPrimary: darkMode ? "#f1f5f9" : "#0f172a"
    readonly property color textSecondary: darkMode ? "#e2e8f0" : "#1e293b"
    readonly property color textMuted: darkMode ? "#94a3b8" : "#64748b"
    readonly property color textDisabled: darkMode ? "#64748b" : "#94a3b8"

    // Borders
    readonly property color borderLight: darkMode ? "#1e293b" : "#f1f5f9"
    readonly property color borderDefault: darkMode ? "#334155" : "#d1d9e6"
    readonly property color borderFocus: darkMode ? "#60a5fa" : "#3b82f6"
    readonly property color borderHover: darkMode ? "#475569" : "#cbd5e1"

    // Radius
    readonly property real radiusSmall: 4
    readonly property real radiusMedium: 8
    readonly property real radiusLarge: 16
    readonly property real radiusCard: 24

    // Spacing
    readonly property real spacingTiny: 4
    readonly property real spacingSmall: 8
    readonly property real spacingMedium: 12
    readonly property real spacingLarge: 16
    readonly property real spacingXLarge: 32

    // Font sizes
    readonly property real fontSizeSmall: 12
    readonly property real fontSizeMedium: 14
    readonly property real fontSizeLarge: 18
    readonly property real fontSizeTitle: 24

    // Font weights
    readonly property int fontWeightNormal: Font.Normal
    readonly property int fontWeightMedium: Font.DemiBold
    readonly property int fontWeightBold: Font.Bold

    // Shadows
    readonly property color shadowLight: darkMode ? Qt.rgba(0, 0, 0, 0.2) : Qt.rgba(0, 0, 0, 0.02)
    readonly property color shadowMedium: darkMode ? Qt.rgba(0, 0, 0, 0.4) : Qt.rgba(0, 0, 0, 0.08)
}
