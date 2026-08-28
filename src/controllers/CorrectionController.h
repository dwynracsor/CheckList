#ifndef CORRECTIONCONTROLLER_H
#define CORRECTIONCONTROLLER_H

#include <QObject>
#include <QQmlListProperty>

class Correction;
class ChecklistItem;

class CorrectionController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ChecklistItem> items READ items NOTIFY itemsChanged)
    Q_PROPERTY(int itemCount READ itemCount NOTIFY itemsChanged)
    Q_PROPERTY(QString correctionName READ correctionName WRITE setCorrectionName NOTIFY nameChanged)

public:
    explicit CorrectionController(QObject *parent = nullptr);
    ~CorrectionController() override;

    QQmlListProperty<ChecklistItem> items();
    int itemCount() const;
    QString correctionName() const;
    void setCorrectionName(const QString &name);

    // Bind to a correction (call when selection changes)
    Q_INVOKABLE void bindTo(Correction *correction);

    // Item operations
    Q_INVOKABLE void addItem(const QString &text);
    Q_INVOKABLE void removeItem(int index);
    Q_INVOKABLE void toggleItem(int index, bool done);
    Q_INVOKABLE ChecklistItem* getItem(int index) const;

signals:
    void itemsChanged();
    void nameChanged();
    void bound();

private:
    static void itemsAppend(QQmlListProperty<ChecklistItem> *list, ChecklistItem *item);
    static qsizetype itemsCount(QQmlListProperty<ChecklistItem> *list);
    static ChecklistItem* itemsAt(QQmlListProperty<ChecklistItem> *list, qsizetype index);
    static void itemsClear(QQmlListProperty<ChecklistItem> *list);

    Correction *m_correction;
};

#endif // CORRECTIONCONTROLLER_H
