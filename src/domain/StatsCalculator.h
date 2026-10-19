#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>

#include "Date.h"
#include "Task.h"

namespace checklist {

struct DailyStat {
    std::string date;      // yyyy-MM-dd
    int tasksCompleted;
    int itemsChecked;
    int timeSpentSeconds;  // tiempo real invertido ese día
};

struct CategoryTimeStat {
    std::string category;
    int totalTimeSeconds;
    int taskCount;
};

struct ProductivityStreak {
    int currentStreak;     // días consecutivos con tareas completadas
    int maxStreak;         // racha máxima histórica
    int daysActive;        // días totales con al menos una tarea completada
};

struct WeeklyReport {
    int totalTasksCompleted;
    int totalItemsChecked;
    int totalTimeSeconds;
    int totalPomodoros;
    int tasksCreated;
    std::vector<DailyStat> dailyStats;
    std::vector<CategoryTimeStat> categoryStats;
    ProductivityStreak streak;
};

class StatsCalculator {
public:
    static WeeklyReport generateWeeklyReport(
        const std::vector<std::shared_ptr<Task>>& tasks,
        int days = 7);

    static std::vector<DailyStat> getDailyCompletedStats(
        const std::vector<std::shared_ptr<Task>>& tasks,
        int days = 7);

    static ProductivityStreak calculateStreak(
        const std::vector<std::shared_ptr<Task>>& tasks);

    static std::vector<CategoryTimeStat> getCategoryTimeDistribution(
        const std::vector<std::shared_ptr<Task>>& tasks);

private:
    static Date getDateFromString(const std::string& dateStr);
    static std::string dateToString(const Date& date);
};

} // namespace checklist