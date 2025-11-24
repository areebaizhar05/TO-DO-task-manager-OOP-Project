#include "AuthSystem.hpp"
#include <iostream>
using namespace std;

AuthSystem::AuthSystem() : currentUser(nullptr) {}

AuthSystem::~AuthSystem() {
    for (auto& pair : userDatabase) {
        delete pair.second;
    }
    userDatabase.clear();
}

bool AuthSystem::registerUser() {
    string username, password, name, email;

    cout << "\n========== REGISTRATION ==========\n";
    cout << "Enter username: ";
    getline(cin, username);

    if (userDatabase.find(username) != userDatabase.end()) {
        cout << "❌ Username already exists!\n";
        return false;
    }

    cout << "Enter password: ";
    getline(cin, password);

    cout << "Enter your name: ";
    getline(cin, name);

    cout << "Enter email: ";
    getline(cin, email);

    User* newUser = new User(username, name, email);
    newUser->setPassword(password);
    userDatabase[username] = newUser;

    cout << "✅ Registration successful!\n";
    return true;
}

bool AuthSystem::loginUser() {
    string username, password;

    cout << "\n========== LOGIN ==========\n";
    cout << "Enter username: ";
    getline(cin, username);

    cout << "Enter password: ";
    getline(cin, password);

    auto it = userDatabase.find(username);
    if (it != userDatabase.end() && it->second->getPassword() == password) {
        currentUser = it->second;
        return true;
    }

    cout << "❌ Invalid credentials!\n";
    return false;
}

bool AuthSystem::isLoggedIn() const {
    return currentUser != nullptr;
}

User* AuthSystem::getCurrentUser() const {
    return currentUser;
}

void AuthSystem::logout() {
    currentUser = nullptr;
}