#include <QtTest>
#include <QSignalSpy>
#include "domain/ChecklistItem.h"

class ChecklistItemTests : public QObject
{
    Q_OBJECT

private slots:
    void testDefaults();
    void testTextProperty();
    void testDoneProperty();
    void testToggle();
    void testSignalTextChanged();
    void testSignalDoneChanged();
};

void ChecklistItemTests::testDefaults()
{
    ChecklistItem item;
    QVERIFY(!item.id().isEmpty());
    QCOMPARE(item.text(), QString(""));
    QCOMPARE(item.done(), false);
}

void ChecklistItemTests::testTextProperty()
{
    ChecklistItem item("Test item");
    QCOMPARE(item.text(), QString("Test item"));

    item.setText("Updated");
    QCOMPARE(item.text(), QString("Updated"));
}

void ChecklistItemTests::testDoneProperty()
{
    ChecklistItem item("Test");
    QCOMPARE(item.done(), false);

    item.setDone(true);
    QCOMPARE(item.done(), true);
}

void ChecklistItemTests::testToggle()
{
    ChecklistItem item("Test");
    QCOMPARE(item.done(), false);

    item.toggle();
    QCOMPARE(item.done(), true);

    item.toggle();
    QCOMPARE(item.done(), false);
}

void ChecklistItemTests::testSignalTextChanged()
{
    ChecklistItem item("Original");
    QSignalSpy spy(&item, &ChecklistItem::textChanged);

    item.setText("Changed");
    QCOMPARE(spy.count(), 1);

    item.setText("Changed");
    QCOMPARE(spy.count(), 1); // No signal for same value
}

void ChecklistItemTests::testSignalDoneChanged()
{
    ChecklistItem item;
    QSignalSpy spy(&item, &ChecklistItem::doneChanged);

    item.setDone(true);
    QCOMPARE(spy.count(), 1);

    item.setDone(true);
    QCOMPARE(spy.count(), 1); // No signal for same value
}

QTEST_MAIN(ChecklistItemTests)
#include "ChecklistItemTests.moc"
