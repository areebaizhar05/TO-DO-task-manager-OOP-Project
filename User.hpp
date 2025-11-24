#ifndef USER_HPP
#define USER_HPP

#include <string>
#include <vector>
#include "Task.cpp"
using namespace std;

class User {
private:
    string username;
    string password;
    string name;
    string email;
    string bio;
    vector<Task*> tasks;

public:
    User(string username, string name = "", string email = "");
    ~User();

    void createTask(string title, string category);
    void deleteTask(string title);
    void displayDashboard() const;
    void displayProfile() const;
    void updateProfile(string newName, string newEmail, string newBio);

    float getOverallProgress() const;
    int getCompletedTasksCount() const;
    Task* getTask(string title);
    vector<Task*> getTasksByCategory(string category) const;
    vector<string> getAllCategories() const;
    
    // Getters
    string getUsername() const { return username; }
    string getName() const { return name; }
    string getEmail() const { return email; }
    string getBio() const { return bio; }
    string getPassword() const { return password; }
    const vector<Task*>& getTasks() const { return tasks; }
    
    // Setters
    void setPassword(string pass) { password = pass; }
};

#endif