#pragma once

#include <string>
#include <ctime>

namespace checklist {

struct Date {
    int year;
    int month;
    int day;

    Date() : year(1970), month(1), day(1) {}
    Date(int y, int m, int d) : year(y), month(m), day(d) {}

    bool operator==(const Date& other) const { return year == other.year && month == other.month && day == other.day; }
    bool operator!=(const Date& other) const { return !(*this == other); }
    bool operator<(const Date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    bool operator>(const Date& other) const { return other < *this; }
    bool operator<=(const Date& other) const { return !(*this > other); }
    bool operator>=(const Date& other) const { return !(*this < other); }

    std::string toString() const {
        char buf[11];
        snprintf(buf, sizeof(buf), "%04d-%02d-%02d", year, month, day);
        return std::string(buf);
    }

    static Date fromTimeT(std::time_t t) {
        std::tm* tm = std::localtime(&t);
        if (tm) {
            return Date(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday);
        }
        return Date();
    }

    int daysSinceEpoch() const {
        // Simplified: convert to time_t and calculate days
        std::tm tm = {};
        tm.tm_year = year - 1900;
        tm.tm_mon = month - 1;
        tm.tm_mday = day;
        tm.tm_isdst = -1;
        std::time_t t = std::mktime(&tm);
        if (t == -1) return 0;
        std::time_t epoch = 0;
        return static_cast<int>((t - epoch) / (24 * 60 * 60));
    }

    static Date today() {
        std::time_t now = std::time(nullptr);
        return fromTimeT(now);
    }
};

} // namespace checklist