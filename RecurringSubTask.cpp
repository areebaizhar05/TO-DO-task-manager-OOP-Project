#include "RecurringSubTask.hpp"
#include <iostream>

using namespace std;

// ============ Base RecurringSubTask Implementation ============
RecurringSubTask::RecurringSubTask(string name, int target, Date startDate)
    : SubTask(name, Date()), completionCount(0), targetCount(target) {
    // If no start date provided, use today
    this->startDate = startDate.isSet() ? startDate : Date::today();
    // Calculate and set the final due date automatically
    //this->dueDate = calculateFinalDueDate();
}

bool RecurringSubTask::isRecurring() const {
    return true;
}

string RecurringSubTask::getProgress() const {
    return to_string(completionCount) + "/" + to_string(targetCount);
}

void RecurringSubTask::display() const {
    string status = (completionCount >= targetCount) ? "✓ Completed!" : "○ In Progress";
    cout << status << " " << name << " [" << getPriorityString() << "] - "
         << getProgress() << " (" << getRecurrenceString() << ")";
    
    if (dueDate.isSet()) {
        cout << " (Final Due: " << dueDate.toString();
        if (dueDate.isOverdue() && completionCount < targetCount) {
            cout << " - OVERDUE!";
        }
        cout << ")";
    }
}

void RecurringSubTask::incrementCompletion() {
    if (completionCount < targetCount) {
        completionCount++;
        if (completionCount >= targetCount) {
            completed = true;
        }
    }
}

Date RecurringSubTask::getNextDueDate() const {
    return calculateNextDueDate();
}

Date RecurringSubTask::getFinalDueDate() const {
    return dueDate;
}

int RecurringSubTask::getCompletionCount() const {
    return completionCount;
}

int RecurringSubTask::getTargetCount() const {
    return targetCount;
}

// ============ DailyRecurringSubTask Implementation ============
DailyRecurringSubTask::DailyRecurringSubTask(string name, int target, Date startDate)
    : RecurringSubTask(name, target, startDate) {this->dueDate = calculateFinalDueDate();}

string DailyRecurringSubTask::getType() const {
    return "Daily Recurring Task";
}

string DailyRecurringSubTask::getRecurrenceString() const {
    return "Daily";
}

Date DailyRecurringSubTask::calculateNextDueDate() const {
    Date today = Date::today();
    return today.addDays(1);
}

Date DailyRecurringSubTask::calculateFinalDueDate() const {
    return startDate.addDays(targetCount);
}

// ============ WeeklyRecurringSubTask Implementation ============
WeeklyRecurringSubTask::WeeklyRecurringSubTask(string name, int target, Date startDate)
    : RecurringSubTask(name, target, startDate) {this->dueDate = calculateFinalDueDate();}

string WeeklyRecurringSubTask::getType() const {
    return "Weekly Recurring Task";
}

string WeeklyRecurringSubTask::getRecurrenceString() const {
    return "Weekly";
}

Date WeeklyRecurringSubTask::calculateNextDueDate() const {
    Date today = Date::today();
    return today.addWeeks(1);
}

Date WeeklyRecurringSubTask::calculateFinalDueDate() const {
    return startDate.addWeeks(targetCount);
}

// ============ MonthlyRecurringSubTask Implementation ============
MonthlyRecurringSubTask::MonthlyRecurringSubTask(string name, int target, Date startDate)
    : RecurringSubTask(name, target, startDate) {this->dueDate = calculateFinalDueDate();}

string MonthlyRecurringSubTask::getType() const {
    return "Monthly Recurring Task";
}

string MonthlyRecurringSubTask::getRecurrenceString() const {
    return "Monthly";
}

Date MonthlyRecurringSubTask::calculateNextDueDate() const {
    Date today = Date::today();
    return today.addMonths(1);
}

Date MonthlyRecurringSubTask::calculateFinalDueDate() const {
    return startDate.addMonths(targetCount);
}

// ============ YearlyRecurringSubTask Implementation ============
YearlyRecurringSubTask::YearlyRecurringSubTask(string name, int target, Date startDate)
    : RecurringSubTask(name, target, startDate) {this->dueDate = calculateFinalDueDate();}

string YearlyRecurringSubTask::getType() const {
    return "Yearly Recurring Task";
}

string YearlyRecurringSubTask::getRecurrenceString() const {
    return "Yearly";
}

Date YearlyRecurringSubTask::calculateNextDueDate() const {
    Date today = Date::today();
    return today.addYears(1);
}

Date YearlyRecurringSubTask::calculateFinalDueDate() const {
    return startDate.addYears(targetCount);
}