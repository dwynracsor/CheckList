#pragma once
#include <QMainWindow>
#include <QVector>
#include <QString>

class QLineEdit;
class QLabel;
class QPushButton;
class QToolButton;
class QVBoxLayout;
class ItemAddRow;
class ToggleSwitch;
class TaskController;

// Ventana principal: cabecera (título + toggle + notificaciones),
// búsqueda, panel "Create list" y listado "My lists".
// Toda la información se gestiona a través de TaskController (persistencia).
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private:
    TaskController *m_controller;
    QVector<QString> m_draftItems; // textos de los ítems del panel "Create list"

    QLineEdit *m_searchBar;
    QLineEdit *m_listNameInput;
    QWidget *m_draftItemsContainer;
    QVBoxLayout *m_draftItemsLayout;
    QPushButton *m_createListBtn;
    QWidget *m_createBox;
    QToolButton *m_chevronBtn;
    QWidget *m_myListsContainer;
    QVBoxLayout *m_myListsLayout;
    ToggleSwitch *m_themeToggle;
    QLabel *m_bell = nullptr;
    bool m_darkMode = false;

    void applyTheme();
    void rebuildDraftItems();
    void updateCreateButtonState();
    void rebuildMyLists();
    void addNewList();
};
