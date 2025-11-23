#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>

enum class Priority { LOW, MEDIUM, HIGH };

class Task {
protected:
    std::string name;
    time_t dueDate;
    bool completed;
    Priority priority;

public:
    Task(std::string name, time_t dueDate = 0);
    virtual ~Task() = default;

    void markCompleted();
    bool isCompleted() const;
    virtual std::string getName() const;   // <-- made const
    Priority getPriority() const;
    std::string getPriorityString() const;
    void calculatePriority();

    virtual std::string getType() const = 0;
    virtual void display() const;
    virtual bool isRecurring() const;
    virtual std::string getProgress() const;
    virtual time_t getNextDueDate() const;

    time_t getDueDate() const { return dueDate; }
};

#endif
