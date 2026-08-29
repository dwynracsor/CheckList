#ifndef JSONSTORAGE_H
#define JSONSTORAGE_H

#include <QObject>
#include <QString>
#include <QJsonObject>
#include <QJsonArray>

class Task;
class Correction;
class ChecklistItem;

class JsonStorage : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)

public:
    explicit JsonStorage(QObject *parent = nullptr);
    explicit JsonStorage(const QString &filePath, QObject *parent = nullptr);
    ~JsonStorage() override;

    QString filePath() const;
    void setFilePath(const QString &path);

    // Core operations
    Q_INVOKABLE QList<Task*> load();
    Q_INVOKABLE bool save(const QList<Task*> &tasks);

    // Static helpers (useful for testing)
    static QJsonObject taskToJson(Task *task);
    static QJsonObject correctionToJson(Correction *correction);
    static QJsonObject itemToJson(ChecklistItem *item);

    static Task* jsonToTask(const QJsonObject &json, QObject *parent = nullptr);
    static Correction* jsonToCorrection(const QJsonObject &json, QObject *parent = nullptr);
    static ChecklistItem* jsonToItem(const QJsonObject &json, QObject *parent = nullptr);

    // Schema versioning
    static constexpr int SCHEMA_VERSION = 1;

    // Date formatting helpers (matches web format: yyyy-MM-dd)
    static QString dateToString(const QDate &date);
    static QDate stringToDate(const QString &str);

signals:
    void filePathChanged();
    void saved();
    void loaded();
    void error(const QString &message);

private:
    bool ensureDirectoryExists() const;
    bool writeAtomic(const QByteArray &data);

    QString m_filePath;
};

#endif // JSONSTORAGE_H
