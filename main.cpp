#include <iostream>
#include <string>
#include <vector>
#include "User.hpp"
#include "AuthSystem.hpp"
#include "Character.hpp"
#include "JSONHandler.hpp"
#include "SimpleSubTask.hpp"
using namespace std;


AuthSystem authSystem;
Character character("Moti");
User* currentUser = nullptr;

void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// Auth Menu
void authMenu() {
    int choice;
    do {
        clearScreen();
        cout << "\n========== AUTHENTICATION ==========" << endl;
        cout << "1. Login" << endl;
        cout << "2. Register" << endl;
        cout << "3. Exit" << endl;
        cout << "====================================" << endl;
        cout << "Enter your choice (1-3): ";
        cin >> choice;
        cin.ignore();

        switch(choice) {
            case 1:
                if (authSystem.loginUser()) {
                    currentUser = authSystem.getCurrentUser();
                    cout << "Login successful! Welcome, " << currentUser->getUsername() << "!" << endl;
                    return;
                }
                break;
            case 2:
                if (authSystem.registerUser()) {
                    cout << "Registration successful! Please login." << endl;
                }
                break;
            case 3:
                JSONHandler::saveToJSON("data.json", authSystem.getUserDatabase());
                exit(0);
        }
        
        cout << "\nPress Enter to continue...";
        cin.get();
    } while (choice != 3);
}

// View filtered tasks by category
void viewTasksByCategory() {
    auto categories = currentUser->getAllCategories();
    
    if (categories.empty()) {
        cout << "No categories found!" << endl;
        return;
    }
    
    cout << "\n========== CATEGORIES ==========\n";
    for (size_t i = 0; i < categories.size(); i++) {
        cout << i + 1 << ". " << categories[i] << endl;
    }
    cout << categories.size() + 1 << ". Back to all tasks\n";
    
    int choice;
    cout << "Select category: ";
    cin >> choice;
    cin.ignore();
    
    if (choice < 1 || choice > categories.size() + 1) return;
    if (choice == categories.size() + 1) return;
    
    string selectedCategory = categories[choice - 1];
    auto filtered = currentUser->getTasksByCategory(selectedCategory);
    
    cout << "\n========== " << selectedCategory << " Tasks ==========\n";
    for (size_t i = 0; i < filtered.size(); i++) {
        cout << i + 1 << ". " << filtered[i]->getTitle()
             << " - " << (int)filtered[i]->getProgressPercent() << "%" << endl;
    }
    
    cout << "\nPress Enter to continue...";
    cin.get();
}

// Mark subtask as complete
void markSubTaskComplete(Task* task) {
    task->display();
    
    if (task->getSubTaskCount() == 0) {
        cout << "No subtasks to complete!" << endl;
        return;
    }
    
    int choice;
    cout << "\nSelect subtask to mark complete (1-" << task->getSubTaskCount() << "): ";
    cin >> choice;
    cin.ignore();
    
    SubTask* subTask = task->getSubTask(choice);
    if (subTask) {
        subTask->markCompleted();
        task->updateProgressCount();
        cout << "Subtask marked as complete!" << endl;
    } else {
        cout << "Invalid selection!" << endl;
    }
}

// Add subtask to a task
void addSubTaskToTask(Task* task) {
    string name;
    cout << "Enter subtask name: ";
    getline(cin, name);
    
    SubTask* newSubTask = new SimpleSubTask(name);
    task->addSubTask(newSubTask);
}

// Task details menu
void taskDetailsMenu(Task* task) {
    int choice;
    do {
        clearScreen();
        task->display();
        
        cout << "\n========== TASK MENU ==========\n";
        cout << "1. Mark Subtask Complete\n";
        cout << "2. Add Subtask\n";
        cout << "3. Back\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();
        
        switch(choice) {
            case 1:
                markSubTaskComplete(task);
                break;
            case 2:
                addSubTaskToTask(task);
                break;
            case 3:
                return;
        }
        
        if (choice != 3) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 3);
}

// Dashboard menu
void dashboardMenu() {
    int choice;
    do {
        clearScreen();
        currentUser->displayDashboard();
        
        // Show character based on completed tasks
        float progress = currentUser->getOverallProgress();
        character.display(progress);
        
        cout << "\n========== DASHBOARD MENU ==========\n";
        cout << "1. Filter by Category\n";
        cout << "2. Select a Task\n";
        cout << "3. Add New Task\n";
        cout << "4. Delete Task\n";
        cout << "5. Back to Main Menu\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();
        
        switch(choice) {
            case 1:
                viewTasksByCategory();
                break;
            case 2: {
                auto tasks = currentUser->getTasks();
                if (tasks.empty()) {
                    cout << "No tasks available!" << endl;
                    break;
                }
                
                int taskChoice;
                cout << "Select task (1-" << tasks.size() << "): ";
                cin >> taskChoice;
                cin.ignore();
                
                if (taskChoice >= 1 && taskChoice <= tasks.size()) {
                    taskDetailsMenu(tasks[taskChoice - 1]);
                }
                break;
            }
            case 3: {
                string title, category;
                cout << "Enter task title: ";
                getline(cin, title);
                cout << "Enter category: ";
                getline(cin, category);
                currentUser->createTask(title, category);
                break;
            }
            case 4: {
                string title;
                cout << "Enter task title to delete: ";
                getline(cin, title);
                currentUser->deleteTask(title);
                break;
            }
            case 5:
                return;
        }
        
        if (choice != 5) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    } while (choice != 5);
}

// Main menu
void mainMenu() {
    int choice;
    do {
        clearScreen();
        cout << "Welcome, " << currentUser->getUsername() << "!" << endl;
        
        cout << "\n========== MAIN MENU ==========\n";
        cout << "1. View Dashboard\n";
        cout << "2. View/Edit Profile\n";
        cout << "3. Logout\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();
        
        switch(choice) {
            case 1:
                dashboardMenu();
                break;
            case 2:
                currentUser->displayProfile();
                cout << "\nPress Enter to continue...";
                cin.get();
                break;
            case 3:
                authSystem.logout();
                currentUser = nullptr;
                JSONHandler::saveToJSON("data.json", authSystem.getUserDatabase());
                return;
        }
    } while (choice != 3);
}

int main() {
    clearScreen();
    cout << "================================" << endl;
    cout << "          TASK TRACKER          " << endl;
    cout << "================================" << endl;
    
    JSONHandler::loadFromJSON("data.json", authSystem.getUserDatabase());
    
    cout << "\nPress Enter to start...";
    cin.get();
    
    while (true) {
        authMenu();
        if (currentUser) {
            mainMenu();
        }
    }
    
    return 0;
}