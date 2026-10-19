#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <functional>
#include <mutex>
#include <atomic>
#include <cstdint>

namespace checklist {

enum class SyncStatus {
    Offline,        // Sin conexión
    Synced,         // Sincronizado
    Syncing,        // Sincronizando
    Conflict,       // Conflicto sin resolver
    Error           // Error en sincronización
};

enum class ChangeType {
    Create,         // Nueva tarea
    Update,         // Tarea modificada
    Delete,         // Tarea eliminada
    ItemToggle,     // Item marcado/desmarcado
    ItemTextChange, // Texto de item cambiado
    TagAdded,       // Tag agregado
    TagRemoved,     // Tag removido
    DependencyChanged, // Dependencia cambiada
    AttachmentAdded,   // Adjunto agregado
    AttachmentRemoved  // Adjunto removido
};

struct PendingChange {
    std::string id;               // UUID del cambio
    ChangeType type;              // Tipo de cambio
    std::string taskId;           // ID de la tarea afectada
    std::string data;             // Datos JSON del cambio
    std::chrono::system_clock::time_point timestamp; // Cuándo ocurrió
    int retryCount;               // Número de reintentos
    bool synced;                  // Ya se sincronizó con el servidor
};

struct SyncConfig {
    std::string serverUrl;        // URL del servidor de sync
    std::string apiKey;           // API key para autenticación
    int syncIntervalSeconds;      // Intervalo de auto-sync (0 = desactivado)
    bool autoSync;                // Sincronizar automáticamente
    bool syncOnChange;            // Sincronizar al detectar cambios
};

class SyncManager {
public:
    using Ptr = std::shared_ptr<SyncManager>;

    SyncManager();
    ~SyncManager();

    // Configuración
    void setConfig(const SyncConfig& config);
    const SyncConfig& config() const { return config_; }

    // Estado
    SyncStatus status() const { return status_; }
    bool isOnline() const { return isOnline_; }
    int pendingChangesCount() const { return static_cast<int>(pendingChanges_.size()); }

    // Callbacks
    using StatusChangedCallback = std::function<void(SyncStatus)>;
    using SyncCompletedCallback = std::function<void(bool success, std::string error)>;
    using ConflictDetectedCallback = std::function<void(std::string taskId, std::string localData, std::string remoteData)>;

    void setStatusChangedCallback(StatusChangedCallback cb) { statusChangedCb_ = std::move(cb); }
    void setSyncCompletedCallback(SyncCompletedCallback cb) { syncCompletedCb_ = std::move(cb); }
    void setConflictDetectedCallback(ConflictDetectedCallback cb) { conflictCb_ = std::move(cb); }

    // Operaciones de cola de cambios
    void addPendingChange(ChangeType type, const std::string& taskId, const std::string& data);
    void removePendingChange(const std::string& changeId);
    void markChangeSynced(const std::string& changeId);
    const std::vector<PendingChange>& pendingChanges() const { return pendingChanges_; }

    // Operaciones de sincronización
    void syncNow();
    void checkConnectivity();
    void setOnline(bool online);

    // Resolución de conflictos
    void resolveConflict(const std::string& taskId, bool useLocalVersion);

private:
    SyncConfig config_;
    SyncStatus status_;
    std::atomic<bool> isOnline_;
    std::vector<PendingChange> pendingChanges_;
    std::mutex syncMutex_;

    // Callbacks
    StatusChangedCallback statusChangedCb_;
    SyncCompletedCallback syncCompletedCb_;
    ConflictDetectedCallback conflictCb_;

    // Helpers privados
    std::string generateChangeId();
    void updateStatus(SyncStatus newStatus);
    void simulateUpload(const PendingChange& change); // Para demo sin servidor real
    void simulateDownload();                           // Para demo sin servidor real
};

} // namespace checklist