#ifndef TASK_H
#define TASK_H
using namespace std;
#include <string>
#include <ctime>


// THIS IS AN ABSTRACT CLASS
enum class Priority { LOW, MEDIUM, HIGH };

class SubTask {
protected:
    std::string name;
    time_t dueDate;
    bool completed;
    Priority priority;

public:
    SubTask(std::string name, time_t dueDate = 0);
    virtual ~SubTask() = default;

    void markCompleted();
    bool isCompleted() const;
    virtual string getName() const;   // <-- made const
    Priority getPriority() const;
    string getPriorityString() const;
    void calculatePriority();

    virtual string getType() const = 0;
    virtual void display() const;
    virtual bool isRecurring() const = 0; // all these will be overridden in the recurring and simple tasks
    virtual string getProgress() const = 0; 
    virtual time_t getNextDueDate() const; // when is due date for a subtask

    time_t getDueDate() const { return dueDate; }
    void setDueDate(time_t date) { dueDate = date;}
};

#endif
