#include "User.hpp"
#include <iostream>
#include <set>
using namespace std;

User::User(string username, string name, string email)
    : username(username), name(name), email(email), bio("") {}

User::~User() {
    for (auto task : tasks) {
        delete task;
    }
    tasks.clear();
}

void User::createTask(string title, string category) {
    Task* newTask = new Task(title, category);
    tasks.push_back(newTask);//push new task in verctor
    cout << "Task '" << title << "' created successfully!" << endl;
}

void User::deleteTask(string title) {
    for (size_t i = 0; i < tasks.size(); i++) {
        if (tasks[i]->getTitle() == title) {
            delete tasks[i];
            tasks.erase(tasks.begin() + i);
            cout << "Task deleted successfully!" << endl;
            return;
        }
    }
    cout << "Task not found!" << endl;
}

void User::displayDashboard() const {
    cout << "\n+------------------------------------+" << endl;
    cout << "|          USER DASHBOARD            |" << endl;
    cout << "+------------------------------------+" << endl;
    cout << "Username: " << username << endl;
    cout << "Overall Progress: " << (int)getOverallProgress() << "%" << endl;
    cout << "Completed Tasks: " << getCompletedTasksCount() << "/" << tasks.size() << endl;
    cout << "------------------------------------" << endl;

    if (tasks.empty()) {
        cout << "No tasks yet. Create one to get started:)" << endl;
    } else {
        cout << "\nYour Tasks:\n" << endl;
        for (size_t i = 0; i < tasks.size(); i++) {
            cout << i + 1 << ". " << tasks[i]->getTitle()
                 << " [" << tasks[i]->getCategory() << "]"
                 << " - " << (int)tasks[i]->getProgressPercent() << "% complete "
                 << "(" << tasks[i]->getCompletedCount() << "/" 
                 << tasks[i]->getSubTaskCount() << " subtasks)" << endl;
        }
    }
}


void User::displayProfile() const {
    cout << "\n========== USER PROFILE ==========\n";
    cout << "Username: " << username << endl;
    cout << "Name: " << (name.empty() ? "Not set" : name) << endl;
    cout << "Email: " << (email.empty() ? "Not set" : email) << endl;
    cout << "Bio: " << (bio.empty() ? "Not set" : bio) << endl;
    cout << "Tasks: " << tasks.size() << endl;
    cout << "==================================\n";
}

void User::updateProfile(string newName, string newEmail, string newBio) {//not tested yet in current main
    name = newName;
    email = newEmail;
    bio = newBio;
    cout << "Profile updated successfully!" << endl;
}

float User::getOverallProgress() const {//avg btw the progress of all tasks
    if (tasks.empty()) return 0.0f;

    float totalProgress = 0.0f;
    for (const auto& task : tasks) {
        totalProgress += task->getProgressPercent();
    }

    return totalProgress / tasks.size();
}

int User::getCompletedTasksCount() const {
    int count = 0;
    for (const auto& task : tasks) {
        if (task->getProgressPercent() == 100.0f) {
            count++;
        }
    }
    return count;
}

Task* User::getTask(string title) {
    for (auto task : tasks) {
        if (task->getTitle() == title) {
            return task;
        }
    }
    return nullptr;
}

vector<Task*> User::getTasksByCategory(string category) const {
    vector<Task*> filtered;
    for (auto task : tasks) {
        if (task->getCategory() == category) {
            filtered.push_back(task);
        }
    }
    return filtered;
}

vector<string> User::getAllCategories() const {
    set<string> categorySet;
    for (auto task : tasks) {
        categorySet.insert(task->getCategory());
    }
    return vector<string>(categorySet.begin(), categorySet.end());
}