#ifndef AUTHSYSTEM_HPP
#define AUTHSYSTEM_HPP

#include "User.hpp"
#include <string>
#include <map>
using namespace std;

class AuthSystem {
private:
    map<string, User*> userDatabase;
    User* currentUser;

public:
    AuthSystem();
    ~AuthSystem();

    bool registerUser();
    bool loginUser();
    bool isLoggedIn() const;
    User* getCurrentUser() const;
    void logout();

    // For JSON loading
    map<string, User*>& getUserDatabase() { return userDatabase; }
};

#endif