#include "ical/ICalExporter.h"
#include <sstream>
#include <iomanip>
#include <ctime>
#include <chrono>

namespace checklist {

ICalExporter::ICalExporter()
    : calendarName_("Checklist"),
      calendarDescription_("Checklist Application Tasks"),
      timeZoneId_("UTC") {}

ICalExporter::~ICalExporter() = default;

void ICalExporter::setCalendarName(const std::string& name) {
    calendarName_ = name;
}

void ICalExporter::setCalendarDescription(const std::string& desc) {
    calendarDescription_ = desc;
}

void ICalExporter::setTimeZone(const std::string& tzId) {
    timeZoneId_ = tzId;
}

std::string ICalExporter::escapeIcalText(const std::string& text) const {
    std::string result;
    for (char c : text) {
        switch (c) {
            case ',': result += "\\,"; break;
            case ';': result += "\\;"; break;
            case '\\': result += "\\\\"; break;
            case '\n': result += "\\n"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string ICalExporter::formatIcalDate(const Date& date) const {
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(4) << date.year
        << std::setw(2) << date.month
        << std::setw(2) << date.day;
    return oss.str();
}

std::string ICalExporter::generateUid(const std::string& taskId) const {
    return taskId + "@checklist.app";
}

std::string ICalExporter::exportTask(const std::shared_ptr<Task>& task) const {
    if (!task) return "";

    std::ostringstream oss;

    oss << "BEGIN:VEVENT\r\n";
    oss << "UID:" << generateUid(task->id()) << "\r\n";
    oss << "DTSTART:" << formatIcalDate(task->date()) << "T090000Z\r\n";
    oss << "DTEND:" << formatIcalDate(task->date()) << "T170000Z\r\n";
    oss << "SUMMARY:" << escapeIcalText(task->name()) << "\r\n";

    if (!task->category().empty()) {
        oss << "CATEGORIES:" << escapeIcalText(task->category()) << "\r\n";
    }

    oss << "DESCRIPTION:" << escapeIcalText("Task from Checklist App") << "\r\n";

    // Estado basado en si está terminada
    if (task->isFinished()) {
        oss << "STATUS:COMPLETED\r\n";
        oss << "COMPLETED:" << formatIcalDate(Date::today()) << "T170000Z\r\n";
    } else {
        oss << "STATUS:CONFIRMED\r\n";
    }

    // Prioridad basada en pomodoros (más pomodoros = mayor prioridad)
    if (task->pomodoroCount() > 0) {
        int priority = std::min(1 + task->pomodoroCount(), 9);
        oss << "PRIORITY:" << priority << "\r\n";
    }

    // Duración estimada
    if (task->estimatedDurationSeconds() > 0) {
        int minutes = static_cast<int>(task->estimatedDurationSeconds() / 60);
        oss << "DURATION:PT" << minutes << "M\r\n";
    }

    oss << "END:VEVENT\r\n";

    return oss.str();
}

std::string ICalExporter::exportTasks(const std::vector<std::shared_ptr<Task>>& tasks) const {
    std::ostringstream oss;

    oss << "BEGIN:VCALENDAR\r\n";
    oss << "VERSION:2.0\r\n";
    oss << "PRODID:-//ChecklistApp//Checklist//EN\r\n";
    oss << "CALSCALE:GREGORIAN\r\n";
    oss << "METHOD:PUBLISH\r\n";
    oss << "X-WR-CALNAME:" << escapeIcalText(calendarName_) << "\r\n";
    oss << "X-WR-CALDESC:" << escapeIcalText(calendarDescription_) << "\r\n";
    oss << "X-WR-TIMEZONE:" << timeZoneId_ << "\r\n";

    for (const auto& task : tasks) {
        if (task) {
            // Extraer solo el VEVENT de cada tarea
            std::string eventStr = exportTask(task);
            // Remover BEGIN:VEVENT y END:VEVENT ya que están en el nivel superior
            // En una implementación más limpia, esto se manejaría diferente
            oss << eventStr;
        }
    }

    oss << "END:VCALENDAR\r\n";

    return oss.str();
}

std::string ICalExporter::exportAll(const std::vector<std::shared_ptr<Task>>& tasks) const {
    return exportTasks(tasks);
}

} // namespace checklist