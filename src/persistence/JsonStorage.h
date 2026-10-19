#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "domain/Task.h"
#include "domain/TaskTemplate.h"
#include "nlohmann/json.hpp"

namespace checklist {

class JsonStorage {
public:
    static constexpr int SchemaVersion = 2;

    JsonStorage();
    explicit JsonStorage(const std::string& filePath);
    ~JsonStorage();

    const std::string& filePath() const { return filePath_; }
    void setFilePath(const std::string& path);

    // Events
    using FilePathChangedCallback = std::function<void()>;
    using SavedCallback = std::function<void()>;
    using LoadedCallback = std::function<void()>;
    using ErrorCallback = std::function<void(const std::string&)>;

    void setFilePathChangedCallback(FilePathChangedCallback cb) { filePathChangedCb_ = std::move(cb); }
    void setSavedCallback(SavedCallback cb) { savedCb_ = std::move(cb); }
    void setLoadedCallback(LoadedCallback cb) { loadedCb_ = std::move(cb); }
    void setErrorCallback(ErrorCallback cb) { errorCb_ = std::move(cb); }

    // Core operations
    std::vector<std::shared_ptr<Task>> load();
    bool save(const std::vector<std::shared_ptr<Task>>& tasks);

    // Template operations
    std::vector<std::shared_ptr<TaskTemplate>> loadTemplates();
    bool saveTemplates(const std::vector<std::shared_ptr<TaskTemplate>>& templates);

    // Static helpers for import/export
    static std::vector<nlohmann::json> tryParseIntoArray(const std::string& jsonString, std::string& error);
    static std::shared_ptr<Task> jsonToTask(const nlohmann::json& json);
    static nlohmann::json taskToJson(const std::shared_ptr<Task>& task);
    static std::shared_ptr<TaskTemplate> jsonToTemplate(const nlohmann::json& json);
    static nlohmann::json templateToJson(const std::shared_ptr<TaskTemplate>& template_);

private:
    std::string filePath_;
    
    // Callbacks
    FilePathChangedCallback filePathChangedCb_;
    SavedCallback savedCb_;
    LoadedCallback loadedCb_;
    ErrorCallback errorCb_;

    // Helpers
    bool ensureDirectoryExists() const;
    bool writeAtomic(const std::string& data) const;
    static std::string dateToString(const Date& date);
    static Date stringToDate(const std::string& str);
};

} // namespace checklist