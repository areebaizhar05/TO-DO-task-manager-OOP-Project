#include "SimpleSubTask.hpp"
#include <iostream>

using namespace std;

SimpleTask::SimpleTask(string name, time_t dueDate): Task(name, dueDate) {}

string SimpleTask::getType() const {
    return "Single Task";
}

void SimpleTask::display() const {
    string status = completed ? "✓" : "✗";
    cout << status << " " << getName() << " [" << getPriorityString() << "]";
}

bool SimpleTask::isRecurring() const {
    return false;
}

std::string SimpleTask::getProgress() const {
    return completed ? "1/1" : "0/1";
}