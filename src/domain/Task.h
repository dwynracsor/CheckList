#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <atomic>
#include "Date.h"
#include "ChecklistItem.h"
#include "Correction.h"

namespace checklist {

enum class TaskDependency {
    None,              // Sin dependencia
    MustCompleteBefore // Esta tarea debe completarse antes que la dependencia
};

struct TaskAttachment {
    std::string id;
    std::string fileName;
    std::string filePath;
    std::string fileType;     // MIME type o extensión
    int64_t fileSize;         // Tamaño en bytes
    std::string addedDate;    // yyyy-MM-dd

    bool operator==(const TaskAttachment& other) const {
        return id == other.id;
    }
};

class Task {
public:
    using Ptr = std::shared_ptr<Task>;

    Task();
    Task(const std::string& name, const Date& date);
    Task(const std::string& id, const std::string& name, const Date& date);
    ~Task();

    // Copy/move
    Task(const Task& other);
    Task& operator=(const Task& other);
    Task(Task&& other) noexcept;
    Task& operator=(Task&& other) noexcept;

    const std::string& id() const { return id_; }
    const std::string& name() const { return name_; }
    const Date& date() const { return date_; }
    const std::string& category() const { return category_; }

    bool isFinished() const;
    size_t itemCount() const { return items_.size(); }
    size_t correctionCount() const { return corrections_.size(); }

    ChecklistItem* getItem(size_t index);
    const ChecklistItem* getItem(size_t index) const;
    Correction* getCorrection(size_t index);
    const Correction* getCorrection(size_t index) const;

    void addItem(const std::shared_ptr<ChecklistItem>& item);
    void removeItem(size_t index);
    std::shared_ptr<Correction> addCorrection(const std::shared_ptr<Correction>& correction);
    void removeCorrection(size_t index);

    // --- Tiempo y Pomodoro ---

    // Duración estimada en segundos
    int64_t estimatedDurationSeconds() const { return estimatedDurationSeconds_; }
    void setEstimatedDurationSeconds(int64_t seconds);

    // Duración real en segundos (tiempo invertido)
    int64_t actualDurationSeconds() const { return actualDurationSeconds_; }

    // Contador de pomodoros
    int pomodoroCount() const { return pomodoroCount_; }
    void incrementPomodoro();
    void resetPomodoros();

    // Estado del cronómetro
    bool isTiming() const { return isTiming_; }
    void startTiming();
    void stopTiming();

    // Tiempo restante (estimado - real)
    int64_t remainingTimeSeconds() const;

    // Porcentaje de progreso basado en tiempo (0-100)
    int progressPercentage() const;

    // Session start time for active timing
    std::chrono::system_clock::time_point sessionStartTime() const { return sessionStartTime_; }

    // --- Dependencias entre tareas ---

    TaskDependency dependencyType() const { return dependencyType_; }
    const std::string& dependencyId() const { return dependencyId_; }

    // Establecer una dependencia (dependencyId vacía = sin dependencia)
    void setDependency(TaskDependency type, const std::string& dependsOnId);

    // Verificar si esta tarea está bloqueada por una dependencia
    bool isBlockedByDependency(const std::shared_ptr<Task>& dependencyTask) const;

    // Verificar si puede marcarse como completada (todas las dependencias cumplidas)
    bool canComplete() const;

    // --- Etiquetas (Tags) multietiqueta ---

    const std::vector<std::string>& tags() const { return tags_; }
    size_t tagCount() const { return tags_.size(); }
    bool hasTag(const std::string& tag) const;
    void addTag(const std::string& tag);
    void removeTag(const std::string& tag);
    void clearTags();
    void setTags(const std::vector<std::string>& newTags);

    // --- Adjuntos/Archivos ---

    const std::vector<TaskAttachment>& attachments() const { return attachments_; }
    size_t attachmentCount() const { return attachments_.size(); }
    const TaskAttachment* getAttachment(size_t index) const;

    void addAttachment(const TaskAttachment& attachment);
    void removeAttachment(const std::string& attachmentId);
    void clearAttachments();
    void setAttachments(const std::vector<TaskAttachment>& newAttachments);

    // Event callbacks
    using NameChangedCallback = std::function<void()>;
    using CategoryChangedCallback = std::function<void()>;
    using ItemCountChangedCallback = std::function<void()>;
    using CorrectionCountChangedCallback = std::function<void()>;
    using ItemsChangedCallback = std::function<void()>;
    using CorrectionsChangedCallback = std::function<void()>;
    using DurationChangedCallback = std::function<void()>;
    using PomodoroChangedCallback = std::function<void()>;
    using TimingStateChangedCallback = std::function<void()>;
    using DependencyChangedCallback = std::function<void()>;
    using TagsChangedCallback = std::function<void()>;
    using AttachmentChangedCallback = std::function<void()>;

    void setNameChangedCallback(NameChangedCallback cb) { nameChangedCb_ = std::move(cb); }
    void setCategoryChangedCallback(CategoryChangedCallback cb) { categoryChangedCb_ = std::move(cb); }
    void setItemCountChangedCallback(ItemCountChangedCallback cb) { itemCountChangedCb_ = std::move(cb); }
    void setCorrectionCountChangedCallback(CorrectionCountChangedCallback cb) { correctionCountChangedCb_ = std::move(cb); }
    void setItemsChangedCallback(ItemsChangedCallback cb) { itemsChangedCb_ = std::move(cb); }
    void setCorrectionsChangedCallback(CorrectionsChangedCallback cb) { correctionsChangedCb_ = std::move(cb); }
    void setDurationChangedCallback(DurationChangedCallback cb) { durationChangedCb_ = std::move(cb); }
    void setPomodoroChangedCallback(PomodoroChangedCallback cb) { pomodoroChangedCb_ = std::move(cb); }
    void setTimingStateChangedCallback(TimingStateChangedCallback cb) { timingStateChangedCb_ = std::move(cb); }
    void setDependencyChangedCallback(DependencyChangedCallback cb) { dependencyChangedCb_ = std::move(cb); }
    void setTagsChangedCallback(TagsChangedCallback cb) { tagsChangedCb_ = std::move(cb); }
    void setAttachmentChangedCallback(AttachmentChangedCallback cb) { attachmentChangedCb_ = std::move(cb); }

    // Invoke callbacks
    void invokeNameChanged() const;
    void invokeCategoryChanged() const;
    void invokeItemCountChanged() const;
    void invokeCorrectionCountChanged() const;
    void invokeItemsChanged() const;
    void invokeCorrectionsChanged() const;
    void invokeDurationChanged() const;
    void invokePomodoroChanged() const;
    void invokeTimingStateChanged() const;
    void invokeDependencyChanged() const;
    void invokeTagsChanged() const;
    void invokeAttachmentChanged() const;

    // String operations
    static std::vector<std::string> tokenize(const std::string& text);
    static std::string generateId();

    void setName(const std::string& name);
    void setCategory(const std::string& category);

private:
    std::string id_;
    std::string name_;
    Date date_;
    std::string category_;
    std::vector<std::shared_ptr<ChecklistItem>> items_;
    std::vector<std::shared_ptr<Correction>> corrections_;

    // --- Tiempo y Pomodoro ---
    int64_t estimatedDurationSeconds_;   // Tiempo estimado en segundos (0 = sin estimación)
    int64_t actualDurationSeconds_;      // Tiempo real invertido en segundos
    int pomodoroCount_;                  // Contador de pomodoros completados
    std::atomic<bool> isTiming_;         // ¿Está corriendo un cronómetro?
    std::chrono::system_clock::time_point sessionStartTime_; // Tiempo de inicio de sesión actual

    // --- Dependencias ---
    TaskDependency dependencyType_;      // Tipo de dependencia
    std::string dependencyId_;           // ID de la tarea de la que depende (vacía = ninguna)

    // --- Etiquetas ---
    std::vector<std::string> tags_;      // Etiquetas multietiqueta (ej. "trabajo", "urgente", "casa")

    // --- Adjuntos ---
    std::vector<TaskAttachment> attachments_;

    // Callbacks
    NameChangedCallback nameChangedCb_;
    CategoryChangedCallback categoryChangedCb_;
    ItemCountChangedCallback itemCountChangedCb_;
    CorrectionCountChangedCallback correctionCountChangedCb_;
    ItemsChangedCallback itemsChangedCb_;
    CorrectionsChangedCallback correctionsChangedCb_;
    DurationChangedCallback durationChangedCb_;
    PomodoroChangedCallback pomodoroChangedCb_;
    TimingStateChangedCallback timingStateChangedCb_;
    DependencyChangedCallback dependencyChangedCb_;
    TagsChangedCallback tagsChangedCb_;
    AttachmentChangedCallback attachmentChangedCb_;

};

} // namespace checklist