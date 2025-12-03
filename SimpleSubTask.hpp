#ifndef SIMPLETASK_H
#define SIMPLETASK_H

#include <string>
#include "SubTask.hpp"
//#include "DateHelper.hpp" indirectly inclyded through subtask.hpp

using namespace std;

class SimpleSubTask : public SubTask {
public:
    SimpleSubTask(string name);

    string getType() const override;
    void display() const override;
    bool isRecurring() const override;
    string getProgress() const override;
};

#endif