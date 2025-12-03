#ifndef SUBTASK_H
#define SUBTASK_H

#include <string>
#include "DateHelper.hpp"

using namespace std;

enum class Priority { LOW, MEDIUM, HIGH };

class SubTask {
protected:
    string name;
    bool completed;
    Priority priority;

public:
    SubTask(string name);
    virtual ~SubTask() = default;

    void markCompleted();
    bool isCompleted() const;
    virtual string getName() const;
    Priority getPriority() const;
    string getPriorityString() const;
    

    virtual string getType() const = 0;
    virtual void display() const;
    virtual bool isRecurring() const = 0;
    virtual string getProgress() const = 0;

};

#endif