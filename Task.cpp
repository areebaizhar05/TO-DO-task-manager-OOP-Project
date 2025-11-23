#include <iostream>
#include <string>
#include <vector>
#include <SubTask.hpp>
using namespace std;

// SubTask (abstract base)
    // ├── SimpleSubTask (one-time tasks)
    // └── RecurringSubTask (abstract base for recurring)
            // ├── DailyRecurringSubTask
            // ├── WeeklyRecurringSubTask
            // ├── MonthlyRecurringSubTask
            // └── YearlyRecurringSubTask

class Task {
private:
    string title; // set a title for your task to be called, eg OOP HW: it will have sub tasks in it like Qs1, project etc tc
    vector<SubTask*> subTasks; // one task is like a list, user can make many of these TASKS and tasks will have subtasks

    int totalSubTasks = 0;
    int completedSubTasks = 0; // these two calculations are to get the percentage of a task done so far for the emotes

public:
    Task(string title);
    ~Task(){
        for (auto st: subTasks){
            delete st; // saving memory leak
        }
        subTasks.clear();}

    void addSubTask(SubTask* st){ 
        subTasks.push_back(st); //added the task to the list
        updateProgressCount(); // because the progress count will change
    }
    void removeSubTask(int index){
        if (index < 1 || index > subTasks.size()) {
        cout << "Invalid number! Try again.\n";
        return;}

    // Convert to 0 index order
    int realIndex = index - 1;
    SubTask* toDelete = subTasks[realIndex];

    cout << "Deleted: " << toDelete->getName() << endl;
    delete toDelete;
    subTasks.erase(subTasks.begin() + realIndex);
    updateProgressCount();
    }



    void display() const{
        cout << "\n=== " << title << " ===" << endl;
        cout << getProgressBar() << endl;

        if (subTasks.empty()) {
            cout << " No Subtasks!\n";
            return;
        }

        for (size_t i = 0; i < subTasks.size(); ++i) {
            cout << "  " << (i + 1) << ". ";
            subTasks[i]->display();
        }}
    
    void updateProgressCount(){ // keep track of subtasks done
        totalSubTasks = subTasks.size(); 
        completedSubTasks = 0; // initally its 0

        for (const auto& st : subTasks){
            if (st->isCompleted())
                completedSubTasks++; // only then the progress count is updated
        }
    }                 
    float getProgressPercent() const{
        if (totalSubTasks ==0) return 100.0;
        return (float(completedSubTasks) / totalSubTasks) * 100;  // 0.0-100.0 is range
    }           
    string getProgressBar() const{
        int percent = int(getProgressPercent()); 
        int filled = percent / 10; // 10 blocks =100%
        string bar = "[";
        for (int i =0; i<10; i++){
            bar+= (i<filled) ? "█" : " ";}
        bar+= "]" +  to_string(percent) + "%";
        return bar;
    }
    string getTitle() const {
        return title;} //getter
    
    int getSubTaskCount() const{
        return subTasks.size();} // getter
    
    int getCompletedCount() {
        updateProgressCount();
        return completedSubTasks;}
    
    SubTask* getSubTask(int index) {
    if (index < 1 || index > subTasks.size()) {
        return nullptr;
    }
    return subTasks[index - 1]; // because count starts with 1
}
};
