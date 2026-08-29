#include <QtTest>
#include <QTemporaryDir>
#include "persistence/JsonStorage.h"
#include "domain/Task.h"
#include "domain/Correction.h"
#include "domain/ChecklistItem.h"

class JsonStorageTests : public QObject
{
    Q_OBJECT

private slots:
    void testDateFormatting();
    void testItemToJson();
    void testItemFromJson();
    void testCorrectionToJson();
    void testCorrectionFromJson();
    void testTaskToJson();
    void testTaskFromJson();
    void testSaveAndLoad();
    void testLoadNonExistentFile();
    void testLoadCorruptedJson();
    void testLoadEmptyArray();
    void testLoadMissingFields();
};

void JsonStorageTests::testDateFormatting()
{
    QDate date(2024, 3, 15);
    QString str = JsonStorage::dateToString(date);
    QCOMPARE(str, QString("2024-03-15"));

    QDate parsed = JsonStorage::stringToDate(str);
    QCOMPARE(parsed, date);

    // Invalid date
    QDate invalid = JsonStorage::stringToDate("invalid");
    QVERIFY(invalid.isValid()); // Should fallback to current date
}

void JsonStorageTests::testItemToJson()
{
    ChecklistItem item("abc-123", "Test item", true);
    QJsonObject json = JsonStorage::itemToJson(&item);

    QCOMPARE(json["id"].toString(), QString("abc-123"));
    QCOMPARE(json["text"].toString(), QString("Test item"));
    QCOMPARE(json["done"].toBool(), true);
}

void JsonStorageTests::testItemFromJson()
{
    QJsonObject json;
    json["id"] = "test-id";
    json["text"] = "From JSON";
    json["done"] = true;

    ChecklistItem *item = JsonStorage::jsonToItem(json);
    QVERIFY(item != nullptr);
    QCOMPARE(item->id(), QString("test-id"));
    QCOMPARE(item->text(), QString("From JSON"));
    QCOMPARE(item->done(), true);
    delete item;

    // Missing ID should generate one
    QJsonObject jsonNoId;
    jsonNoId["text"] = "No ID";
    ChecklistItem *item2 = JsonStorage::jsonToItem(jsonNoId);
    QVERIFY(item2 != nullptr);
    QVERIFY(!item2->id().isEmpty());
    delete item2;
}

void JsonStorageTests::testCorrectionToJson()
{
    Correction corr("test-id", "Mi Corrección", QDate(2024, 1, 1));
    corr.addNewItem("Item 1");
    corr.addNewItem("Item 2");

    QJsonObject json = JsonStorage::correctionToJson(&corr);

    QCOMPARE(json["id"].toString(), QString("test-id"));
    QCOMPARE(json["name"].toString(), QString("Mi Corrección"));
    QCOMPARE(json["date"].toString(), QString("2024-01-01"));
    QCOMPARE(json["items"].toArray().size(), 2);
}

void JsonStorageTests::testCorrectionFromJson()
{
    QJsonObject json;
    json["id"] = "corr-id";
    json["name"] = "Test Corr";
    json["date"] = "2024-05-20";

    QJsonArray items;
    QJsonObject item1;
    item1["text"] = "Item A";
    item1["done"] = false;
    items.append(item1);
    json["items"] = items;

    Correction *corr = JsonStorage::jsonToCorrection(json);
    QVERIFY(corr != nullptr);
    QCOMPARE(corr->id(), QString("corr-id"));
    QCOMPARE(corr->name(), QString("Test Corr"));
    QCOMPARE(corr->itemCount(), 1);
    QCOMPARE(corr->getItem(0)->text(), QString("Item A"));
    delete corr;
}

void JsonStorageTests::testTaskToJson()
{
    Task task("task-id", "Mi Tarea", QDate(2024, 3, 10));
    task.addNewItem("Check 1");
    task.addNewItem("Check 2");
    task.addNewCorrection("Corrección 1");

    QJsonObject json = JsonStorage::taskToJson(&task);

    QCOMPARE(json["id"].toString(), QString("task-id"));
    QCOMPARE(json["name"].toString(), QString("Mi Tarea"));
    QCOMPARE(json["date"].toString(), QString("2024-03-10"));
    QCOMPARE(json["items"].toArray().size(), 2);
    QCOMPARE(json["corrections"].toArray().size(), 1);
}

void JsonStorageTests::testTaskFromJson()
{
    QJsonObject json;
    json["id"] = "task-123";
    json["name"] = "From JSON";
    json["date"] = "2024-07-01";

    QJsonArray items;
    QJsonObject item;
    item["text"] = "Check";
    item["done"] = true;
    items.append(item);
    json["items"] = items;

    QJsonArray corrections;
    QJsonObject corr;
    corr["name"] = "Corr";
    corr["date"] = "2024-07-02";
    corrections.append(corr);
    json["corrections"] = corrections;

    Task *task = JsonStorage::jsonToTask(json);
    QVERIFY(task != nullptr);
    QCOMPARE(task->id(), QString("task-123"));
    QCOMPARE(task->name(), QString("From JSON"));
    QCOMPARE(task->itemCount(), 1);
    QCOMPARE(task->correctionCount(), 1);
    QCOMPARE(task->getItem(0)->text(), QString("Check"));
    QCOMPARE(task->getCorrection(0)->name(), QString("Corr"));
    delete task;
}

void JsonStorageTests::testSaveAndLoad()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString filePath = dir.filePath("tasks.json");

    JsonStorage storage(filePath);

    // Create tasks
    QList<Task*> tasks;
    Task *t1 = new Task("Tarea 1", QDate(2024, 1, 1));
    t1->addNewItem("Item 1");
    t1->addNewItem("Item 2");
    tasks.append(t1);

    Task *t2 = new Task("Tarea 2", QDate(2024, 2, 2));
    t2->addNewCorrection("Corrección A");
    tasks.append(t2);

    // Save
    bool saved = storage.save(tasks);
    QVERIFY(saved);

    // Load
    QList<Task*> loaded = storage.load();
    QCOMPARE(loaded.size(), 2);

    // Verify first task
    QCOMPARE(loaded[0]->name(), QString("Tarea 1"));
    QCOMPARE(loaded[0]->date(), QString("2024-01-01"));
    QCOMPARE(loaded[0]->itemCount(), 2);
    QCOMPARE(loaded[0]->getItem(0)->text(), QString("Item 1"));
    QCOMPARE(loaded[0]->getItem(1)->text(), QString("Item 2"));

    // Verify second task
    QCOMPARE(loaded[1]->name(), QString("Tarea 2"));
    QCOMPARE(loaded[1]->correctionCount(), 1);
    QCOMPARE(loaded[1]->getCorrection(0)->name(), QString("Corrección A"));

    // Cleanup
    qDeleteAll(tasks);
    qDeleteAll(loaded);
}

void JsonStorageTests::testLoadNonExistentFile()
{
    JsonStorage storage("/nonexistent/path/tasks.json");
    QList<Task*> tasks = storage.load();
    QCOMPARE(tasks.size(), 0);
}

void JsonStorageTests::testLoadCorruptedJson()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString filePath = dir.filePath("corrupted.json");

    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write("{ this is not valid json }}}");
    file.close();

    JsonStorage storage(filePath);
    QList<Task*> tasks = storage.load();
    QCOMPARE(tasks.size(), 0);
}

void JsonStorageTests::testLoadEmptyArray()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString filePath = dir.filePath("empty.json");

    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write("[]");
    file.close();

    JsonStorage storage(filePath);
    QList<Task*> tasks = storage.load();
    QCOMPARE(tasks.size(), 0);
}

void JsonStorageTests::testLoadMissingFields()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QString filePath = dir.filePath("minimal.json");

    // Task with no items, no corrections, no date
    QFile file(filePath);
    file.open(QIODevice::WriteOnly);
    file.write(R"([{"name": "Minimal Task"}])");
    file.close();

    JsonStorage storage(filePath);
    QList<Task*> tasks = storage.load();
    QCOMPARE(tasks.size(), 1);
    QCOMPARE(tasks[0]->name(), QString("Minimal Task"));
    QCOMPARE(tasks[0]->itemCount(), 0);
    QCOMPARE(tasks[0]->correctionCount(), 0);
    qDeleteAll(tasks);
}

QTEST_MAIN(JsonStorageTests)
#include "JsonStorageTests.moc"
