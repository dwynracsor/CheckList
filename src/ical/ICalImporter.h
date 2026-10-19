#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "domain/Task.h"
#include "domain/Date.h"

namespace checklist {

struct ImportedTask {
    std::string uid;
    std::string summary;
    std::string description;
    std::string categories;
    Date startDate;
    Date endDate;
    bool completed;
    int priority;
    int durationMinutes;
};

class ICalImporter {
public:
    using Ptr = std::shared_ptr<ICalImporter>;

    ICalImporter();
    ~ICalImporter();

    // Importar tareas desde un string .ics
    std::vector<ImportedTask> importFromString(const std::string& icsData) const;

    // Importar desde un archivo .ics
    std::vector<ImportedTask> importFromFile(const std::string& filePath) const;

    // Crear un Task desde un ImportedTask
    std::shared_ptr<Task> importTaskToTask(const ImportedTask& imported,
                                           const std::string& newId = "") const;

private:
    std::vector<std::string> splitLines(const std::string& data) const;
    std::string unescapeIcalText(const std::string& text) const;
    Date parseIcalDate(const std::string& dateStr) const;
    std::string extractPropertyValue(const std::string& line, const std::string& propName) const;
    bool isVeventBlock(const std::string& block) const;
};

} // namespace checklist