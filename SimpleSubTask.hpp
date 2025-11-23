#ifndef SIMPLETASK_H
#define SIMPLETASK_H

#include <string>
using namespace std;
#include "SubTask.hpp" 

class SimpleSubTask : public SubTask {
public:
    SimpleSubTask(string name, time_t dueDate = 0);

    string getType() const override;
    string getName() const override;
    void display() const override;
    bool isRecurring() const override;
    string getProgress() const override;
};

#endif
