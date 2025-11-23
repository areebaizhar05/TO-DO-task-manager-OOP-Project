#ifndef RECURRINGTASK_H
#define RECURRINGTASK_H

#include <string>
#include "SubTask.hpp"
using namespace std;

// Base Recurring Task class which will be inherited by 4 other classes
class RecurringSubTask : public SubTask {
protected:
    int completionCount; // how many times completed so far
    int targetCount; // how many times NEED to compelted in total
    virtual time_t calculateNextDueDate() const = 0; // everychild class will have its own duedate calculted

public:
    RecurringSubTask(string name, int target, time_t dueDate = 0);
    virtual ~RecurringSubTask() = default;

    bool isRecurring() const override; // this will be true  always for this type for task
    string getProgress() const override;
    void display() const override;

    time_t getNextDueDate() const override; // this is overrriden from the subtask
    
    virtual string getRecurrenceString() const = 0; // name of the type of reurrence
    
    void incrementCompletion(); // increases the completion count each time the task is done
    int getCompletionCount() const; // getter for completion count since this class will be inherited 
    int getTargetCount() const; // getter for target count since this will be inherited
};

// INHERTIED 4 TYPES OF RECURRING TASKS ( DAILY, WEEKLY, MONTHLY, YEARLY)
// Daily Recurring Task
class DailyRecurringSubTask : public RecurringSubTask {
    protected:
        time_t calculateNextDueDate() const override; 
public:
    DailyRecurringSubTask(string name, int target = 7, time_t dueDate = 0); // by default teh target will be set 7 but user CAN input number of days they want the task to repeat (basically acts like number of instances done for this sub task)
    
    string getRecurrenceString() const override; // it returns the name of the type of recurence (useful for cout options in main)
    time_t calculateNextDueDate() const override; // timeinfo
    // time_t getNextDueDate() const override;
};

// Weekly Recurring Task
class WeeklyRecurringSubTask : public RecurringSubTask {
public:
    WeeklyRecurringSubTask(string name, int target = 4, time_t dueDate = 0);
    
    string getRecurrenceString() const override;
    time_t calculateNextDueDate() const override;
    // time_t getNextDueDate() const override;
};

// Monthly Recurring Task
class MonthlyRecurringSubTask : public RecurringSubTask {
public:
    MonthlyRecurringSubTask(string name, int target = 12, time_t dueDate = 0);
    
    string getRecurrenceString() const override;
    time_t calculateNextDueDate() const override;
    // time_t getNextDueDate() const override;
};

// Yearly Recurring Task
class YearlyRecurringSubTask : public RecurringSubTask {
public:
    YearlyRecurringSubTask(string name, int target = 5, time_t dueDate = 0);
    
    string getRecurrenceString() const override;
    time_t calculateNextDueDate() const override;
    // time_t getNextDueDate() const override;
};

#endif