#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include "domain/Task.h"

namespace checklist {

class ICalExporter {
public:
    using Ptr = std::shared_ptr<ICalExporter>;

    ICalExporter();
    ~ICalExporter();

    // Exportar una sola tarea a formato iCal
    std::string exportTask(const std::shared_ptr<Task>& task) const;

    // Exportar múltiples tareas a formato iCal
    std::string exportTasks(const std::vector<std::shared_ptr<Task>>& tasks) const;

    // Exportar todas las tareas a un string .ics
    std::string exportAll(const std::vector<std::shared_ptr<Task>>& tasks) const;

    // Configurar propiedades del calendario
    void setCalendarName(const std::string& name);
    void setCalendarDescription(const std::string& desc);
    void setTimeZone(const std::string& tzId);

private:
    std::string calendarName_;
    std::string calendarDescription_;
    std::string timeZoneId_;

    std::string escapeIcalText(const std::string& text) const;
    std::string formatIcalDate(const Date& date) const;
    std::string generateUid(const std::string& taskId) const;
};

} // namespace checklist