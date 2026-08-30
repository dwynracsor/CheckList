#include "mainwindow.h"
#include "toggleswitch.h"
#include "itemrow.h"
#include "itemaddrow.h"
#include "listcard.h"
#include "styles.h"
#include "controllers/TaskController.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QToolButton>
#include <QScrollArea>
#include <QFrame>
#include <QDate>
#include <QSvgRenderer>
#include <QPixmap>
#include <QPainter>

// ---------------------------------------------------------------
// Pequeño widget del icono de notificaciones con badge (🔔 + n)
// ---------------------------------------------------------------
namespace {
QPixmap makeBellPixmap(const QColor &color) {
    // Icono de campana en SVG; se repinta en el color deseado para
    // que sea visible tanto en tema claro como oscuro.
    static const char kBellSvg[] = R"(<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24"><path d="M12 22c1.1 0 2-.9 2-2h-4c0 1.1.9 2 2 2zm6-6v-5c0-3.07-1.63-5.64-4.5-6.32V4c0-.83-.67-1.5-1.5-1.5s-1.5.67-1.5 1.5v.68C7.64 5.36 6 7.92 6 11v5l-2 2v1h16v-1l-2-2z"/></svg>)";

    QPixmap pm(20, 20);
    pm.fill(Qt::transparent);
    QSvgRenderer renderer;
    renderer.load(QString::fromUtf8(kBellSvg));
    QPainter painter(&pm);
    renderer.render(&painter);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pm.rect(), color);
    return pm;
}

QWidget *makeBellWithBadge(QWidget *parent, int count, const QColor &color, QLabel **outBell) {
    auto *container = new QWidget(parent);
    container->setFixedSize(32, 30);

    auto *bell = new QLabel(container);
    bell->setObjectName("BellIcon");
    bell->setPixmap(makeBellPixmap(color));
    bell->setGeometry(2, 4, 22, 22);
    bell->setAlignment(Qt::AlignCenter);
    if (outBell) *outBell = bell;

    if (count > 0) {
        auto *badge = new QLabel(QString::number(count), container);
        badge->setObjectName("Badge");
        badge->setAlignment(Qt::AlignCenter);
        badge->setGeometry(16, 1, 14, 14);
    }
    return container;
}
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    m_controller = new TaskController(this);
    m_controller->load();

    setWindowTitle("CheckList");
    resize(440, 800);

    auto *root = new QWidget(this);
    root->setObjectName("RootBackground");
    setCentralWidget(root);

    auto *mainLayout = new QVBoxLayout(root);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(14);

    // -------- Top bar: título + toggle + campana --------
    auto *topBar = new QHBoxLayout();
    auto *title = new QLabel("<u>Check</u>List", this);
    title->setObjectName("AppTitle");
    QFont titleFont = title->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    title->setFont(titleFont);

    m_themeToggle = new ToggleSwitch(this);
    m_themeToggle->setFixedSize(40, 22);

    topBar->addWidget(title);
    topBar->addSpacing(10);
    topBar->addWidget(m_themeToggle);
    topBar->addStretch(1);
    topBar->addWidget(makeBellWithBadge(this, 1, QColor(m_darkMode ? "#F2F2F2" : "#111111"), &m_bell));
    mainLayout->addLayout(topBar);

    // -------- Barra de búsqueda --------
    m_searchBar = new QLineEdit(this);
    m_searchBar->setPlaceholderText(tr("Search list"));
    mainLayout->addWidget(m_searchBar);

    // -------- Encabezado "Create list" + chevron --------
    auto *createHeader = new QHBoxLayout();
    auto *createLabel = new QLabel(tr("Create list"), this);
    createLabel->setObjectName("SectionLabel");
    QFont clFont = createLabel->font();
    clFont.setPointSize(13);
    createLabel->setFont(clFont);

    m_chevronBtn = new QToolButton(this);
    m_chevronBtn->setText("\u25BC"); // ▼
    m_chevronBtn->setAutoRaise(true);
    m_chevronBtn->setCursor(Qt::PointingHandCursor);

    createHeader->addWidget(createLabel);
    createHeader->addStretch(1);
    createHeader->addWidget(m_chevronBtn);
    mainLayout->addLayout(createHeader);

    // -------- Caja "Create list" --------
    m_createBox = new QWidget(this);
    m_createBox->setObjectName("CreateBox");
    auto *createBoxLayout = new QVBoxLayout(m_createBox);
    createBoxLayout->setContentsMargins(14, 14, 14, 14);
    createBoxLayout->setSpacing(10);

    m_listNameInput = new QLineEdit(m_createBox);
    m_listNameInput->setPlaceholderText(tr("List name"));
    createBoxLayout->addWidget(m_listNameInput);

    auto *addRow = new ItemAddRow(m_createBox);
    createBoxLayout->addWidget(addRow);

    m_draftItemsContainer = new QWidget(m_createBox);
    m_draftItemsLayout = new QVBoxLayout(m_draftItemsContainer);
    m_draftItemsLayout->setContentsMargins(0, 0, 0, 0);
    m_draftItemsLayout->setSpacing(2);
    createBoxLayout->addWidget(m_draftItemsContainer);

    m_createListBtn = new QPushButton(tr("Crear lista"), m_createBox);
    m_createListBtn->setObjectName("CreateListButton");
    m_createListBtn->setCursor(Qt::PointingHandCursor);
    createBoxLayout->addWidget(m_createListBtn);

    mainLayout->addWidget(m_createBox);

    // -------- "My lists" --------
    auto *myListsLabel = new QLabel(tr("My lists"), this);
    myListsLabel->setObjectName("SectionLabel");
    QFont mlFont = myListsLabel->font();
    mlFont.setPointSize(13);
    myListsLabel->setFont(mlFont);
    mainLayout->addWidget(myListsLabel);

    auto *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    m_myListsContainer = new QWidget();
    m_myListsLayout = new QVBoxLayout(m_myListsContainer);
    m_myListsLayout->setContentsMargins(0, 0, 0, 0);
    m_myListsLayout->setSpacing(12);
    m_myListsLayout->addStretch(1);
    scrollArea->setWidget(m_myListsContainer);

    mainLayout->addWidget(scrollArea, 1);

    // -------- Conexiones --------
    connect(addRow, &ItemAddRow::itemAdded, this, [this](const QString &text) {
        m_draftItems.append(text);
        rebuildDraftItems();
        updateCreateButtonState();
    });
    connect(m_listNameInput, &QLineEdit::textChanged, this, [this](const QString &) {
        updateCreateButtonState();
    });
    connect(m_listNameInput, &QLineEdit::returnPressed, this, &MainWindow::addNewList);
    connect(m_chevronBtn, &QToolButton::clicked, this, [this]() {
        bool visible = m_createBox->isVisible();
        m_createBox->setVisible(!visible);
        m_chevronBtn->setText(visible ? QString("\u25B2") : QString("\u25BC"));
    });
    connect(m_createListBtn, &QPushButton::clicked, this, &MainWindow::addNewList);
    connect(m_themeToggle, &ToggleSwitch::toggled, this, [this](bool checked) {
        m_darkMode = checked;
        applyTheme();
    });
    connect(m_controller, &TaskController::taskAdded, this, &MainWindow::rebuildMyLists);
    connect(m_controller, &TaskController::taskRemoved, this, &MainWindow::rebuildMyLists);

    updateCreateButtonState();
    applyTheme();
    rebuildMyLists();

    // Fuente monoespaciada para todo el árbol de widgets.
    QFont mono("JetBrains Mono");
    if (!QFontInfo(mono).exactMatch()) {
        mono = QFont("Cascadia Mono");
        if (!QFontInfo(mono).exactMatch()) {
            mono.setFamily("Monospace");
            mono.setStyleHint(QFont::Monospace);
        }
    }
    mono.setPointSize(10);
    setFont(mono);
}

void MainWindow::applyTheme() {
    QWidget *root = centralWidget();
    root->setStyleSheet(m_darkMode ? Styles::dark() : Styles::light());
    if (m_bell)
        m_bell->setPixmap(makeBellPixmap(m_darkMode ? QColor("#F2F2F2") : QColor("#111111")));
}

void MainWindow::rebuildDraftItems() {
    QLayoutItem *child;
    while ((child = m_draftItemsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
    for (int i = 0; i < m_draftItems.size(); ++i) {
        auto *row = new ItemRow(m_draftItems[i], false, m_draftItemsContainer);
        connect(row, &ItemRow::deleteRequested, this, [this, i]() {
            if (i < m_draftItems.size()) {
                m_draftItems.remove(i);
                rebuildDraftItems();
                updateCreateButtonState();
            }
        });
        m_draftItemsLayout->addWidget(row);
    }
}

void MainWindow::updateCreateButtonState() {
    bool ok = !m_listNameInput->text().trimmed().isEmpty() && !m_draftItems.isEmpty();
    m_createListBtn->setEnabled(ok);
}

void MainWindow::addNewList() {
    QString name = m_listNameInput->text().trimmed();
    if (name.isEmpty() || m_draftItems.isEmpty()) return;

    int idx = m_controller->taskCount();
    m_controller->addTask(name);
    for (const QString &text : m_draftItems)
        m_controller->addItemToTask(idx, text);

    m_listNameInput->clear();
    m_draftItems.clear();
    rebuildDraftItems();
    updateCreateButtonState();
}

void MainWindow::rebuildMyLists() {
    QLayoutItem *child;
    while ((child = m_myListsLayout->takeAt(0)) != nullptr) {
        if (child->widget()) child->widget()->deleteLater();
        delete child;
    }
    for (int i = 0; i < m_controller->taskCount(); ++i) {
        auto *card = new ListCard(m_controller, i, m_myListsContainer);
        m_myListsLayout->addWidget(card);
    }
    m_myListsLayout->addStretch(1);
}
