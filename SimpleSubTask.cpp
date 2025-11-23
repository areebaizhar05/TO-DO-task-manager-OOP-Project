#include "SimpleSubTask.hpp"
#include <iostream>

using namespace std;

SimpleSubTask::SimpleSubTask(string name, Date dueDate) 
    : SubTask(name, dueDate) {}

string SimpleSubTask::getType() const {
    return "One-time Task";
}

void SimpleSubTask::display() const {
    string status = completed ? "✓ Completed!" : "✗ Pending";
    cout << status << " " << name << " [" << getPriorityString() << "]";
    
    if (dueDate.isSet()) {
        cout << " (Due: " << dueDate.toString();
        if (dueDate.isOverdue() && !completed) {
            cout << " - OVERDUE!";
        }
        cout << ")";
    }
}

bool SimpleSubTask::isRecurring() const {
    return false;
}

string SimpleSubTask::getProgress() const {
    return completed ? "1/1" : "0/1";
}