#include "SubTask.hpp"
#include <iostream>

using namespace std;

SubTask::SubTask(string name, Date dueDate)
    : name(name), dueDate(dueDate), completed(false), priority(Priority::MEDIUM) {
    calculatePriority();
}

void SubTask::markCompleted() {
    completed = true;
    cout << "Task marked as completed!" << endl;
}

bool SubTask::isCompleted() const {
    return completed;
}

string SubTask::getName() const {
    return name;
}

Priority SubTask::getPriority() const {
    return priority;
}

string SubTask::getPriorityString() const {
    switch(priority) {
        case Priority::LOW: return "LOW";
        case Priority::MEDIUM: return "MEDIUM";
        case Priority::HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}

void SubTask::calculatePriority() {//incorporate this when Date helper stuff starts working
    if (!dueDate.isSet()) {
        priority = Priority::MEDIUM;
        return;
    }
    
    int daysUntilDue = dueDate.daysUntil();
    
    if (daysUntilDue < 0) {
        priority = Priority::HIGH; // Overdue
    } else if (daysUntilDue < 2) {
        priority = Priority::HIGH;
    } else if (daysUntilDue < 7) {
        priority = Priority::MEDIUM;
    } else {
        priority = Priority::LOW;
    }
}

void SubTask::display() const {
    string status = completed ? "✓" : "✗";
    cout << status << " " << name << " [" << getPriorityString() << "]";
    
    if (dueDate.isSet()) {
        cout << " (Due: " << dueDate.toString() << ")";
    }
}

Date SubTask::getNextDueDate() const {
    return dueDate;
}

Date SubTask::getDueDate() const {
    return dueDate;
}

void SubTask::setDueDate(Date date) {
    dueDate = date;
    calculatePriority();
}