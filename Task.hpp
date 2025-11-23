#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>
#include <vector>
using namespace std;

// creating task as an abstract class bcz we have subdivisions of types of tasks
enum class Priority{LOW , MEDIUM, HIGH}; // enum is where we have fixed attribute to output

class Task{ // abstract class
    protected:
        string name;
        time_t dueDate;
        bool completed;
        Priority priority;
    
        public:
            Task(string name, time_t dueDate = 0);
            virtual ~Task() = default;

            void markCompleted();
            bool isCompleted() const;
            string getName();
            Priority getPriority() const;
            string getPriorityString() const;
            void calculatePriority();

            virtual string getType() const = 0;  // Pure virtual
            virtual void display() const;
            virtual bool isRecurring() const;
            virtual string getProgress() const;
            virtual time_t getNextDueDate() const;  // For recurring tasks
            
            //getter 
            time_t getDueDate() const { return dueDate; }


    };

    #endif