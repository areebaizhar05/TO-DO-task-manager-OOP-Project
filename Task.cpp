#ifndef TASK_CPP
#define TASK_CPP//header guards since we're not using a seperate hpp file

#include <iostream>
#include <string>
#include <vector>
#include "DateHelper.hpp" 
#include "SubTask.hpp"
#include "Task.hpp" // MUST include the class definition header
using namespace std;

// SubTask (abstract base)
    // ├── SimpleSubTask (one-time tasks)
    // └── RecurringSubTask (abstract base for recurring)
    //             // ├── DailyRecurringSubTask
    //             // ├── WeeklyRecurringSubTask
    //             // ├── MonthlyRecurringSubTask
    //             // └── YearlyRecurringSubTask


// IMPLEMENTATION OF CONSTRUCTOR (Moved from class body)
Task::Task(string title, string category, Date dueDate) 
    : title(title), category(category), totalSubTasks(0), completedSubTasks(0), dueDate(dueDate) {
    calculatePriority(); // NEW: Calculate priority right after creation
}

// IMPLEMENTATION OF DESTRUCTOR
Task::~Task(){
    for (auto st: subTasks){
        delete st; // saving memory leak
    }
    subTasks.clear();
}

// IMPLEMENTATION OF addSubTask
void Task::addSubTask(SubTask* st){ 
    subTasks.push_back(st); //added the task to the list
    updateProgressCount(); // because the progress count will change
}

// IMPLEMENTATION OF removeSubTask
void Task::removeSubTask(int index){
    if (index < 1 || index > subTasks.size()) {
        cout << "Invalid number! Try again.\n";
        return;
    }
    
    // Convert to 0 index order
    int realIndex = index - 1;
    SubTask* toDelete = subTasks[realIndex];

    cout << "Deleted: " << toDelete->getName() << endl;
    delete toDelete;
    subTasks.erase(subTasks.begin() + realIndex);
    updateProgressCount();
}

// IMPLEMENTATION OF display
void Task::display() const {
    cout << "\n=== " << title << " ===" << endl;
    cout << getProgressBar() << endl;

    if (subTasks.empty()) {
        cout << " No Subtasks!\n";
        return;
    }

    for (size_t i = 0; i < subTasks.size(); ++i) {
        cout << "  " << (i + 1) << ". ";
        subTasks[i]->display();
        cout << endl;
    }
}

// IMPLEMENTATION OF updateProgressCount
void Task::updateProgressCount(){ // keep track of subtasks done
    totalSubTasks = subTasks.size(); 
    completedSubTasks = 0; // initally its 0

    for (const auto& st : subTasks){
        if (st->isCompleted())
            completedSubTasks++; // only then the progress count is updated
    }
} 

// IMPLEMENTATION OF getProgressPercent
float Task::getProgressPercent() const{
    if (totalSubTasks ==0) return 100.0;
    return (float(completedSubTasks) / totalSubTasks) * 100;  // 0.0-100.0 is range
} 

// IMPLEMENTATION OF getProgressBar
string Task::getProgressBar() const{
    int percent = (int)getProgressPercent(); 
    int filled = percent / 10; // 10 blocks =100%
    string bar = "[";
    for (int i =0; i<10; i++){
        bar+= (i<filled) ? "█" : " ";}
    bar+= "]" +  to_string(percent) + "%";
    return bar;
}

// IMPLEMENTATION OF getTitle
string Task::getTitle() const {
    return title;} //getter

// IMPLEMENTATION OF getCategory
string Task::getCategory() const { return category; }

// IMPLEMENTATION OF getSubTaskCount
int Task::getSubTaskCount() const{
    return subTasks.size();} // getter

// IMPLEMENTATION OF getCompletedCount
int Task::getCompletedCount() {
    updateProgressCount();
    return completedSubTasks;}

// IMPLEMENTATION OF getSubTask
SubTask* Task::getSubTask(int index) {
    if (index < 1 || index > subTasks.size()) {
        return nullptr;
    }
    return subTasks[index - 1]; // because count starts with 1
}

// IMPLEMENTATION OF getDueDate
Date Task::getDueDate() const { return dueDate; }

// IMPLEMENTATION OF getPriority
Priority Task::getPriority() const { return priority; }

// IMPLEMENTATION OF getPriorityString
string Task::getPriorityString() const {
    switch(priority) {
        case Priority::LOW: return "LOW";
        case Priority::MEDIUM: return "MEDIUM";
        case Priority::HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}

// IMPLEMENTATION OF daysUntilDue
int Task::daysUntilDue() const {
    if (!dueDate.isSet()) return 999; // Far off
    return dueDate.daysUntil();
}


// IMPLEMENTATION OF calculatePriority (moving it to globalscope so we can use it in sfml app too)
void Task::calculatePriority() {
    if (!this->dueDate.isSet()) {
        this->priority = Priority::MEDIUM;
        return;
    }
    
    int daysUntilDue = this->dueDate.daysUntil();
    
    if (daysUntilDue < 0) {
        this->priority = Priority::HIGH; // Overdue
    } else if (daysUntilDue < 2) {
        this->priority = Priority::HIGH;
    } else if (daysUntilDue < 7) {
        this->priority = Priority::MEDIUM;
    } else {
        this->priority = Priority::LOW;
    }
}
#endif