#ifndef RECURRINGTASK_H
#define RECURRINGTASK_H

#include <string>
#include "SubTask.hpp"
using namespace std;


enum class RecurrenceType { DAILY, WEEKLY, MONTHLY, YEARLY }; // type of recurrence

class RecurringSubTask : public SubTask {
private:
    RecurrenceType recurrenceType;
    int completionCount; // how many times completed
    int targetCount; // total occurrences needed

public:
    RecurringSubTask(string name, RecurrenceType type, int target = 0, time_t dueDate = 0);

    string getType() const override;
    void display() const override;
    bool isRecurring() const override;
    string getProgress() const override;
    time_t getNextDueDate() const override;

    void incrementCompletion();
    RecurrenceType getRecurrenceType() const;
    string getRecurrenceString() const;
    int getCompletionCount() const;
    int getTargetCount() const;
};

#endif