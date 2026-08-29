#include <QtTest>
#include <QSignalSpy>
#include <QTemporaryDir>
#include "controllers/TaskController.h"
#include "domain/Task.h"
#include "domain/Correction.h"
#include "domain/ChecklistItem.h"

class TaskControllerTests : public QObject
{
    Q_OBJECT

private slots:
    void testDefaults();
    void testAddTask();
    void testAddTaskEmptyName();
    void testRemoveTask();
    void testGetTaskInvalidIndex();
    void testAddItemToTask();
    void testRemoveItemFromTask();
    void testToggleItemInTask();
    void testAddCorrectionToTask();
    void testRemoveCorrectionFromTask();
    void testAddItemToCorrection();
    void testRemoveItemFromCorrection();
    void testToggleItemInCorrection();
    void testSetTaskName();
    void testSetCorrectionName();
    void testSaveAndLoad();
    void testLoadNonExistentFile();
    void testImportFromJsonString();
    void testImportInvalidJson();
    void testAutoSave();
    void testSignals();
};

void TaskControllerTests::testDefaults()
{
    TaskController controller;
    QCOMPARE(controller.taskCount(), 0);
    QVERIFY(!controller.isLoaded());
    QVERIFY(controller.lastError().isEmpty());
    QVERIFY(controller.autoSave());
    QCOMPARE(controller.autoSaveDelay(), 500);
}

void TaskControllerTests::testAddTask()
{
    TaskController controller;
    Task *task = controller.addTask("Mi Tarea");

    QVERIFY(task != nullptr);
    QCOMPARE(controller.taskCount(), 1);
    QCOMPARE(task->name(), QString("Mi Tarea"));
}

void TaskControllerTests::testAddTaskEmptyName()
{
    TaskController controller;
    Task *task = controller.addTask("");

    QVERIFY(task == nullptr);
    QCOMPARE(controller.taskCount(), 0);
    QVERIFY(!controller.lastError().isEmpty());
}

void TaskControllerTests::testRemoveTask()
{
    TaskController controller;
    controller.addTask("Tarea 1");
    controller.addTask("Tarea 2");

    controller.removeTask(0);
    QCOMPARE(controller.taskCount(), 1);
    QCOMPARE(controller.getTask(0)->name(), QString("Tarea 2"));
}

void TaskControllerTests::testGetTaskInvalidIndex()
{
    TaskController controller;
    QVERIFY(controller.getTask(-1) == nullptr);
    QVERIFY(controller.getTask(0) == nullptr);
    QVERIFY(controller.getTask(100) == nullptr);
}

void TaskControllerTests::testAddItemToTask()
{
    TaskController controller;
    controller.addTask("Tarea");

    controller.addItemToTask(0, "Item 1");
    controller.addItemToTask(0, "Item 2");

    Task *task = controller.getTask(0);
    QCOMPARE(task->itemCount(), 2);
    QCOMPARE(task->getItem(0)->text(), QString("Item 1"));
}

void TaskControllerTests::testRemoveItemFromTask()
{
    TaskController controller;
    controller.addTask("Tarea");
    controller.addItemToTask(0, "Item 1");
    controller.addItemToTask(0, "Item 2");

    controller.removeItemFromTask(0, 0);
    QCOMPARE(controller.getTask(0)->itemCount(), 1);
    QCOMPARE(controller.getTask(0)->getItem(0)->text(), QString("Item 2"));
}

void TaskControllerTests::testToggleItemInTask()
{
    TaskController controller;
    controller.addTask("Tarea");
    controller.addItemToTask(0, "Item 1");

    controller.toggleItemInTask(0, 0, true);
    QVERIFY(controller.getTask(0)->getItem(0)->done());

    controller.toggleItemInTask(0, 0, false);
    QVERIFY(!controller.getTask(0)->getItem(0)->done());
}

void TaskControllerTests::testAddCorrectionToTask()
{
    TaskController controller;
    controller.addTask("Tarea");

    Correction *corr = controller.addCorrectionToTask(0, "Corrección 1");
    QVERIFY(corr != nullptr);
    QCOMPARE(controller.getTask(0)->correctionCount(), 1);
    QCOMPARE(corr->name(), QString("Corrección 1"));
}

void TaskControllerTests::testRemoveCorrectionFromTask()
{
    TaskController controller;
    controller.addTask("Tarea");
    controller.addCorrectionToTask(0, "Corr 1");
    controller.addCorrectionToTask(0, "Corr 2");

    controller.removeCorrectionFromTask(0, 0);
    QCOMPARE(controller.getTask(0)->correctionCount(), 1);
    QCOMPARE(controller.getTask(0)->getCorrection(0)->name(), QString("Corr 2"));
}

void TaskControllerTests::testAddItemToCorrection()
{
    TaskController controller;
    controller.addTask("Tarea");
    controller.addCorrectionToTask(0, "Corr 1");

    controller.addItemToCorrection(0, 0, "Item 1");
    QCOMPARE(controller.getTask(0)->getCorrection(0)->itemCount(), 1);
    QCOMPARE(controller.getTask(0)->getCorrection(0)->getItem(0)->text(), QString("Item 1"));
}

void TaskControllerTests::testRemoveItemFromCorrection()
{
    TaskController controller;
    controller.addTask("Tarea");
    controller.addCorrectionToTask(0, "Corr 1");
    controller.addItemToCorrection(0, 0, "Item 1");
    controller.addItemToCorrection(0, 0, "Item 2");

    controller.removeItemFromCorrection(0, 0, 0);
    QCOMPARE(controller.getTask(0)->getCorrection(0)->itemCount(), 1);
    QCOMPARE(controller.getTask(0)->getCorrection(0)->getItem(0)->text(), QString("Item 2"));
}

void TaskControllerTests::testToggleItemInCorrection()
{
    TaskController controller;
    controller.addTask("Tarea");
    controller.addCorrectionToTask(0, "Corr 1");
    controller.addItemToCorrection(0, 0, "Item 1");

    controller.toggleItemInCorrection(0, 0, 0, true);
    QVERIFY(controller.getTask(0)->getCorrection(0)->getItem(0)->done());

    controller.toggleItemInCorrection(0, 0, 0, false);
    QVERIFY(!controller.getTask(0)->getCorrection(0)->getItem(0)->done());
}

void TaskControllerTests::testSetTaskName()
{
    TaskController controller;
    controller.addTask("Original");

    controller.setTaskName(0, "Actualizada");
    QCOMPARE(controller.getTask(0)->name(), QString("Actualizada"));
}

void TaskControllerTests::testSetCorrectionName()
{
    TaskController controller;
    controller.addTask("Tarea");
    controller.addCorrectionToTask(0, "Original");

    controller.setCorrectionName(0, 0, "Actualizada");
    QCOMPARE(controller.getTask(0)->getCorrection(0)->name(), QString("Actualizada"));
}

void TaskControllerTests::testSaveAndLoad()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString filePath = dir.filePath("tasks.json");

    // Save
    {
        TaskController controller(filePath);
        controller.setAutoSave(false);
        controller.addTask("Tarea 1");
        controller.addItemToTask(0, "Item 1");
        controller.addCorrectionToTask(0, "Corr 1");
        controller.addItemToCorrection(0, 0, "Corr Item 1");
        controller.save();
    }

    // Load
    {
        TaskController controller(filePath);
        controller.load();
        QVERIFY(controller.isLoaded());
        QCOMPARE(controller.taskCount(), 1);

        Task *task = controller.getTask(0);
        QCOMPARE(task->name(), QString("Tarea 1"));
        QCOMPARE(task->itemCount(), 1);
        QCOMPARE(task->getItem(0)->text(), QString("Item 1"));
        QCOMPARE(task->correctionCount(), 1);
        QCOMPARE(task->getCorrection(0)->name(), QString("Corr 1"));
        QCOMPARE(task->getCorrection(0)->itemCount(), 1);
        QCOMPARE(task->getCorrection(0)->getItem(0)->text(), QString("Corr Item 1"));
    }
}

void TaskControllerTests::testLoadNonExistentFile()
{
    TaskController controller("/nonexistent/path/tasks.json");
    controller.load();
    QVERIFY(controller.isLoaded());
    QCOMPARE(controller.taskCount(), 0);
}

void TaskControllerTests::testImportFromJsonString()
{
    TaskController controller;
    controller.setAutoSave(false);

    QString json = R"([
        {
            "id": "test-1",
            "name": "Imported Task",
            "date": "2024-01-15",
            "items": [
                {"id": "i1", "text": "Imported Item", "done": false}
            ],
            "corrections": []
        }
    ])";

    controller.importFromJsonString(json);
    QCOMPARE(controller.taskCount(), 1);
    QCOMPARE(controller.getTask(0)->name(), QString("Imported Task"));
    QCOMPARE(controller.getTask(0)->itemCount(), 1);
}

void TaskControllerTests::testImportInvalidJson()
{
    TaskController controller;
    controller.importFromJsonString("not valid json");
    QCOMPARE(controller.taskCount(), 0);
    QVERIFY(!controller.lastError().isEmpty());
}

void TaskControllerTests::testAutoSave()
{
    TaskController controller;
    QVERIFY(controller.autoSave());

    controller.setAutoSave(false);
    QVERIFY(!controller.autoSave());

    controller.setAutoSaveDelay(1000);
    QCOMPARE(controller.autoSaveDelay(), 1000);
}

void TaskControllerTests::testSignals()
{
    TaskController controller;
    QSignalSpy tasksSpy(&controller, &TaskController::tasksChanged);
    QSignalSpy addedSpy(&controller, &TaskController::taskAdded);
    QSignalSpy removedSpy(&controller, &TaskController::taskRemoved);

    controller.addTask("Tarea");
    QCOMPARE(tasksSpy.count(), 1);
    QCOMPARE(addedSpy.count(), 1);

    controller.removeTask(0);
    QCOMPARE(tasksSpy.count(), 2);
    QCOMPARE(removedSpy.count(), 1);
}

QTEST_MAIN(TaskControllerTests)
#include "TaskControllerTests.moc"
