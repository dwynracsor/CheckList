#include "StatsCalculator.h"
#include "Task.h"
#include "ChecklistItem.h"
#include "Date.h"
#include <algorithm>
#include <sstream>
#include <set>

namespace checklist {

Date StatsCalculator::getDateFromString(const std::string& dateStr) {
    int year = 0, month = 0, day = 0;
    if (std::sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) == 3) {
        return Date(year, month, day);
    }
    return Date::today();
}

std::string StatsCalculator::dateToString(const Date& date) {
    return date.toString();
}

std::vector<DailyStat> StatsCalculator::getDailyCompletedStats(
    const std::vector<std::shared_ptr<Task>>& tasks,
    int days) {

    std::vector<DailyStat> result;

    // Calcular el rango de fechas
    Date today = Date::today();
    std::set<Date> dateSet;

    for (int i = days - 1; i >= 0; --i) {
        // Simplemente crear fechas aproximadas restando días
        // Usamos una aproximación simple para los días pasados
        Date d(today.year, today.month, today.day - i);
        if (d.day < 1) {
            // Ajuste simple para mes anterior
            d.month--;
            if (d.month < 1) {
                d.month = 12;
                d.year--;
            }
            d.day = 30; // aproximación
        }
        dateSet.insert(d);
    }

    // Inicializar estadísticas para cada fecha
    for (const auto& d : dateSet) {
        DailyStat stat;
        stat.date = dateToString(d);
        stat.tasksCompleted = 0;
        stat.itemsChecked = 0;
        stat.timeSpentSeconds = 0;
        result.push_back(stat);
    }

    // Ordenar por fecha
    std::sort(result.begin(), result.end(),
        [](const DailyStat& a, const DailyStat& b) { return a.date < b.date; });

    // Contar tareas completadas por fecha
    // Nota: En esta implementación simplificada, usamos la fecha de creación
    // de la tarea como referencia. En una versión completa, se tendría un
    // historial de cuándo se completó cada tarea.
    for (const auto& task : tasks) {
        if (task->isFinished()) {
            std::string taskDate = dateToString(task->date());
            for (auto& stat : result) {
                if (stat.date == taskDate) {
                    stat.tasksCompleted++;
                    // Contar items checkeados
                    for (size_t i = 0; i < task->itemCount(); ++i) {
                        auto item = task->getItem(i);
                        if (item && item->done()) {
                            stat.itemsChecked++;
                        }
                    }
                    stat.timeSpentSeconds += task->actualDurationSeconds();
                    break;
                }
            }
        }
    }

    return result;
}

ProductivityStreak StatsCalculator::calculateStreak(
    const std::vector<std::shared_ptr<Task>>& tasks) {

    ProductivityStreak streak;
    streak.currentStreak = 0;
    streak.maxStreak = 0;
    streak.daysActive = 0;

    if (tasks.empty()) return streak;

    // Recopilar fechas únicas donde se completaron tareas
    std::set<Date> completedDates;
    for (const auto& task : tasks) {
        if (task->isFinished()) {
            completedDates.insert(getDateFromString(dateToString(task->date())));
        }
    }

    streak.daysActive = static_cast<int>(completedDates.size());

    if (completedDates.empty()) return streak;

    // Calcular racha actual (desde hoy hacia atrás)
    Date today = Date::today();
    int current = 0;
    Date checkDate(today.year, today.month, today.day);

    while (completedDates.find(checkDate) != completedDates.end()) {
        current++;
        checkDate.day--;
        if (checkDate.day < 1) {
            checkDate.month--;
            if (checkDate.month < 1) {
                checkDate.month = 12;
                checkDate.year--;
            }
            checkDate.day = 30; // aproximación
        }
    }
    streak.currentStreak = current;

    // Calcular racha máxima
    int maxStreak = 0;
    int tempStreak = 0;
    Date prevDate(0, 0, 0);
    bool first = true;

    for (const auto& d : completedDates) {
        if (first) {
            tempStreak = 1;
            prevDate = d;
            first = false;
        } else {
            // Verificar si es consecutivo (día anterior)
            Date expected(prevDate.year, prevDate.month, prevDate.day - 1);
            if (expected.day < 1) {
                expected.month--;
                if (expected.month < 1) {
                    expected.month = 12;
                    expected.year--;
                }
                expected.day = 30;
            }

            if (d.year == expected.year && d.month == expected.month && d.day == expected.day) {
                tempStreak++;
            } else {
                maxStreak = std::max(maxStreak, tempStreak);
                tempStreak = 1;
            }
            prevDate = d;
        }
    }
    maxStreak = std::max(maxStreak, tempStreak);
    streak.maxStreak = maxStreak;

    return streak;
}

std::vector<CategoryTimeStat> StatsCalculator::getCategoryTimeDistribution(
    const std::vector<std::shared_ptr<Task>>& tasks) {

    std::map<std::string, CategoryTimeStat> categoryMap;

    for (const auto& task : tasks) {
        std::string cat = task->category();
        if (cat.empty()) cat = "Sin categoría";

        if (categoryMap.find(cat) == categoryMap.end()) {
            CategoryTimeStat stat;
            stat.category = cat;
            stat.totalTimeSeconds = 0;
            stat.taskCount = 0;
            categoryMap[cat] = stat;
        }

        categoryMap[cat].totalTimeSeconds += task->actualDurationSeconds();
        categoryMap[cat].taskCount++;
    }

    std::vector<CategoryTimeStat> result;
    for (const auto& pair : categoryMap) {
        result.push_back(pair.second);
    }

    // Ordenar por tiempo total descendente
    std::sort(result.begin(), result.end(),
        [](const CategoryTimeStat& a, const CategoryTimeStat& b) {
            return a.totalTimeSeconds > b.totalTimeSeconds;
        });

    return result;
}

WeeklyReport StatsCalculator::generateWeeklyReport(
    const std::vector<std::shared_ptr<Task>>& tasks,
    int days) {

    WeeklyReport report;

    report.dailyStats = getDailyCompletedStats(tasks, days);
    report.streak = calculateStreak(tasks);
    report.categoryStats = getCategoryTimeDistribution(tasks);

    // Calcular totales
    report.totalTasksCompleted = 0;
    report.totalItemsChecked = 0;
    report.totalTimeSeconds = 0;
    report.totalPomodoros = 0;
    report.tasksCreated = 0;

    for (const auto& task : tasks) {
        report.tasksCreated++;
        if (task->isFinished()) {
            report.totalTasksCompleted++;
            for (size_t i = 0; i < task->itemCount(); ++i) {
                auto item = task->getItem(i);
                if (item && item->done()) {
                    report.totalItemsChecked++;
                }
            }
            report.totalTimeSeconds += task->actualDurationSeconds();
        }
        report.totalPomodoros += task->pomodoroCount();
    }

    return report;
}

} // namespace checklist