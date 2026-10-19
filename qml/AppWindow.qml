import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    property var controller
    property bool darkMode: false

    readonly property var theme: darkMode ? theme.darkColors : theme.lightColors

    property string searchText: ""
    property int groupModeIndex: 0
    property int dateFilterIndex: 0

    property bool isCreateExpanded: true
    property string draftName: ""
    property string draftCategory: ""
    property string draftItemText: ""
    property string selectedDraftTheme: ""

    property bool isNotificationsOpen: false

    property var groups: []
    property var draftItems: []
    property var alerts: []
    property var draftThemeOptions: []

    Component.onCompleted: {
        controller.tasksChanged.connect(refreshLists);
        controller.load();
        refreshLists();

        var timer = new QTimer();
        timer.interval = 60000;
        timer.timeout.connect(rebuildAlerts);
        timer.start();
        rebuildAlerts();
    }

    ColumnLayout {
        anchors.fill: true
        spacing: 0

        RowLayout {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            spacing: theme.spacing.small
            padding: theme.spacing.medium

            Button {
                text: isCreateExpanded ? "▼" : "▲"
                flat: true
                onClicked: isCreateExpanded = !isCreateExpanded
                Layout.preferredWidth: 30
            }

            TextField {
                placeholderText: qsTr("Search tasks...")
                text: root.searchText
                onTextChanged: root.searchText = text
                Layout.fillWidth: true
            }

            ComboBox {
                model: ["By theme", "By date"]
                currentIndex: root.groupModeIndex
                onCurrentIndexChanged: root.groupModeIndex = currentIndex
                Layout.preferredWidth: 120
            }

            ComboBox {
                model: ["All dates", "Today", "This week", "This month", "Custom range"]
                currentIndex: root.dateFilterIndex
                onCurrentIndexChanged: root.dateFilterIndex = currentIndex
                Layout.preferredWidth: 140
            }

            Button {
                text: qsTr("☀")
                flat: true
                onClicked: {
                    root.darkMode = !root.darkMode
                    stackView.currentItem.darkMode = root.darkMode
                }
                Layout.preferredWidth: 30
                tooltip: qsTr("Toggle dark mode")
            }

            Button {
                text: root.controller.presentationMode ? qsTr("⏹") : qsTr("🖥")
                flat: true
                onClicked: root.controller.setPresentationMode(!root.controller.presentationMode)
                Layout.preferredWidth: 30
                tooltip: root.controller.presentationMode ? qsTr("Salir modo presentación") : qsTr("Activar modo presentación")
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            ColumnLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: theme.spacing.small

                RowLayout {
                    Layout.fillWidth: true
                    spacing: theme.spacing.small

                    Text {
                        text: qsTr("Alerts: %1").arg(alerts.length)
                        font.bold: alerts.length > 0
                        color: alerts.length > 0 ? theme.colors.error : theme.colors.textSecondary
                    }

                    Button {
                        text: isNotificationsOpen ? "▼" : "▲"
                        flat: true
                        onClicked: isNotificationsOpen = !isNotificationsOpen
                        Layout.preferredWidth: 30
                    }
                }

                ColumnLayout {
                    visible: isNotificationsOpen
                    Layout.fillWidth: true
                    spacing: theme.spacing.small

                    Repeater {
                        model: alerts
                        delegate: AlertVm {
                            alert: modelData
                            onDismiss: {
                                var idx = alerts.indexOf(modelData);
                                if (idx >= 0) alerts.splice(idx, 1);
                            }
                        }
                    }
                }

                ScrollView {
                    Layout.fillWidth: true
                    Layout.fillHeight: true

                    ColumnLayout {
                        spacing: theme.spacing.medium

                        Repeater {
                            model: groups
                            delegate: GroupVm {
                                group: modelData
                                controller: root.controller
                                onTaskSelected: {
                                }
                            }
                        }

                        RowLayout {
                            visible: !hasLists
                            Layout.alignment: Qt.AlignCenter
                            spacing: theme.spacing.small

                            Text {
                                text: qsTr("No tasks yet. Create your first list!")
                                color: theme.colors.textSecondary
                                font.italic: true
                            }
                        }
                    }
                }
            }

            ColumnLayout {
                visible: isCreateExpanded
                Layout.fillWidth: true
                Layout.preferredWidth: 400
                spacing: theme.spacing.medium
                padding: theme.spacing.medium

                GroupBox {
                    title: qsTr("Create New List")
                    Layout.fillWidth: true

                    ColumnLayout {
                        spacing: theme.spacing.small
                        padding: theme.spacing.small

                        TextField {
                            placeholderText: qsTr("List name")
                            text: root.draftName
                            onTextChanged: root.draftName = text
                            Layout.fillWidth: true
                        }

                        RowLayout {
                            spacing: theme.spacing.small
                            Layout.fillWidth: true

                            ComboBox {
                                model: draftThemeOptions
                                editText: root.draftCategory
                                onEditTextChanged: root.draftCategory = editText
                                Layout.fillWidth: true
                            }

                            Button {
                                text: qsTr("+")
                                flat: true
                                onClicked: {
                                    if (selectedDraftTheme.length > 0) {
                                        draftCategory = selectedDraftTheme;
                                        selectedDraftTheme = "";
                                    }
                                }
                                Layout.preferredWidth: 30
                            }
                        }

                        TextField {
                            placeholderText: qsTr("Add item...")
                            text: root.draftItemText
                            onTextChanged: root.draftItemText = text
                            onAccepted: addDraftItem()
                            Layout.fillWidth: true
                        }

                        RowLayout {
                            spacing: theme.spacing.small
                            Layout.fillWidth: true

                            Button {
                                text: qsTr("Add Item")
                                enabled: draftItemText.trimmed().length > 0
                                onClicked: addDraftItem()
                                Layout.fillWidth: true
                            }
                        }

                        ColumnLayout {
                            spacing: theme.spacing.small

                            Repeater {
                                model: draftItems
                                delegate: AddItemRow {
                                    item: modelData
                                    onRemove: {
                                        var idx = draftItems.indexOf(modelData);
                                        if (idx >= 0) DraftItems.splice(idx, 1);
                                    }
                                }
                            }

                            RowLayout {
                                visible: draftItems.length === 0
                                Layout.alignment: Qt.AlignCenter

                                Text {
                                    text: qsTr("No items yet")
                                    color: theme.colors.textSecondary
                                    font.italic: true
                                }
                            }
                        }

                        Button {
                            text: qsTr("Create List")
                            enabled: canCreate
                            onClicked: createList()
                            Layout.fillWidth: true
                        }
                    }
                }
            }
        }
    }

    function addDraftItem() {
        var text = draftItemText.trimmed();
        if (text.length === 0) return;

        draftItems.push({ text: text, done: false, id: "draft_" + Date.now() });
        draftItemText = "";

        if (draftCategory.trimmed().length === 0) {
            suggestCategoryFromItems();
        }
    }

    function suggestCategoryFromItems() {
        if (draftCategory.trimmed().length > 0) return;

        var itemWords = [];
        for (var i = 0; i < draftItems.length; i++) {
            var words = draftItems[i].text.split(/\s+/);
            for (var j = 0; j < words.length; j++) {
                if (words[j].length >= 2) itemWords.push(words[j].toLowerCase());
            }
        }

        if (itemWords.length === 0) return;

        var categories = controller.tasks().map(function(t) { return t.category; });
        var uniqueCategories = [];
        for (var i = 0; i < categories.length; i++) {
            if (categories[i].length > 0 && uniqueCategories.indexOf(categories[i]) < 0) {
                uniqueCategories.push(categories[i]);
            }
        }

        for (var i = 0; i < uniqueCategories.length; i++) {
            var category = uniqueCategories[i];
            var categoryWords = category.split(/\s+/);
            for (var j = 0; j < categoryWords.length; j++) {
                for (var k = 0; k < itemWords.length; k++) {
                    if (itemWords[k] === categoryWords[j].toLowerCase() ||
                        (categoryWords[j].length >= 4 && itemWords[k].length >= 4 &&
                         (itemWords[k].startsWith(categoryWords[j].toLowerCase()) ||
                          categoryWords[j].toLowerCase().startsWith(itemWords[k])))) {
                        draftCategory = category;
                        return;
                    }
                }
            }
        }
    }

    function createList() {
        if (!canCreate) return;

        var task = controller.addTask(draftName.trimmed(), draftCategory.trimmed());
        if (task) {
            for (var i = 0; i < draftItems.length; i++) {
                controller.addItemToTask(controller.taskCount - 1, draftItems[i].text);
            }
        }

        draftName = "";
        draftItemText = "";
        draftItems = [];
        refreshLists();
    }

    function refreshLists() {
        var query = searchText.trimmed();
        var today = new Date();
        today.setHours(0, 0, 0, 0);

        var vms = [];
        for (var i = controller.taskCount - 1; i >= 0; i--) {
            var task = controller.getTask(i);
            if (!task) continue;

            if (query.length > 0 && !task.name.toLowerCase().includes(query.toLowerCase())) continue;

            var taskDate = QDate.fromString(task.date, "yyyy-MM-dd");
            if (!passesDateFilter(taskDate, today, dateFilterIndex)) continue;

            vms.push(task);
        }

        groups = [];
        if (groupModeIndex === 0) {
            addThemeGroups(vms);
        } else {
            addDateGroups(vms, today);
        }

        refreshThemeOptions();
        rebuildAlerts();
    }

    function passesDateFilter(date, today, index) {
        switch (index) {
            case 1: return date.equals(today);
            case 2: {
                var weekStart = new Date(today);
                weekStart.setDate(today.getDate() - today.getDay());
                return date >= weekStart;
            }
            case 3: return date.getFullYear() === today.getFullYear() && date.getMonth() === today.getMonth();
            default: return true;
        }
    }

    function addThemeGroups(vms) {
        var noTopic = [];
        var categories = [];

        for (var i = 0; i < vms.length; i++) {
            if (vms[i].category.length === 0) {
                noTopic.push(vms[i]);
            } else if (categories.indexOf(vms[i].category) < 0) {
                categories.push(vms[i].category);
            }
        }

        categories.sort();

        for (var i = 0; i < categories.length; i++) {
            var members = vms.filter(function(v) { return v.category === categories[i]; });
            groups.push({ label: categories[i], tasks: members });
        }

        if (noTopic.length > 0) {
            groups.push({ label: qsTr("No theme"), tasks: noTopic });
        }
    }

    function addDateGroups(vms, today) {
        var labels = ["Today", "Yesterday", "This week", "This month", "Earlier"];

        for (var i = 0; i < labels.length; i++) {
            var members = vms.filter(function(v) {
                var taskDate = QDate.fromString(v.date, "yyyy-MM-dd");
                return dateGroupLabel(taskDate, today) === labels[i];
            });

            if (members.length > 0) {
                groups.push({ label: labels[i], tasks: members });
            }
        }
    }

    function dateGroupLabel(date, today) {
        if (date.equals(today)) return "Today";

        var yesterday = new Date(today);
        yesterday.setDate(today.getDate() - 1);
        if (date.equals(yesterday)) return "Yesterday";

        var weekStart = new Date(today);
        weekStart.setDate(today.getDate() - today.getDay());
        if (date >= weekStart) return "This week";

        if (date.getFullYear() === today.getFullYear() && date.getMonth() === today.getMonth()) {
            return "This month";
        }

        return "Earlier";
    }

    function refreshThemeOptions() {
        var categories = [];
        for (var i = 0; i < controller.taskCount; i++) {
            var cat = controller.getTask(i).category;
            if (cat.length > 0 && categories.indexOf(cat) < 0) {
                categories.push(cat);
            }
        }
        categories.sort();
        draftThemeOptions = categories;
    }

    function rebuildAlerts() {
        var today = new Date();
        today.setHours(0, 0, 0, 0);
        var thresholdDays = 7;

        var pending = [];
        for (var i = 0; i < controller.taskCount; i++) {
            var task = controller.getTask(i);
            if (!task || task.isFinished) continue;

            var taskDate = QDate.fromString(task.date, "yyyy-MM-dd");
            var days = Math.floor((today - taskDate) / (1000 * 60 * 60 * 24));

            if (days < thresholdDays) continue;
            pending.push({ task: task, days: days });
        }

        pending.sort(function(a, b) { return b.days - a.days; });

        alerts = pending.map(function(p) {
            return {
                taskId: p.task.id,
                taskName: p.task.name,
                category: p.task.category,
                message1: "Created on " + p.task.date,
                message2: "Pending for " + p.days + " days"
            };
        });
    }

    function undo() {
        controller.undo();
    }

    function redo() {
        controller.redo();
    }

    function save() {
        controller.save();
    }

    function toggleCreateExpanded() {
        isCreateExpanded = !isCreateExpanded;
    }

    readonly property bool canCreate: draftName.trimmed().length > 0 && draftItems.length > 0
    readonly property bool hasLists: groups.length > 0 && groups.some(function(g) { return g.tasks.length > 0; })
}