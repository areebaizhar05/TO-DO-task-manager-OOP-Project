#ifndef TASK_H
#define TASK_H

#include <string>
#include "DateHelper.hpp"

using namespace std;

// THIS IS AN ABSTRACT CLASS
enum class Priority { LOW, MEDIUM, HIGH };

class SubTask {
protected:
    string name;
    Date dueDate;  // Changed from time_t to Date
    bool completed;
    Priority priority;

public:
    SubTask(string name, Date dueDate = Date());
    virtual ~SubTask() = default;

    void markCompleted();
    bool isCompleted() const;
    virtual string getName() const;
    Priority getPriority() const;
    string getPriorityString() const;
    void calculatePriority();

    virtual string getType() const = 0;
    virtual void display() const;
    virtual bool isRecurring() const = 0;
    virtual string getProgress() const = 0;
    virtual Date getNextDueDate() const;

    Date getDueDate() const { return dueDate; }
    void setDueDate(Date date) { dueDate = date; }
    string getDueDateString() const { return dueDate.toString(); }
};

#endif