#ifndef JSONHANDLER_H
#define JSONHANDLER_H

#include "User.hpp"
#include "Task.cpp"  // Keep this as Task.cpp since that's where your class is
#include "SubTask.hpp"
#include <string>
#include <vector>
#include <map>

class JSONHandler {
private:
    static std::string trim(const std::string& str);
    static std::string getJSONValue(const std::string& json, const std::string& key);
    static std::vector<std::string> getJSONArray(const std::string& json, const std::string& key);

public:
    static bool loadFromJSON(const std::string& filename, std::map<std::string, User*>& userDatabase);
    static bool saveToJSON(const std::string& filename, const std::map<std::string, User*>& userDatabase);
    static User* parseUser(const std::string& userJSON);
    static void parseActivities(User* user, const std::string& userJSON);
};

#endif