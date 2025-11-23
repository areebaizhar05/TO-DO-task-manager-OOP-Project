#include "RecurringSubTask.hpp"
#include "DateHelper.hpp"
#include <iostream>
#include <ctime>

using namespace std;

RecurringSubTask::RecurringSubTask(string name, int target, time_t dueDate)
    : SubTask(name, dueDate), completionCount(0), targetCount(target) {} // main constructor

bool RecurringSubTask::isRecurring() const {
    return true;} // this till be true for this type of tasks

string RecurringSubTask::getProgress() const {
    return to_string(completionCount) + "/" + to_string(targetCount);} // string output to terminal

void RecurringSubTask::display() const {
    string status = (completionCount >= targetCount) ? "✓ Completed!" : "○ In Progress";
    cout << status << " " << name << " [" << getPriorityString() << "] - " 
    << getProgress() << " (" << getRecurrenceString() << ")";}

void RecurringSubTask::incrementCompletion() { // increments occurances completed and if they reach teh max, mark the task completed
    if (completionCount < targetCount) {
        completionCount++;
        if (completionCount >= targetCount) {
            completed = true;
        }
        dueDate = getNextDueDate();
    }
}
 
time_t RecurringSubTask::getNextDueDate() const {
    return calculateNextDueDate();
}

int RecurringSubTask::getCompletionCount() const {
    return completionCount;
}

int RecurringSubTask::getTargetCount() const {
    return targetCount;
}

// daily: 
DailyRecurringSubTask::DailyRecurringSubTask(string name, int target, time_t dueDate)
    : RecurringSubTask(name, target, dueDate) {}

string DailyRecurringSubTask::getRecurrenceString() const {
    return "Daily";
}

time_t DailyRecurringSubTask::calculateNextDueDate() const {
    if (dueDate == 0) return 0;
    return dueDate + (1 * 24 * 60 * 60); // Simple: add 1 day in seconds
}


// weekly:
WeeklyRecurringSubTask::WeeklyRecurringSubTask(string name, int target, time_t dueDate)
    : RecurringSubTask(name, target, dueDate) {}

string WeeklyRecurringSubTask::getRecurrenceString() const {
    return "Weekly";
}

time_t WeeklyRecurringSubTask::calculateNextDueDate() const {
    if (dueDate == 0) return 0;
    return dueDate + (7 * 24 * 60 * 60); // Simple: add 7 days in seconds
}

// monthly:
MonthlyRecurringSubTask::MonthlyRecurringSubTask(string name, int target, time_t dueDate)
    : RecurringSubTask(name, target, dueDate) {}

string MonthlyRecurringSubTask::getRecurrenceString() const {
    return "Monthly";
}

time_t MonthlyRecurringSubTask::calculateNextDueDate() const {
    if (dueDate == 0) return 0;
    return dueDate + (30 * 24 * 60 * 60); // Simple: add 30 days in seconds
}

//yearly:
YearlyRecurringSubTask::YearlyRecurringSubTask(string name, int target, time_t dueDate)
    : RecurringSubTask(name, target, dueDate) {}

string YearlyRecurringSubTask::getRecurrenceString() const {
    return "Yearly";
}

time_t YearlyRecurringSubTask::calculateNextDueDate() const {
    if (dueDate == 0) return 0;
    return dueDate + (365 * 24 * 60 * 60); // Simple: add 365 days in seconds
}