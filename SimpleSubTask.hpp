#ifndef SIMPLETASK_H
#define SIMPLETASK_H

#include <string>
#include "SubTask.hpp"
#include "DateHelper.hpp"

using namespace std;

class SimpleSubTask : public SubTask {
public:
    SimpleSubTask(string name, Date dueDate = Date());

    string getType() const override;
    void display() const override;
    bool isRecurring() const override;
    string getProgress() const override;
};

#endif