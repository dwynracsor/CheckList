#include <QtTest>
#include <QSignalSpy>
#include "domain/Correction.h"
#include "domain/ChecklistItem.h"

class CorrectionTests : public QObject
{
    Q_OBJECT

private slots:
    void testDefaults();
    void testNameProperty();
    void testDateProperty();
    void testAddItem();
    void testRemoveItem();
    void testGetItem();
    void testItemCount();
    void testAddNewItem();
    void testSignals();
};

void CorrectionTests::testDefaults()
{
    Correction corr;
    QVERIFY(!corr.id().isEmpty());
    QCOMPARE(corr.name(), QString("Sin nombre"));
    QVERIFY(!corr.date().isEmpty());
    QCOMPARE(corr.itemCount(), 0);
}

void CorrectionTests::testNameProperty()
{
    Correction corr("Mi Corrección");
    QCOMPARE(corr.name(), QString("Mi Corrección"));

    corr.setName("Actualizada");
    QCOMPARE(corr.name(), QString("Actualizada"));
}

void CorrectionTests::testDateProperty()
{
    QDate today(2024, 1, 15);
    Correction corr("Test", today);
    QCOMPARE(corr.date(), QString("2024-01-15"));
}

void CorrectionTests::testAddItem()
{
    Correction corr;
    ChecklistItem *item = new ChecklistItem("Item 1");
    corr.addItem(item);

    QCOMPARE(corr.itemCount(), 1);
    QCOMPARE(corr.getItem(0)->text(), QString("Item 1"));
}

void CorrectionTests::testRemoveItem()
{
    Correction corr;
    corr.addNewItem("Item 1");
    corr.addNewItem("Item 2");
    QCOMPARE(corr.itemCount(), 2);

    corr.removeItem(0);
    QCOMPARE(corr.itemCount(), 1);
    QCOMPARE(corr.getItem(0)->text(), QString("Item 2"));
}

void CorrectionTests::testGetItem()
{
    Correction corr;
    corr.addNewItem("Item 1");

    ChecklistItem *item = corr.getItem(0);
    QVERIFY(item != nullptr);
    QCOMPARE(item->text(), QString("Item 1"));

    // Out of bounds
    QVERIFY(corr.getItem(-1) == nullptr);
    QVERIFY(corr.getItem(1) == nullptr);
}

void CorrectionTests::testItemCount()
{
    Correction corr;
    QCOMPARE(corr.itemCount(), 0);

    corr.addNewItem("A");
    QCOMPARE(corr.itemCount(), 1);

    corr.addNewItem("B");
    QCOMPARE(corr.itemCount(), 2);
}

void CorrectionTests::testAddNewItem()
{
    Correction corr;
    corr.addNewItem("New item");

    QCOMPARE(corr.itemCount(), 1);
    QCOMPARE(corr.getItem(0)->text(), QString("New item"));
    QCOMPARE(corr.getItem(0)->done(), false);
}

void CorrectionTests::testSignals()
{
    Correction corr;
    QSignalSpy nameSpy(&corr, &Correction::nameChanged);
    QSignalSpy countSpy(&corr, &Correction::itemCountChanged);

    corr.setName("New name");
    QCOMPARE(nameSpy.count(), 1);

    corr.addNewItem("Item");
    QCOMPARE(countSpy.count(), 1);
}

QTEST_MAIN(CorrectionTests)
#include "CorrectionTests.moc"
