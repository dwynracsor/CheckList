#pragma once
#include <QString>

// Paleta y estilos replicando la imagen de referencia.
// Se definen dos variantes (claro / oscuro) que se alternan
// con el toggle de la cabecera.

namespace Styles {

// ---------- MODO CLARO ----------
inline QString light() {
    return R"(
        QWidget#RootBackground {
            background-color: #FFFFFF;
        }

        QLabel#AppTitle {
            color: #111111;
            font-weight: 700;
        }

        QLabel#SectionLabel {
            color: #111111;
            font-weight: 500;
        }

        QLineEdit {
            background-color: #F2F2F2;
            border: none;
            border-radius: 12px;
            padding: 10px 14px;
            color: #1A1A1A;
        }
        QLineEdit::placeholder {
            color: #9E9E9E;
        }

        QPushButton#PlusButton {
            background-color: #111111;
            color: #FFFFFF;
            border: none;
            border-radius: 20px;
            font-size: 18px;
            font-weight: 700;
        }
        QPushButton#PlusButton:hover { background-color: #2A2A2A; }
        QPushButton#PlusButton:pressed { background-color: #000000; }

        QPushButton#NewItemButton {
            background-color: #F2F2F2;
            color: #1A1A1A;
            border: none;
            border-radius: 12px;
            padding: 10px;
        }
        QPushButton#NewItemButton:hover { background-color: #E8E8E8; }

        QPushButton#CreateListButton {
            background-color: #111111;
            color: #FFFFFF;
            border: none;
            border-radius: 12px;
            padding: 10px;
            font-weight: 600;
        }
        QPushButton#CreateListButton:disabled {
            background-color: #D0D0D0;
            color: #FFFFFF;
        }
        QPushButton#CreateListButton:hover:!disabled { background-color: #2A2A2A; }

        QWidget#CreateBox, QWidget#ListCard {
            background-color: #F5F5F5;
            border-radius: 14px;
        }

        QPlainTextEdit {
            background-color: #F2F2F2;
            border: none;
            border-radius: 12px;
            padding: 10px 14px;
            color: #1A1A1A;
        }
        QPlainTextEdit QScrollBar { width: 0; height: 0; }

        QLabel#ItemText {
            color: #1A1A1A;
        }
        QLabel#ItemText[completed="true"] {
            color: #BDBDBD;
        }

        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border-radius: 4px;
            border: 2px solid #C7C7C7;
            background: transparent;
        }
        QCheckBox::indicator:checked {
            background-color: #111111;
            border: 2px solid #111111;
            image: none;
        }

        QToolButton#DeleteX {
            color: #B0B0B0;
            border: none;
            font-weight: 600;
        }
        QToolButton#DeleteX:hover { color: #E53935; }

        QToolButton#EditPencil {
            color: #9E9E9E;
            border: none;
        }
        QToolButton#EditPencil:hover { color: #111111; }

        QToolButton#MinusButton {
            color: #111111;
            border: none;
            font-weight: 700;
            font-size: 16px;
        }

        QLabel#ListName {
            color: #111111;
            font-weight: 500;
        }
        QLabel#Slash {
            color: #C7C7C7;
        }

        QLabel#DateLabel {
            color: #9E9E9E;
            font-size: 11px;
        }

        QLabel#ProgressLabel {
            color: #9E9E9E;
            font-size: 11px;
        }

        QProgressBar#ProgressPill {
            background-color: #E0E0E0;
            border: none;
            border-radius: 4px;
            max-height: 8px;
        }
        QProgressBar#ProgressPill::chunk {
            background-color: #111111;
            border-radius: 4px;
        }

        QLabel#BellIcon {
            color: #111111;
        }
        QLabel#Badge {
            background-color: #E53935;
            color: #FFFFFF;
            border-radius: 8px;
            font-size: 10px;
            font-weight: 700;
        }

        QScrollArea { border: none; background: transparent; }
        QScrollArea > QWidget > QWidget { background: transparent; }
        QScrollBar:vertical { width: 6px; background: transparent; }
        QScrollBar::handle:vertical { background: #D8D8D8; border-radius: 3px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )";
}

// ---------- MODO OSCURO ----------
inline QString dark() {
    return R"(
        QWidget#RootBackground {
            background-color: #121212;
        }

        QLabel#AppTitle {
            color: #F2F2F2;
            font-weight: 700;
        }

        QLabel#SectionLabel {
            color: #F2F2F2;
            font-weight: 500;
        }

        QLineEdit {
            background-color: #1E1E1E;
            border: none;
            border-radius: 12px;
            padding: 10px 14px;
            color: #EDEDED;
        }
        QLineEdit::placeholder {
            color: #777777;
        }

        QPushButton#PlusButton {
            background-color: #F2F2F2;
            color: #111111;
            border: none;
            border-radius: 20px;
            font-size: 18px;
            font-weight: 700;
        }
        QPushButton#PlusButton:hover { background-color: #FFFFFF; }
        QPushButton#PlusButton:pressed { background-color: #DDDDDD; }

        QPushButton#NewItemButton {
            background-color: #1E1E1E;
            color: #EDEDED;
            border: none;
            border-radius: 12px;
            padding: 10px;
        }
        QPushButton#NewItemButton:hover { background-color: #2A2A2A; }

        QPushButton#CreateListButton {
            background-color: #F2F2F2;
            color: #111111;
            border: none;
            border-radius: 12px;
            padding: 10px;
            font-weight: 600;
        }
        QPushButton#CreateListButton:disabled {
            background-color: #3A3A3A;
            color: #888888;
        }
        QPushButton#CreateListButton:hover:!disabled { background-color: #FFFFFF; }

        QWidget#CreateBox, QWidget#ListCard {
            background-color: #1A1A1A;
            border-radius: 14px;
        }

        QPlainTextEdit {
            background-color: #1E1E1E;
            border: none;
            border-radius: 12px;
            padding: 10px 14px;
            color: #EDEDED;
        }
        QPlainTextEdit QScrollBar { width: 0; height: 0; }

        QLabel#ItemText {
            color: #EDEDED;
        }
        QLabel#ItemText[completed="true"] {
            color: #6A6A6A;
        }

        QCheckBox::indicator {
            width: 16px;
            height: 16px;
            border-radius: 4px;
            border: 2px solid #4A4A4A;
            background: transparent;
        }
        QCheckBox::indicator:checked {
            background-color: #F2F2F2;
            border: 2px solid #F2F2F2;
            image: none;
        }

        QToolButton#DeleteX {
            color: #5A5A5A;
            border: none;
            font-weight: 600;
        }
        QToolButton#DeleteX:hover { color: #FF6B60; }

        QToolButton#EditPencil {
            color: #777777;
            border: none;
        }
        QToolButton#EditPencil:hover { color: #F2F2F2; }

        QToolButton#MinusButton {
            color: #F2F2F2;
            border: none;
            font-weight: 700;
            font-size: 16px;
        }

        QLabel#ListName {
            color: #F2F2F2;
            font-weight: 500;
        }
        QLabel#Slash {
            color: #4A4A4A;
        }

        QLabel#DateLabel {
            color: #777777;
            font-size: 11px;
        }

        QLabel#ProgressLabel {
            color: #777777;
            font-size: 11px;
        }

        QProgressBar#ProgressPill {
            background-color: #333333;
            border: none;
            border-radius: 4px;
            max-height: 8px;
        }
        QProgressBar#ProgressPill::chunk {
            background-color: #F2F2F2;
            border-radius: 4px;
        }

        QLabel#BellIcon {
            color: #111111;
        }
        QLabel#Badge {
            background-color: #E53935;
            color: #FFFFFF;
            border-radius: 8px;
            font-size: 10px;
            font-weight: 700;
        }

        QScrollArea { border: none; background: transparent; }
        QScrollArea > QWidget > QWidget { background: transparent; }
        QScrollBar:vertical { width: 6px; background: transparent; }
        QScrollBar::handle:vertical { background: #3A3A3A; border-radius: 3px; }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }
    )";
}

} // namespace Styles
