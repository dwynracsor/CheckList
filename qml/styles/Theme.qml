pragma Singleton
import QtQuick

QtObject {
    // Primary colors (from CSS #3b82f6)
    readonly property color primary: "#3b82f6"
    readonly property color primaryHover: "#2563eb"

    // Success (from CSS #22c55e)
    readonly property color success: "#22c55e"
    readonly property color successHover: "#16a34a"

    // Danger (from CSS #ef4444)
    readonly property color danger: "#ef4444"
    readonly property color dangerHover: "#dc2626"

    // Neutral / background
    readonly property color background: "#f5f7fb"
    readonly property color cardBackground: "#ffffff"
    readonly property color formBackground: "#f8fafc"
    readonly property color formBorder: "#e9edf2"

    // Text
    readonly property color textPrimary: "#0f172a"
    readonly property color textSecondary: "#1e293b"
    readonly property color textMuted: "#64748b"
    readonly property color textDisabled: "#94a3b8"

    // Borders
    readonly property color borderLight: "#f1f5f9"
    readonly property color borderDefault: "#d1d9e6"
    readonly property color borderFocus: "#3b82f6"
    readonly property color borderHover: "#cbd5e1"

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
    readonly property color shadowLight: Qt.rgba(0, 0, 0, 0.02)
    readonly property color shadowMedium: Qt.rgba(0, 0, 0, 0.08)
}
