#include "SimpleSubTask.hpp"
#include <iostream>
#include <string>

using namespace std;

SimpleSubTask::SimpleSubTask(string name, time_t dueDate): SubTask(name, dueDate) {}

string SimpleSubTask::getType() const {
    return "One time Task";
}

void SimpleSubTask::display() const {
    string status = completed ? "✓ Completed!" : "✗ Pending "; // for terminal display
    cout << status << " " << name << " [" << getPriorityString() << "]";
}

bool SimpleSubTask::isRecurring() const { // non recurring tasks
    return false;
}

string SimpleSubTask::getProgress() const {

    return completed ? "1/1" : "0/1";
}