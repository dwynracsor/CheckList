import QtQuick 2.15

QtObject {
    id: theme

    readonly property var lightColors: {
        "textPrimary": "#212121",
        "textSecondary": "#757575",
        "textMuted": "#9E9E9E",
        "textOnAccent": "#FFFFFF",
        "accent": "#3F51B5",
        "accentDark": "#303F9F",
        "border": "#E0E0E0",
        "cardBackground": "#FAFAFA",
        "surface": "#FFFFFF",
        "background": "#F5F5F5",
        "error": "#F44336",
        "warning": "#FF9800",
        "success": "#4CAF50",
        "alertBackground": "#FFF3E0"
    }

    readonly property var darkColors: {
        "textPrimary": "#FAFAFA",
        "textSecondary": "#B0B0B0",
        "textMuted": "#757575",
        "textOnAccent": "#FFFFFF",
        "accent": "#5C6BC0",
        "accentDark": "#3F51B5",
        "border": "#424242",
        "cardBackground": "#2C2C2C",
        "surface": "#333333",
        "background": "#212121",
        "error": "#EF5350",
        "warning": "#FFB74D",
        "success": "#66BB6A",
        "alertBackground": "#4E342E"
    }

    readonly property var spacing: {
        "extraSmall": 4,
        "small": 8,
        "medium": 16,
        "large": 24
    }
}