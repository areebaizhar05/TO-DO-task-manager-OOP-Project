

//IGNORE THIS FILE ITS TO BE DELETED LATER

//TEST FILE TO SEE FUNCTIONALITY IN SFML
//WONT BE USED LATER


#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <algorithm>

struct Task {
    std::string task;
    std::string status;
};

struct Activity {
    std::string name;
    std::string category;
    std::vector<Task> task_list;
    int progress;
};

struct User {
    std::string username;
    std::string password;
    std::string name;
    std::string email;
    std::string bio;
    std::vector<Activity> activities;
};

class JSONManager {
public:
    static std::vector<User> loadUsers(const std::string& filename) {
        std::vector<User> users;
        std::ifstream file(filename);
        
        if (!file.is_open()) {
            std::cerr << "Could not open file: " << filename << std::endl;
            return users;
        }

        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();

        // Simple JSON parsing
        size_t usersPos = content.find("\"users\"");
        if (usersPos == std::string::npos) return users;

        size_t startArray = content.find('[', usersPos);
        size_t endArray = content.rfind(']');

        std::string usersContent = content.substr(startArray + 1, endArray - startArray - 1);

        // Parse each user object
        size_t userStart = 0;
        while ((userStart = usersContent.find('{', userStart)) != std::string::npos) {
            size_t userEnd = usersContent.find('}', userStart);
            std::string userStr = usersContent.substr(userStart, userEnd - userStart + 1);

            User user;
            user.username = extractJsonString(userStr, "username");
            user.password = extractJsonString(userStr, "password");
            user.name = extractJsonString(userStr, "name");
            user.email = extractJsonString(userStr, "email");
            user.bio = extractJsonString(userStr, "bio");

            // Parse activities
            size_t activitiesPos = userStr.find("\"activities\"");
            if (activitiesPos != std::string::npos) {
                size_t actStart = userStr.find('[', activitiesPos);
                size_t actEnd = userStr.rfind(']', userEnd);
                std::string actContent = userStr.substr(actStart + 1, actEnd - actStart - 1);

                size_t actObjStart = 0;
                while ((actObjStart = actContent.find('{', actObjStart)) != std::string::npos) {
                    size_t actObjEnd = actContent.find('}', actObjStart);
                    std::string actStr = actContent.substr(actObjStart, actObjEnd - actObjStart + 1);

                    Activity activity;
                    activity.name = extractJsonString(actStr, "name");
                    activity.category = extractJsonString(actStr, "category");
                    activity.progress = extractJsonInt(actStr, "progress");

                    // Parse task list
                    size_t taskListPos = actStr.find("\"task_list\"");
                    if (taskListPos != std::string::npos) {
                        size_t tStart = actStr.find('[', taskListPos);
                        size_t tEnd = actStr.rfind(']');
                        std::string taskContent = actStr.substr(tStart + 1, tEnd - tStart - 1);

                        size_t taskObjStart = 0;
                        while ((taskObjStart = taskContent.find('{', taskObjStart)) != std::string::npos) {
                            size_t taskObjEnd = taskContent.find('}', taskObjStart);
                            std::string taskStr = taskContent.substr(taskObjStart, taskObjEnd - taskObjStart + 1);

                            Task t;
                            t.task = extractJsonString(taskStr, "task");
                            t.status = extractJsonString(taskStr, "status");
                            activity.task_list.push_back(t);

                            taskObjStart = taskObjEnd + 1;
                        }
                    }

                    user.activities.push_back(activity);
                    actObjStart = actObjEnd + 1;
                }
            }

            users.push_back(user);
            userStart = userEnd + 1;
        }

        return users;
    }

    static void saveUsers(const std::string& filename, const std::vector<User>& users) {
        std::ofstream file(filename);
        file << "{\n    \"users\": [\n";

        for (size_t i = 0; i < users.size(); ++i) {
            file << "        {\n";
            file << "            \"username\": \"" << users[i].username << "\",\n";
            file << "            \"password\": \"" << users[i].password << "\",\n";
            file << "            \"name\": \"" << users[i].name << "\",\n";
            file << "            \"email\": \"" << users[i].email << "\",\n";
            file << "            \"bio\": \"" << users[i].bio << "\",\n";
            file << "            \"activities\": [\n";

            for (size_t j = 0; j < users[i].activities.size(); ++j) {
                file << "                {\n";
                file << "                    \"name\": \"" << users[i].activities[j].name << "\",\n";
                file << "                    \"category\": \"" << users[i].activities[j].category << "\",\n";
                file << "                    \"task_list\": [\n";

                for (size_t k = 0; k < users[i].activities[j].task_list.size(); ++k) {
                    file << "                        { \"task\": \"" << users[i].activities[j].task_list[k].task << "\", \"status\": \"" << users[i].activities[j].task_list[k].status << "\" }";
                    if (k < users[i].activities[j].task_list.size() - 1) file << ",";
                    file << "\n";
                }

                file << "                    ],\n";
                file << "                    \"progress\": " << users[i].activities[j].progress << "\n";
                file << "                }";
                if (j < users[i].activities.size() - 1) file << ",";
                file << "\n";
            }

            file << "            ]\n";
            file << "        }";
            if (i < users.size() - 1) file << ",";
            file << "\n";
        }

        file << "    ]\n}\n";
        file.close();
    }

private:
    static std::string extractJsonString(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return "";

        size_t start = json.find('\"', pos + searchKey.length());
        if (start == std::string::npos) return "";

        size_t end = json.find('\"', start + 1);
        if (end == std::string::npos) return "";

        return json.substr(start + 1, end - start - 1);
    }

    static int extractJsonInt(const std::string& json, const std::string& key) {
        std::string searchKey = "\"" + key + "\"";
        size_t pos = json.find(searchKey);
        if (pos == std::string::npos) return 0;

        size_t start = json.find(':', pos);
        if (start == std::string::npos) return 0;

        size_t end = json.find_first_of(",}", start);
        if (end == std::string::npos) return 0;

        std::string numStr = json.substr(start + 1, end - start - 1);
        numStr.erase(0, numStr.find_first_not_of(" \t\n\r"));

        return std::stoi(numStr);
    }
};

class Button {
public:
    sf::RectangleShape rect;
    sf::Text text;
    bool hovered = false;

    Button(float x, float y, float w, float h, const std::string& label, const sf::Font& font) {
        rect.setSize(sf::Vector2f(w, h));
        rect.setPosition(x, y);
        rect.setFillColor(sf::Color(255, 182, 193));
        
        text.setString(label);
        text.setFont(font);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::Black);
        
        auto bounds = text.getLocalBounds();
        text.setPosition(x + (w - bounds.width) / 2, y + (h - bounds.height) / 2 - 5);
    }

    bool isClicked(sf::Vector2f pos) {
        return rect.getGlobalBounds().contains(pos);
    }

    void update(sf::Vector2f pos) {
        hovered = rect.getGlobalBounds().contains(pos);
        if (hovered) {
            rect.setFillColor(sf::Color(255, 160, 180));
        } else {
            rect.setFillColor(sf::Color(255, 182, 193));
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rect);
        window.draw(text);
    }
};

class InputBox {
public:
    sf::RectangleShape rect;
    sf::Text text;
    sf::Text placeholder;
    std::string input;
    bool focused = false;

    InputBox(float x, float y, float w, float h, const std::string& placeHolder, const sf::Font& font) {
        rect.setSize(sf::Vector2f(w, h));
        rect.setPosition(x, y);
        rect.setFillColor(sf::Color(240, 240, 240));
        rect.setOutlineColor(sf::Color(200, 200, 200));
        rect.setOutlineThickness(1);

        placeholder.setString(placeHolder);
        placeholder.setFont(font);
        placeholder.setCharacterSize(16);
        placeholder.setFillColor(sf::Color(180, 180, 180));
        placeholder.setPosition(x + 10, y + 8);

        text.setFont(font);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::Black);
        text.setPosition(x + 10, y + 8);
    }

    bool isClicked(sf::Vector2f pos) {
        return rect.getGlobalBounds().contains(pos);
    }

    void handleClick(sf::Vector2f pos) {
        focused = isClicked(pos);
        if (focused) {
            rect.setOutlineColor(sf::Color(150, 150, 255));
        } else {
            rect.setOutlineColor(sf::Color(200, 200, 200));
        }
    }

    void handleKeyPress(sf::Event& event) {
        if (!focused) return;
        
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8) { // Backspace
                if (!input.empty()) {
                    input.pop_back();
                }
            } else if (event.text.unicode >= 32 && event.text.unicode < 127 && input.length() < 50) {
                input += static_cast<char>(event.text.unicode);
            }
        }
        text.setString(input);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rect);
        if (input.empty()) {
            window.draw(placeholder);
        } else {
            window.draw(text);
        }
    }

    void clear() {
        input.clear();
        text.setString("");
    }
};

enum Screen {
    LOGIN_SIGNUP,
    WELCOME,
    PROFILE,
    DASHBOARD
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 700), "Task-Tracker");
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    Screen currentScreen = LOGIN_SIGNUP;
    
    // Load users from JSON (one directory up from sfml folder)
    std::string jsonFile = "../../dummy_data.json";
    std::vector<User> allUsers = JSONManager::loadUsers(jsonFile);
    
    User* currentUser = nullptr;
    bool loginError = false;
    sf::Clock errorClock;

    // Login/Signup screen
    InputBox usernameInput(300, 200, 600, 50, "Username", font);
    InputBox passwordInput(300, 280, 600, 50, "Password", font);
    InputBox signupEmailInput(300, 360, 600, 50, "Email", font);
    Button loginBtn(300, 450, 280, 50, "Login", font);
    Button signupBtn(620, 450, 280, 50, "Sign Up", font);

    // Welcome screen
    Button profileBtn(100, 300, 1000, 120, "Profile - View/edit your info", font);
    Button dashboardBtn(100, 480, 1000, 120, "Dashboard - See all your activities", font);

    // Profile screen
    InputBox profileNameInput(200, 180, 800, 50, "Name", font);
    InputBox profileEmailInput(200, 260, 800, 50, "Email", font);
    InputBox profileBioInput(200, 340, 800, 120, "Bio", font);
    Button saveProfileBtn(300, 520, 600, 50, "Save Profile", font);
    Button backBtn(50, 50, 100, 40, "Back", font);

    // Dashboard screen
    InputBox activityNameInput(50, 140, 400, 40, "Activity name", font);
    Button addActivityBtn(50, 240, 120, 40, "Add Activity", font);
    Button clearBtn(200, 240, 80, 40, "Clear", font);

    bool isSigningUp = false;

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (currentScreen == LOGIN_SIGNUP) {
                usernameInput.handleClick(mousePos);
                passwordInput.handleClick(mousePos);
                signupEmailInput.handleClick(mousePos);

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (loginBtn.isClicked(mousePos)) {
                        bool found = false;
                        for (auto& user : allUsers) {
                            if (user.username == usernameInput.input && user.password == passwordInput.input) {
                                currentUser = &user;
                                currentScreen = WELCOME;
                                found = true;
                                loginError = false;
                                break;
                            }
                        }
                        if (!found) {
                            loginError = true;
                            errorClock.restart();
                        }
                    }
                    if (signupBtn.isClicked(mousePos)) {
                        if (!usernameInput.input.empty() && !passwordInput.input.empty() && !signupEmailInput.input.empty()) {
                            // Check if username already exists
                            bool exists = false;
                            for (auto& user : allUsers) {
                                if (user.username == usernameInput.input) {
                                    exists = true;
                                    break;
                                }
                            }
                            if (!exists) {
                                User newUser;
                                newUser.username = usernameInput.input;
                                newUser.password = passwordInput.input;
                                newUser.email = signupEmailInput.input;
                                newUser.name = usernameInput.input;
                                newUser.bio = "";
                                allUsers.push_back(newUser);
                                
                                JSONManager::saveUsers(jsonFile, allUsers);
                                currentUser = &allUsers.back();
                                
                                usernameInput.clear();
                                passwordInput.clear();
                                signupEmailInput.clear();
                                currentScreen = WELCOME;
                            }
                        }
                    }
                }

                usernameInput.handleKeyPress(event);
                passwordInput.handleKeyPress(event);
                signupEmailInput.handleKeyPress(event);
            }
            else if (currentScreen == WELCOME) {
                if (event.type == sf::Event::MouseButtonPressed) {
                    if (profileBtn.isClicked(mousePos)) {
                        profileNameInput.input = currentUser->name;
                        profileEmailInput.input = currentUser->email;
                        profileBioInput.input = currentUser->bio;
                        currentScreen = PROFILE;
                    }
                    if (dashboardBtn.isClicked(mousePos)) {
                        currentScreen = DASHBOARD;
                    }
                }
            }
            else if (currentScreen == PROFILE) {
                profileNameInput.handleClick(mousePos);
                profileEmailInput.handleClick(mousePos);
                profileBioInput.handleClick(mousePos);

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (saveProfileBtn.isClicked(mousePos)) {
                        currentUser->name = profileNameInput.input;
                        currentUser->email = profileEmailInput.input;
                        currentUser->bio = profileBioInput.input;
                        JSONManager::saveUsers(jsonFile, allUsers);
                        currentScreen = WELCOME;
                    }
                    if (backBtn.isClicked(mousePos)) {
                        currentScreen = WELCOME;
                    }
                }

                profileNameInput.handleKeyPress(event);
                profileEmailInput.handleKeyPress(event);
                profileBioInput.handleKeyPress(event);
            }
            else if (currentScreen == DASHBOARD) {
                activityNameInput.handleClick(mousePos);

                if (event.type == sf::Event::MouseButtonPressed) {
                    if (addActivityBtn.isClicked(mousePos)) {
                        if (!activityNameInput.input.empty() && currentUser) {
                            Activity newActivity;
                            newActivity.name = activityNameInput.input;
                            newActivity.category = "General";
                            newActivity.progress = 0;
                            currentUser->activities.push_back(newActivity);
                            JSONManager::saveUsers(jsonFile, allUsers);
                            activityNameInput.clear();
                            std::cout << "Activity added: " << newActivity.name << std::endl;
                        }
                    }
                    if (clearBtn.isClicked(mousePos)) {
                        activityNameInput.clear();
                    }
                    if (backBtn.isClicked(mousePos)) {
                        currentScreen = WELCOME;
                    }
                }

                activityNameInput.handleKeyPress(event);
            }
        }

        // Update button hover states
        loginBtn.update(mousePos);
        signupBtn.update(mousePos);
        profileBtn.update(mousePos);
        dashboardBtn.update(mousePos);
        saveProfileBtn.update(mousePos);
        backBtn.update(mousePos);
        addActivityBtn.update(mousePos);
        clearBtn.update(mousePos);

        // Draw
        window.clear(sf::Color::White);

        if (currentScreen == LOGIN_SIGNUP) {
            sf::Text title("Task-Tracker", font, 50);
            title.setFillColor(sf::Color::Black);
            title.setPosition(400, 50);
            window.draw(title);

            sf::Text subtitle("Stay one step ahead..", font, 24);
            subtitle.setFillColor(sf::Color(150, 150, 150));
            subtitle.setPosition(400, 120);
            window.draw(subtitle);

            usernameInput.draw(window);
            passwordInput.draw(window);
            signupEmailInput.draw(window);
            loginBtn.draw(window);
            signupBtn.draw(window);

            if (loginError && errorClock.getElapsedTime().asSeconds() < 3.0f) {
                sf::Text errorMsg("Invalid username or password!", font, 18);
                errorMsg.setFillColor(sf::Color::Red);
                errorMsg.setPosition(300, 550);
                window.draw(errorMsg);
            }
        }
        else if (currentScreen == WELCOME && currentUser) {
            sf::Text title("Task-Tracker", font, 50);
            title.setFillColor(sf::Color::Black);
            title.setPosition(350, 80);
            window.draw(title);

            sf::Text subtitle("Welcome, " + currentUser->name, font, 24);
            subtitle.setFillColor(sf::Color(150, 150, 150));
            subtitle.setPosition(350, 150);
            window.draw(subtitle);

            profileBtn.draw(window);
            dashboardBtn.draw(window);
        }
        else if (currentScreen == PROFILE && currentUser) {
            sf::Text title("Profile", font, 40);
            title.setFillColor(sf::Color::Black);
            title.setPosition(500, 30);
            window.draw(title);

            sf::Text subtitle("Edit your personal information", font, 18);
            subtitle.setFillColor(sf::Color(150, 150, 150));
            subtitle.setPosition(350, 90);
            window.draw(subtitle);

            sf::CircleShape avatar(50);
            avatar.setFillColor(sf::Color(220, 180, 220));
            avatar.setPosition(550, 120);
            window.draw(avatar);

            profileNameInput.draw(window);
            profileEmailInput.draw(window);
            profileBioInput.draw(window);
            saveProfileBtn.draw(window);
            backBtn.draw(window);
        }
        else if (currentScreen == DASHBOARD && currentUser) {
            sf::Text title("Task-Tracker - " + currentUser->name + "'s Activity Dashboard", font, 24);
            title.setFillColor(sf::Color::Black);
            title.setPosition(50, 30);
            window.draw(title);

            sf::Text subtitle("cute pastel task manager - escape deadlines with style", font, 14);
            subtitle.setFillColor(sf::Color(150, 150, 150));
            subtitle.setPosition(50, 70);
            window.draw(subtitle);

            // Left panel - Add Activity
            sf::Text addActivityLabel("Add Activity", font, 20);
            addActivityLabel.setFillColor(sf::Color::Black);
            addActivityLabel.setPosition(50, 110);
            window.draw(addActivityLabel);

            activityNameInput.draw(window);
            addActivityBtn.draw(window);
            clearBtn.draw(window);

            // Right panel - All Activities
            sf::Text allActivitiesLabel("All Activities", font, 20);
            allActivitiesLabel.setFillColor(sf::Color::Black);
            allActivitiesLabel.setPosition(600, 110);
            window.draw(allActivitiesLabel);

            float activityY = 160;
            for (const auto& activity : currentUser->activities) {
                sf::RectangleShape activityBox(sf::Vector2f(600, 80));
                activityBox.setPosition(600, activityY);
                activityBox.setFillColor(sf::Color(200, 220, 255));
                window.draw(activityBox);

                sf::RectangleShape progressBar(sf::Vector2f((activity.progress / 100.0f) * 500, 10));
                progressBar.setPosition(620, activityY + 50);
                progressBar.setFillColor(sf::Color(255, 192, 203));
                window.draw(progressBar);

                sf::Text activityName(activity.name, font, 18);
                activityName.setFillColor(sf::Color::Black);
                activityName.setPosition(620, activityY + 10);
                window.draw(activityName);

                sf::Text activityInfo(activity.category + " • " + std::to_string(activity.task_list.size()) + " tasks", font, 14);
                activityInfo.setFillColor(sf::Color(100, 100, 100));
                activityInfo.setPosition(620, activityY + 35);
                window.draw(activityInfo);

                sf::Text progressText(std::to_string(activity.progress) + "%", font, 14);
                progressText.setFillColor(sf::Color::Black);
                progressText.setPosition(1100, activityY + 30);
                window.draw(progressText);

                activityY += 100;
            }

            backBtn.draw(window);
        }

        window.display();
    }

    return 0;
}