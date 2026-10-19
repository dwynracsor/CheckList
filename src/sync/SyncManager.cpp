#include "sync/SyncManager.h"
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <random>

namespace checklist {

SyncManager::SyncManager()
    : config_{
        "",              // serverUrl
        "",              // apiKey
        300,             // syncIntervalSeconds (5 min)
        true,            // autoSync
        true             // syncOnChange
      },
      status_(SyncStatus::Offline),
      isOnline_(false) {}

SyncManager::~SyncManager() = default;

void SyncManager::setConfig(const SyncConfig& config) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    config_ = config;
    if (config_.serverUrl.empty()) {
        updateStatus(SyncStatus::Offline);
    } else if (isOnline_) {
        updateStatus(SyncStatus::Synced);
    }
}

void SyncManager::addPendingChange(ChangeType type, const std::string& taskId, const std::string& data) {
    std::lock_guard<std::mutex> lock(syncMutex_);

    PendingChange change;
    change.id = generateChangeId();
    change.type = type;
    change.taskId = taskId;
    change.data = data;
    change.timestamp = std::chrono::system_clock::now();
    change.retryCount = 0;
    change.synced = false;

    pendingChanges_.push_back(change);

    // Si auto-sync está activado y estamos online, intentar sincronizar
    if (config_.autoSync && config_.syncOnChange && isOnline_) {
        // En una implementación real, esto lanzaría un hilo async
        // Por ahora, marcamos que hay cambios pendientes
        updateStatus(SyncStatus::Syncing);
    }
}

void SyncManager::removePendingChange(const std::string& changeId) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    pendingChanges_.erase(
        std::remove_if(pendingChanges_.begin(), pendingChanges_.end(),
            [&](const PendingChange& c) { return c.id == changeId; }),
        pendingChanges_.end());
}

void SyncManager::markChangeSynced(const std::string& changeId) {
    std::lock_guard<std::mutex> lock(syncMutex_);
    for (auto& change : pendingChanges_) {
        if (change.id == changeId) {
            change.synced = true;
            break;
        }
    }
    // Limpiar cambios ya sincronizados
    pendingChanges_.erase(
        std::remove_if(pendingChanges_.begin(), pendingChanges_.end(),
            [](const PendingChange& c) { return c.synced; }),
        pendingChanges_.end());

    if (pendingChanges_.empty() && isOnline_) {
        updateStatus(SyncStatus::Synced);
    }
}

void SyncManager::syncNow() {
    std::lock_guard<std::mutex> lock(syncMutex_);

    if (!isOnline_) {
        updateStatus(SyncStatus::Offline);
        if (syncCompletedCb_) syncCompletedCb_(false, "No hay conexión");
        return;
    }

    if (config_.serverUrl.empty()) {
        updateStatus(SyncStatus::Error);
        if (syncCompletedCb_) syncCompletedCb_(false, "No hay servidor configurado");
        return;
    }

    updateStatus(SyncStatus::Syncing);

    // Simular subida de cambios pendientes
    for (auto& change : pendingChanges_) {
        if (!change.synced) {
            simulateUpload(change);
        }
    }

    // Simular descarga de cambios remotos
    simulateDownload();

    // Limpiar cambios sincronizados
    pendingChanges_.erase(
        std::remove_if(pendingChanges_.begin(), pendingChanges_.end(),
            [](const PendingChange& c) { return c.synced; }),
        pendingChanges_.end());

    updateStatus(SyncStatus::Synced);
    if (syncCompletedCb_) syncCompletedCb_(true, "");
}

void SyncManager::checkConnectivity() {
    // En una implementación real, esto haría un ping al servidor
    // Por ahora, simulamos que estamos online si hay URL configurada
    bool wasOnline = isOnline_;
    bool nowOnline = !config_.serverUrl.empty();

    if (wasOnline != nowOnline) {
        isOnline_ = nowOnline;
        if (nowOnline) {
            updateStatus(SyncStatus::Synced);
        } else {
            updateStatus(SyncStatus::Offline);
        }
    }
}

void SyncManager::setOnline(bool online) {
    if (isOnline_ != online) {
        isOnline_ = online;
        if (online) {
            checkConnectivity();
        } else {
            updateStatus(SyncStatus::Offline);
        }
    }
}

void SyncManager::resolveConflict(const std::string& taskId, bool useLocalVersion) {
    // En una implementación real, esto resolvería el conflicto
    // y enviaría la versión ganadora al servidor
    std::lock_guard<std::mutex> lock(syncMutex_);

    // Buscar si hay un conflicto pendiente para esta tarea
    // Por ahora, simplemente marcamos como resuelto
    updateStatus(SyncStatus::Synced);
}

std::string SyncManager::generateChangeId() {
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()).count();
    std::ostringstream oss;
    oss << "change_" << ms << "_" << std::rand();
    return oss.str();
}

void SyncManager::updateStatus(SyncStatus newStatus) {
    if (status_ != newStatus) {
        status_ = newStatus;
        if (statusChangedCb_) statusChangedCb_(status_);
    }
}

void SyncManager::simulateUpload(const PendingChange& change) {
    // Simular subida al servidor
    // En una implementación real, esto haría una petición HTTP POST
    std::cout << "[Sync] Subiendo cambio: " << change.id << " (tipo: " 
              << static_cast<int>(change.type) << ", tarea: " << change.taskId << ")" << std::endl;
    // Marcar como sincronizado (simulado)
    // Nota: esto no modifica pendingChanges_ directamente porque es const
    // En una implementación real, se haría async y se marcaría cuando termine
}

void SyncManager::simulateDownload() {
    // Simular descarga de cambios del servidor
    // En una implementación real, esto haría una petición HTTP GET
    std::cout << "[Sync] Descargando cambios remotos..." << std::endl;
    // Si hay cambios remotos que entran en conflicto, llamar a conflictCb_
}

} // namespace checklist