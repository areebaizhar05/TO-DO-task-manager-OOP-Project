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
    // Constructors
    Date() : day(0), month(0), year(0) {}  // Empty date
    Date(int d, int m, int y) : day(d), month(m), year(y) {}  // Full date
    
    // Getters - to access private data
    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }
    
    // Setters - to modify private data
    void setDay(int d) { day = d; }
    void setMonth(int m) { month = m; }
    void setYear(int y) { year = y; }
    
    // Check if date has been set
    bool isSet() const {
        return day != 0 && month != 0 && year != 0;
    }
    
    // Check if date is valid (basic validation)
    bool isValid() const {
        return (day > 0 && day <= 31 && 
                month > 0 && month <= 12 && 
                year > 1900);
    }
    
    // Convert date to readable string: DD/MM/YYYY
    string toString() const {
        if (!isSet()) return "No due date";
        
        stringstream ss;
        ss << setfill('0') << setw(2) << day << "/"
           << setfill('0') << setw(2) << month << "/"
           << year;
        return ss.str();
    }
    
    // Get today's date
    static Date today() {
        time_t now = time(nullptr);
        struct tm* timeinfo = localtime(&now);
        return Date(timeinfo->tm_mday, 
                   timeinfo->tm_mon + 1, 
                   timeinfo->tm_year + 1900);
    }
    
    // Add days to current date
    Date addDays(int numDays) const {
        if (!isSet()) return Date();
        
        // Convert date to seconds
        struct tm timeinfo = {};
        timeinfo.tm_mday = day;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_year = year - 1900;
        time_t dateInSeconds = mktime(&timeinfo);
        
        // Add days (in seconds: days * 24 hours * 60 min * 60 sec)
        dateInSeconds += (numDays * 86400);
        
        // Convert back to date
        struct tm* newTime = localtime(&dateInSeconds);
        return Date(newTime->tm_mday, 
                   newTime->tm_mon + 1, 
                   newTime->tm_year + 1900);
    }
    
    // Add weeks to current date
    Date addWeeks(int numWeeks) const {
        return addDays(numWeeks * 7);
    }
    
    // Add months to current date
    Date addMonths(int numMonths) const {
        if (!isSet()) return Date();
        
        struct tm timeinfo = {};
        timeinfo.tm_mday = day;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_year = year - 1900;
        
        // Add months
        timeinfo.tm_mon += numMonths;
        time_t newDate = mktime(&timeinfo);
        
        struct tm* newTime = localtime(&newDate);
        return Date(newTime->tm_mday, 
                   newTime->tm_mon + 1, 
                   newTime->tm_year + 1900);
    }
    
    // Add years to current date
    Date addYears(int numYears) const {
        if (!isSet()) return Date();
        return Date(day, month, year + numYears);
    }
    
    // How many days until this date?
    int daysUntil() const {
        if (!isSet()) return 0;
        
        // Get current time
        time_t now = time(nullptr);
        
        // Convert this date to seconds
        struct tm timeinfo = {};
        timeinfo.tm_mday = day;
        timeinfo.tm_mon = month - 1;
        timeinfo.tm_year = year - 1900;
        time_t dueTime = mktime(&timeinfo);
        
        // Calculate difference in seconds, then convert to days
        double diffSeconds = difftime(dueTime, now);
        return int(diffSeconds / 86400);
    }
    
    // Is this date in the past?
    bool isOverdue() const {
        if (!isSet()) return false;
        return daysUntil() < 0;
    }
};

#endif