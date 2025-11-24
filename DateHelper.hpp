// DateHelper.hpp - KEEP THIS VERSION
#ifndef DATE_H
#define DATE_H

#include <string>
#include <ctime>
#include <sstream>
#include <iomanip>

using namespace std;

class Date {
private:
    int day;
    int month;
    int year;

public:
    Date() : day(0), month(0), year(0) {}
    Date(int d, int m, int y) : day(d), month(m), year(y) {}
    
    // All your existing methods stay the same
    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }
    
    bool isSet() const {
        return day != 0 && month != 0 && year != 0;
    }
    
    string toString() const {
        if (!isSet()) return "No due date";
        stringstream ss;
        ss << setfill('0') << setw(2) << day << "/"
           << setfill('0') << setw(2) << month << "/"
           << year;
        return ss.str();
    }
    
    static Date today() {
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        return Date(timeinfo->tm_mday, 
                   timeinfo->tm_mon + 1, 
                   timeinfo->tm_year + 1900);
    }
    
    Date addDays(int numDays) const {
        if (!isSet()) return Date();
        
        struct tm timeinfo = {};
        timeinfo.tm_mday = day;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_year = year - 1900;
        time_t dateInSeconds = mktime(&timeinfo);
        dateInSeconds += (numDays * 86400);
        
        struct tm* newTime = localtime(&dateInSeconds);
        return Date(newTime->tm_mday, 
                   newTime->tm_mon + 1, 
                   newTime->tm_year + 1900);
    }
    
    Date addWeeks(int numWeeks) const {
        return addDays(numWeeks * 7);
    }
    
    Date addMonths(int numMonths) const {
        if (!isSet()) return Date();
        
        struct tm timeinfo = {};
        timeinfo.tm_mday = day;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_year = year - 1900;
        timeinfo.tm_mon += numMonths;
        time_t newDate = mktime(&timeinfo);
        
        struct tm* newTime = localtime(&newDate);
        return Date(newTime->tm_mday, 
                   newTime->tm_mon + 1, 
                   newTime->tm_year + 1900);
    }
    
    Date addYears(int numYears) const {
        if (!isSet()) return Date();
        return Date(day, month, year + numYears);
    }
    
    int daysUntil() const {
        if (!isSet()) return 0;
        
        time_t now = time(nullptr);
        struct tm timeinfo = {};
        timeinfo.tm_mday = day;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_year = year - 1900;
        time_t dueTime = mktime(&timeinfo);
        
        double diffSeconds = difftime(dueTime, now);
        return int(diffSeconds / 86400);
    }
    
    bool isOverdue() const {
        if (!isSet()) return false;
        return daysUntil() < 0;
    }
};

#endif