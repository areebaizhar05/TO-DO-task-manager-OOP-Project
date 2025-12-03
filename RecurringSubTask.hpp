#ifndef RECURRINGTASK_H
#define RECURRINGTASK_H

#include <string>
#include "SubTask.hpp"
#include "DateHelper.hpp"

using namespace std;

// Base Recurring Task class which will be inherited by 4 other classes
class RecurringSubTask : public SubTask {
protected:
    int completionCount; // how many times completed so far
    int targetCount; // how many times NEED to be completed in total
    Date startDate; // When the task started
    Date finalDueDate;
    virtual Date calculateNextDueDate() const = 0; // each child class will have its own duedate calculated
    virtual Date calculateFinalDueDate() const = 0; // auto-calculate final deadline

public:
    RecurringSubTask(string name, int target, Date startDate = Date());
    virtual ~RecurringSubTask() = default;

    bool isRecurring() const override; // this will be true always for this type of task
    string getProgress() const override;
    void display() const override;

    Date getNextDueDate() const; // this is overridden from the subtask
    Date getFinalDueDate() const;
    
    virtual string getRecurrenceString() const = 0; // name of the type of recurrence
    
    void incrementCompletion(); // increases the completion count each time the task is done
    int getCompletionCount() const; // getter for completion count since this class will be inherited
    int getTargetCount() const; // getter for target count since this will be inherited
};

// INHERITED 4 TYPES OF RECURRING TASKS (DAILY, WEEKLY, MONTHLY, YEARLY)

// Daily Recurring Task
class DailyRecurringSubTask : public RecurringSubTask {
protected:
    Date calculateNextDueDate() const override;
    Date calculateFinalDueDate() const override;

public:
    DailyRecurringSubTask(string name, int target = 7, Date startDate = Date());
    
    string getType() const override;
    string getRecurrenceString() const override;
};

// Weekly Recurring Task
class WeeklyRecurringSubTask : public RecurringSubTask {
protected:
    Date calculateNextDueDate() const override;
    Date calculateFinalDueDate() const override;

public:
    WeeklyRecurringSubTask(string name, int target = 4, Date startDate = Date());
    
    string getType() const override;
    string getRecurrenceString() const override;
};

// Monthly Recurring Task
class MonthlyRecurringSubTask : public RecurringSubTask {
protected:
    Date calculateNextDueDate() const override;
    Date calculateFinalDueDate() const override;

public:
    MonthlyRecurringSubTask(string name, int target = 12, Date startDate = Date());
    
    string getType() const override;
    string getRecurrenceString() const override;
};

// Yearly Recurring Task
class YearlyRecurringSubTask : public RecurringSubTask {
protected:
    Date calculateNextDueDate() const override;
    Date calculateFinalDueDate() const override;

public:
    YearlyRecurringSubTask(string name, int target = 5, Date startDate = Date());
    
    string getType() const override;
    string getRecurrenceString() const override;
};

#endif