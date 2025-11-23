#ifndef SIMPLEDATEHELPER_H
#define SIMPLEDATEHELPER_H

#include <ctime>
#include <string>
using namespace std;
class SimpleDateHelper {
public:
    // convert days to time_t
    static time_t getDueDate(int daysFromNow) {
        if (daysFromNow <= 0) return 0;
        return time(0) + (daysFromNow * 86400); // 86400 seconds in a day
    }
    
    // Convert time_t to simple string
    static string getDateString(time_t date) {
        if (date == 0) return "No deadline";

        char slot[30];
        ctime_s(slot, sizeof(slot), &date);
        return string(slot);
    }
    
    // Check if task is overdue
    static bool isOverdue(time_t dueDate) {
        if (dueDate == 0) return false;
        return time(0) > dueDate;
    }
    
};

#endif