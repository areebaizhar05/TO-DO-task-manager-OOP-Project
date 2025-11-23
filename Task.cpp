#include <iostream>
#include <string>
#include <vector>
#include <SubTask.hpp>
using namespace std;

class Task {
private:
    string title; // set a title for your task to be called, eg OOP HW: it will have sub tasks in it like Qs1, project etc tc
    vector<SubTask*> subTasks; // one task is like a list, user can make many of these TASKS and tasks will have subtasks

    int totalSubTasks = 0;
    int completedSubTasks = 0; // these two calculations are to get the percentage of a task done so far for the emotes

public:
    Task(string title);
    ~Task();

    void addSubTask(SubTask* st){ 
        subTasks.push_back(st); //added the task to the list
    }

    void display() const;
    
    // NEW: Progress tracking
    void updateProgressCount(){
        totalSubTasks = subTasks.size(); 
        completedSubTasks = 0; // initally its 0

        for (const auto& st : subTasks){
            if (st->isCompleted())
                completedSubTasks++; // only then the progress count is updated
        }
    }                 
    float getProgressPercent() const{
        if (totalSubTasks ==0) return 100.0;
        return float(((completedSubTasks)/totalSubTasks)*100);  // 0.0-100.0 is range
    }           
    string getProgressBar() const{
        int percent = int(getProgressPercent()); 
        int filled = percent / 10; // 10 blocks =100%
        string bar = "[]";
        for (int i =0; i<10; i++){
            bar+= (i<filled) ? "█" : " ";}
        bar+= "]" +  to_string(percent) + "%";
        return bar;
    }
};
