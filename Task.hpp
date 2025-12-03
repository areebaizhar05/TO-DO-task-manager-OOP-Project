#ifndef TASK_HPP
#define TASK_HPP

#include <string>
#include <vector>
#include "SubTask.hpp"
#include "DateHelper.hpp"
using namespace std;

class Task {
private:
    string title; 
    vector<SubTask*> subTasks; 
    string category;
    int totalSubTasks = 0;
    int completedSubTasks = 0; 
    
    //just added these for the due date functionailty
    Date dueDate; 
    Priority priority;
    
public:
    // forward declaration cuz im using it in constructor
    void calculatePriority();
    
    // ADD THIS CONSTRUCTOR IMPLEMENTATION:
    Task(string title, string category = "general", Date dueDate = Date());
    ~Task();

    void addSubTask(SubTask* st); 
    void removeSubTask(int index);

    void display() const;
    void updateProgressCount();
    
    float getProgressPercent() const;
    string getProgressBar() const;
    
    string getTitle() const;
    string getCategory() const;
    int getSubTaskCount() const;
    int getCompletedCount();
    SubTask* getSubTask(int index);
    
    Date getDueDate() const;
    Priority getPriority() const;
    string getPriorityString() const;
    int daysUntilDue() const;
};

#endif // TASK_HPP