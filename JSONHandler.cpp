#include "JSONHandler.hpp"
#include "SimpleSubTask.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

std::string JSONHandler::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    return str.substr(start, end - start + 1);
}

std::string JSONHandler::getJSONValue(const std::string& json, const std::string& key) {
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return "";
    
    pos += search.length();
    size_t start = json.find_first_of("\"", pos);
    if (start == std::string::npos) return "";
    
    size_t end = json.find_first_of("\"", start + 1);
    if (end == std::string::npos) return "";
    
    return json.substr(start + 1, end - start - 1);
}

// === THIS IS THE FIXED FUNCTION ===
std::vector<std::string> JSONHandler::getJSONArray(const std::string& json, const std::string& key) {
    std::vector<std::string> result;
    std::string search = "\"" + key + "\":";
    size_t pos = json.find(search);
    if (pos == std::string::npos) return result;
    
    pos += search.length();
    size_t start = json.find("[", pos);
    if (start == std::string::npos) return result;
    
    // NEW LOGIC: Count brackets to handle nested arrays (like task_list inside activities)
    size_t end = start;
    int bracketCount = 0;
    bool foundEnd = false;

    for (size_t i = start; i < json.length(); i++) {
        if (json[i] == '[') bracketCount++;
        if (json[i] == ']') bracketCount--;
        
        if (bracketCount == 0) {
            end = i;
            foundEnd = true;
            break;
        }
    }
    
    if (!foundEnd) return result;
    
    std::string arrayStr = json.substr(start + 1, end - start - 1);
    
    // Split items by comma, respecting curly braces {}
    size_t itemStart = 0;
    int braceCount = 0;
    
    for (size_t i = 0; i < arrayStr.length(); i++) {
        if (arrayStr[i] == '{') braceCount++;
        if (arrayStr[i] == '}') braceCount--;
        
        if (braceCount == 0 && arrayStr[i] == ',') {
            std::string item = arrayStr.substr(itemStart, i - itemStart);
            result.push_back(trim(item));
            itemStart = i + 1;
        }
    }
    
    if (itemStart < arrayStr.length()) {
        std::string item = arrayStr.substr(itemStart);
        result.push_back(trim(item));
    }
    
    return result;
}

bool JSONHandler::loadFromJSON(const std::string& filename, std::map<std::string, User*>& userDatabase) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "⚠️ Could not open JSON file: " << filename << " (creating new database)\n";
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    file.close();
    
    std::vector<std::string> usersArray = getJSONArray(jsonContent, "users");
    
    for (const auto& userJSON : usersArray) {
        User* user = parseUser(userJSON);
        if (user) {
            std::string lowerKey = user->getUsername();
            std::transform(lowerKey.begin(), lowerKey.end(), lowerKey.begin(), ::tolower);
            userDatabase[lowerKey] = user;
            parseActivities(user, userJSON);
        }
    }
    
    std::cout << "✅ Loaded " << userDatabase.size() << " users from JSON\n";
    return true;
}

User* JSONHandler::parseUser(const std::string& userJSON) {
    std::string username = getJSONValue(userJSON, "username");
    std::string password = getJSONValue(userJSON, "password");
    std::string name = getJSONValue(userJSON, "name");
    std::string email = getJSONValue(userJSON, "email");
    std::string bio = getJSONValue(userJSON, "bio");
    
    if (username.empty()) {
        return nullptr;
    }
    
    User* user = new User(username, name, email);
    user->setPassword(password);
    if (!bio.empty()) {
        user->updateProfile(name, email, bio);
    }
    
    return user;
}

void JSONHandler::parseActivities(User* user, const std::string& userJSON) {
    std::vector<std::string> activitiesArray = getJSONArray(userJSON, "activities");
    
    for (const auto& activityJSON : activitiesArray) {
        std::string taskName = getJSONValue(activityJSON, "name");
        std::string category = getJSONValue(activityJSON, "category");
        
        if (!taskName.empty()) {
            user->createTask(taskName, category);
            Task* task = user->getTask(taskName);
            
            if (task) {
                std::vector<std::string> subtasksArray = getJSONArray(activityJSON, "task_list");
                
                for (const auto& subTaskJSON : subtasksArray) {
                    std::string subTaskTitle = getJSONValue(subTaskJSON, "task");
                    std::string status = getJSONValue(subTaskJSON, "status");
                    
                    if (!subTaskTitle.empty()) {
                        SubTask* subTask = new SimpleSubTask(subTaskTitle);
                        if (status == "complete") {
                            subTask->markCompleted();
                        }
                        task->addSubTask(subTask);
                    }
                }
                task->updateProgressCount(); 
            }
        }
    }
}

bool JSONHandler::saveToJSON(const std::string& filename, const std::map<std::string, User*>& userDatabase) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cout << "❌ Could not save to JSON file: " << filename << "\n";
        return false;
    }
    
    file << "{\n";
    file << "    \"users\": [\n";
    
    bool firstUser = true;
    for (const auto& pair : userDatabase) {
        User* user = pair.second;
        
        if (!firstUser) {
            file << ",\n";
        }
        firstUser = false;
        
        file << "        {\n";
        file << "            \"username\": \"" << user->getUsername() << "\",\n";
        file << "            \"password\": \"" << user->getPassword() << "\",\n";
        file << "            \"name\": \"" << user->getName() << "\",\n";
        file << "            \"email\": \"" << user->getEmail() << "\",\n";
        file << "            \"bio\": \"" << user->getBio() << "\",\n";
        file << "            \"activities\": [\n";
        
        auto tasks = user->getTasks();
        bool firstTask = true;
        
        for (auto task : tasks) {
            if (!firstTask) {
                file << ",\n";
            }
            firstTask = false;
            
            file << "                {\n";
            file << "                    \"name\": \"" << task->getTitle() << "\",\n";
            file << "                    \"category\": \"" << task->getCategory() << "\",\n";
            file << "                    \"task_list\": [\n";
            
            bool firstSubTask = true;
            for (int i = 1; i <= task->getSubTaskCount(); i++) {
                SubTask* subTask = task->getSubTask(i);
                
                if (!firstSubTask) {
                    file << ",\n";
                }
                firstSubTask = false;
                
                file << "                        { \"task\": \"" << subTask->getName() 
                     << "\", \"status\": \"" 
                     << (subTask->isCompleted() ? "complete" : "incomplete") << "\" }";
            }
            
            file << "\n                    ],\n";
            file << "                    \"progress\": " << static_cast<int>(task->getProgressPercent()) << "\n";
            file << "                }";
        }
        
        file << "\n            ]\n";
        file << "        }";
    }
    
    file << "\n    ]\n";
    file << "}\n";
    
    file.close();
    std::cout << "✅ Saved " << userDatabase.size() << " users to JSON\n";
    return true;
}