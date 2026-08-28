#include <QtTest>
#include <QSignalSpy>
#include "domain/Task.h"
#include "domain/Correction.h"
#include "domain/ChecklistItem.h"

class TaskTests : public QObject
{
    Q_OBJECT

private slots:
    void testDefaults();
    void testNameProperty();
    void testDateProperty();
    void testAddItem();
    void testRemoveItem();
    void testAddCorrection();
    void testRemoveCorrection();
    void testSignals();
};

void TaskTests::testDefaults()
{
    Task task;
    QVERIFY(!task.id().isEmpty());
    QCOMPARE(task.name(), QString("Sin nombre"));
    QVERIFY(!task.date().isEmpty());
    QCOMPARE(task.itemCount(), 0);
    QCOMPARE(task.correctionCount(), 0);
}

void TaskTests::testNameProperty()
{
    Task task("Mi Tarea");
    QCOMPARE(task.name(), QString("Mi Tarea"));

    task.setName("Actualizada");
    QCOMPARE(task.name(), QString("Actualizada"));
}

void TaskTests::testDateProperty()
{
    QDate today(2024, 6, 20);
    Task task("Test", today);
    QCOMPARE(task.date(), QString("2024-06-20"));
}

void TaskTests::testAddItem()
{
    Task task;
    task.addNewItem("Item 1");
    task.addNewItem("Item 2");

    QCOMPARE(task.itemCount(), 2);
    QCOMPARE(task.getItem(0)->text(), QString("Item 1"));
    QCOMPARE(task.getItem(1)->text(), QString("Item 2"));
}

void TaskTests::testRemoveItem()
{
    Task task;
    task.addNewItem("Item 1");
    task.addNewItem("Item 2");

    task.removeItem(0);
    QCOMPARE(task.itemCount(), 1);
    QCOMPARE(task.getItem(0)->text(), QString("Item 2"));
}

void TaskTests::testAddCorrection()
{
    Task task;
    Correction *corr = task.addNewCorrection("Corrección 1");

    QCOMPARE(task.correctionCount(), 1);
    QVERIFY(corr != nullptr);
    QCOMPARE(corr->name(), QString("Corrección 1"));
}

void TaskTests::testRemoveCorrection()
{
    Task task;
    task.addNewCorrection("Corr 1");
    task.addNewCorrection("Corr 2");

    task.removeCorrection(0);
    QCOMPARE(task.correctionCount(), 1);
    QCOMPARE(task.getCorrection(0)->name(), QString("Corr 2"));
}

void TaskTests::testSignals()
{
    Task task;
    QSignalSpy nameSpy(&task, &Task::nameChanged);
    QSignalSpy itemCountSpy(&task, &Task::itemCountChanged);
    QSignalSpy corrCountSpy(&task, &Task::correctionCountChanged);

    task.setName("New name");
    QCOMPARE(nameSpy.count(), 1);

    task.addNewItem("Item");
    QCOMPARE(itemCountSpy.count(), 1);

    task.addNewCorrection("Corr");
    QCOMPARE(corrCountSpy.count(), 1);
}

QTEST_MAIN(TaskTests)
#include "TaskTests.moc"
