#include "ical/ICalImporter.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>

namespace checklist {

ICalImporter::ICalImporter() {}

ICalImporter::~ICalImporter() = default;

std::vector<std::string> ICalImporter::splitLines(const std::string& data) const {
    std::vector<std::string> lines;
    std::istringstream iss(data);
    std::string line;
    while (std::getline(iss, line)) {
        // Manejar líneas continuadas (líneas que empiezan con espacio)
        if (!line.empty() && (line[0] == ' ' || line[0] == '\t')) {
            if (!lines.empty()) {
                lines.back() += line.substr(1);
            }
        } else {
            lines.push_back(line);
        }
    }
    return lines;
}

std::string ICalImporter::unescapeIcalText(const std::string& text) const {
    std::string result;
    for (size_t i = 0; i < text.size(); ++i) {
        if (text[i] == '\\' && i + 1 < text.size()) {
            switch (text[i + 1]) {
                case ',': result += ','; break;
                case ';': result += ';'; break;
                case '\\': result += '\\'; break;
                case 'n': result += '\n'; break;
                case 'N': result += '\n'; break;
                default: result += text[i + 1]; break;
            }
            ++i;
        } else {
            result += text[i];
        }
    }
    return result;
}

Date ICalImporter::parseIcalDate(const std::string& dateStr) const {
    // Formato: YYYYMMDD o YYYYMMDDTHHMMSSZ
    if (dateStr.size() < 8) return Date::today();

    int year = 0, month = 0, day = 0;
    try {
        year = std::stoi(dateStr.substr(0, 4));
        month = std::stoi(dateStr.substr(4, 2));
        day = std::stoi(dateStr.substr(6, 2));
    } catch (...) {
        return Date::today();
    }

    return Date(year, month, day);
}

std::string ICalImporter::extractPropertyValue(const std::string& line, const std::string& propName) const {
    std::string prefix = propName + ":";
    if (line.substr(0, prefix.size()) == prefix) {
        return line.substr(prefix.size());
    }
    return "";
}

bool ICalImporter::isVeventBlock(const std::string& block) const {
    return block.find("BEGIN:VEVENT") != std::string::npos;
}

std::vector<ImportedTask> ICalImporter::importFromString(const std::string& icsData) const {
    std::vector<ImportedTask> tasks;

    auto lines = splitLines(icsData);

    // Extraer bloques VEVENT
    std::vector<std::string> veventBlocks;
    std::string currentBlock;
    bool inVevent = false;

    for (const auto& line : lines) {
        if (line == "BEGIN:VEVENT") {
            inVevent = true;
            currentBlock = line + "\n";
        } else if (line == "END:VEVENT") {
            if (inVevent) {
                currentBlock += line + "\n";
                veventBlocks.push_back(currentBlock);
                currentBlock.clear();
                inVevent = false;
            }
        } else if (inVevent) {
            currentBlock += line + "\n";
        }
    }

    // Procesar cada bloque VEVENT
    for (const auto& block : veventBlocks) {
        ImportedTask imported;
        imported.completed = false;
        imported.priority = 0;
        imported.durationMinutes = 0;

        auto blockLines = splitLines(block);

        for (const auto& line : blockLines) {
            if (line.substr(0, 4) == "UID:") {
                imported.uid = extractPropertyValue(line, "UID");
            } else if (line.substr(0, 7) == "SUMMARY") {
                imported.summary = unescapeIcalText(extractPropertyValue(line, "SUMMARY"));
            } else if (line.substr(0, 11) == "DESCRIPTION") {
                imported.description = unescapeIcalText(extractPropertyValue(line, "DESCRIPTION"));
            } else if (line.substr(0, 10) == "CATEGORIES") {
                imported.categories = unescapeIcalText(extractPropertyValue(line, "CATEGORIES"));
            } else if (line.substr(0, 8) == "DTSTART:") {
                imported.startDate = parseIcalDate(extractPropertyValue(line, "DTSTART"));
            } else if (line.substr(0, 6) == "DTEND:") {
                imported.endDate = parseIcalDate(extractPropertyValue(line, "DTEND"));
            } else if (line.substr(0, 7) == "STATUS:") {
                std::string status = extractPropertyValue(line, "STATUS");
                imported.completed = (status == "COMPLETED");
            } else if (line.substr(0, 9) == "PRIORITY:") {
                try {
                    imported.priority = std::stoi(extractPropertyValue(line, "PRIORITY"));
                } catch (...) {
                    imported.priority = 0;
                }
            } else if (line.substr(0, 9) == "DURATION:") {
                std::string durStr = extractPropertyValue(line, "DURATION");
                // Parse simple: PTnnM
                if (durStr.size() > 2 && durStr[0] == 'P' && durStr[1] == 'T') {
                    try {
                        std::string minStr = durStr.substr(2);
                        if (!minStr.empty() && minStr.back() == 'M') {
                            minStr.pop_back();
                            imported.durationMinutes = std::stoi(minStr);
                        }
                    } catch (...) {}
                }
            }
        }

        if (!imported.uid.empty() && !imported.summary.empty()) {
            tasks.push_back(imported);
        }
    }

    return tasks;
}

std::vector<ImportedTask> ICalImporter::importFromFile(const std::string& filePath) const {
    // En una implementación real, esto leería el archivo del disco
    // Por ahora, retornamos vacío (el usuario puede usar importFromString)
    std::vector<ImportedTask> empty;
    return empty;
}

std::shared_ptr<Task> ICalImporter::importTaskToTask(const ImportedTask& imported,
                                                      const std::string& newId) const {
    std::string id = newId.empty() ? imported.uid : newId;

    auto task = std::make_shared<Task>(id, imported.summary, imported.startDate);

    if (!imported.categories.empty()) {
        task->setCategory(imported.categories);
    }

    // Si está completada, marcar todos los items como hechos
    // (En una implementación completa, se crearían items desde la descripción)
    // Por ahora, solo marcamos la fecha

    // Establecer duración estimada si hay duración
    if (imported.durationMinutes > 0) {
        task->setEstimatedDurationSeconds(imported.durationMinutes * 60);
    }

    // Establecer pomodoros basados en prioridad (mapeo simple)
    if (imported.priority > 0) {
        for (int i = 0; i < imported.priority - 1; ++i) {
            task->incrementPomodoro();
        }
    }

    return task;
}

} // namespace checklist