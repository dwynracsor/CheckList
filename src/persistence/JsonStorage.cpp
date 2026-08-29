#include "persistence/JsonStorage.h"
#include "domain/Task.h"
#include "domain/Correction.h"
#include "domain/ChecklistItem.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QUuid>

JsonStorage::JsonStorage(QObject *parent)
    : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    m_filePath = dataDir + "/tasks.json";
}

JsonStorage::JsonStorage(const QString &filePath, QObject *parent)
    : QObject(parent)
    , m_filePath(filePath)
{
}

JsonStorage::~JsonStorage() = default;

QString JsonStorage::filePath() const
{
    return m_filePath;
}

void JsonStorage::setFilePath(const QString &path)
{
    if (m_filePath != path) {
        m_filePath = path;
        emit filePathChanged();
    }
}

// --- Core Operations ---

QList<Task*> JsonStorage::load()
{
    QList<Task*> tasks;

    QFile file(m_filePath);
    if (!file.exists()) {
        emit loaded();
        return tasks;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit error("No se pudo abrir el archivo: " + file.errorString());
        return tasks;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit error("Error al parsear JSON: " + parseError.errorString());
        return tasks;
    }

    QJsonArray array;

    if (doc.isObject()) {
        // New format: { "version": 1, "tasks": [...] }
        QJsonObject root = doc.object();
        int version = root["version"].toInt(0);
        if (version > SCHEMA_VERSION) {
            emit error("Versión de schema no soportada: " + QString::number(version));
            return tasks;
        }
        array = root["tasks"].toArray();
    } else if (doc.isArray()) {
        // Legacy format: bare array [...]
        array = doc.array();
    } else {
        emit error("El archivo JSON no contiene un objeto o array");
        return tasks;
    }

    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            Task *task = jsonToTask(value.toObject());
            if (task) {
                tasks.append(task);
            }
        }
    }

    emit loaded();
    return tasks;
}

bool JsonStorage::save(const QList<Task*> &tasks)
{
    if (!ensureDirectoryExists()) {
        emit error("No se pudo crear el directorio de datos");
        return false;
    }

    QJsonArray tasksArray;
    for (Task *task : tasks) {
        tasksArray.append(taskToJson(task));
    }

    QJsonObject root;
    root["version"] = SCHEMA_VERSION;
    root["tasks"] = tasksArray;

    QJsonDocument doc(root);
    QByteArray data = doc.toJson(QJsonDocument::Indented);

    if (writeAtomic(data)) {
        emit saved();
        return true;
    }

    emit error("No se pudo escribir el archivo");
    return false;
}

// --- Serialization ---

QJsonObject JsonStorage::itemToJson(ChecklistItem *item)
{
    if (!item) return QJsonObject();
    return {
        {"id", item->id()},
        {"text", item->text()},
        {"done", item->done()}
    };
}

QJsonObject JsonStorage::correctionToJson(Correction *correction)
{
    if (!correction) return QJsonObject();

    QJsonArray itemsArray;
    for (int i = 0; i < correction->itemCountRaw(); ++i) {
        itemsArray.append(itemToJson(correction->getItem(i)));
    }

    return {
        {"id", correction->id()},
        {"name", correction->name()},
        {"date", correction->date()},
        {"items", itemsArray}
    };
}

QJsonObject JsonStorage::taskToJson(Task *task)
{
    if (!task) return QJsonObject();

    QJsonArray itemsArray;
    for (int i = 0; i < task->itemCount(); ++i) {
        itemsArray.append(itemToJson(task->getItem(i)));
    }

    QJsonArray correctionsArray;
    for (int i = 0; i < task->correctionCount(); ++i) {
        correctionsArray.append(correctionToJson(task->getCorrection(i)));
    }

    return {
        {"id", task->id()},
        {"name", task->name()},
        {"date", task->date()},
        {"items", itemsArray},
        {"corrections", correctionsArray}
    };
}

// --- Deserialization ---

ChecklistItem* JsonStorage::jsonToItem(const QJsonObject &json, QObject *parent)
{
    QString id = json["id"].toString();
    QString text = json["text"].toString();
    bool done = json["done"].toBool(false);

    // Normalize: if id is missing or empty, generate one
    if (id.isEmpty()) {
        id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    return new ChecklistItem(id, text, done, parent);
}

Correction* JsonStorage::jsonToCorrection(const QJsonObject &json, QObject *parent)
{
    QString id = json["id"].toString();
    QString name = json["name"].toString("Sin nombre");
    QDate date = stringToDate(json["date"].toString());

    if (id.isEmpty()) {
        id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    Correction *correction = new Correction(id, name, date, parent);

    QJsonArray itemsArray = json["items"].toArray();
    for (const QJsonValue &val : itemsArray) {
        if (val.isObject()) {
            ChecklistItem *item = jsonToItem(val.toObject(), correction);
            correction->addItem(item);
        }
    }

    return correction;
}

Task* JsonStorage::jsonToTask(const QJsonObject &json, QObject *parent)
{
    QString id = json["id"].toString();
    QString name = json["name"].toString("Sin nombre");
    QDate date = stringToDate(json["date"].toString());

    if (id.isEmpty()) {
        id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }

    Task *task = new Task(id, name, date, parent);

    // Items
    QJsonArray itemsArray = json["items"].toArray();
    for (const QJsonValue &val : itemsArray) {
        if (val.isObject()) {
            ChecklistItem *item = jsonToItem(val.toObject(), task);
            task->addItem(item);
        }
    }

    // Corrections
    QJsonArray correctionsArray = json["corrections"].toArray();
    for (const QJsonValue &val : correctionsArray) {
        if (val.isObject()) {
            Correction *correction = jsonToCorrection(val.toObject(), task);
            task->addCorrection(correction);
        }
    }

    return task;
}

// --- Date Helpers ---

QString JsonStorage::dateToString(const QDate &date)
{
    return date.toString("yyyy-MM-dd");
}

QDate JsonStorage::stringToDate(const QString &str)
{
    QDate date = QDate::fromString(str, "yyyy-MM-dd");
    return date.isValid() ? date : QDate::currentDate();
}

// --- Private Helpers ---

bool JsonStorage::ensureDirectoryExists() const
{
    QFileInfo info(m_filePath);
    QDir dir = info.absoluteDir();
    if (!dir.exists()) {
        return dir.mkpath(".");
    }
    return true;
}

bool JsonStorage::writeAtomic(const QByteArray &data)
{
    QString tempPath = m_filePath + ".tmp." + QUuid::createUuid().toString(QUuid::WithoutBraces);

    QFile tempFile(tempPath);
    if (!tempFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    qint64 written = tempFile.write(data);
    tempFile.close();

    if (written != data.size()) {
        tempFile.remove();
        return false;
    }

    // Atomic replace
    QFile::remove(m_filePath);
    bool ok = tempFile.rename(m_filePath);
    if (!ok) {
        tempFile.remove();
    }
    return ok;
}
