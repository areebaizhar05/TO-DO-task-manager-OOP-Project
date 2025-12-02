#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>

#include "AuthSystem.hpp"
#include "JSONHandler.hpp"
#include "User.hpp"
#include "Task.cpp"          
#include "SimpleSubTask.hpp"
#include "UI_Components.hpp"    

//converting string to lowercase for authentication in login
std::string toLower(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    return str;
}

//Character emotion based on progress percentage
int getEmotionIndex(float progress) {
    if (progress >= 80) return 4; 
    if (progress >= 60) return 3; 
    if (progress >= 40) return 2; 
    if (progress >= 20) return 1; 
    return 0; 
}

//different stages/states of our app (each screen basicaly)
enum AppState { SIGNUP, LOGIN, WELCOME, DASHBOARD, PROFILE, TASK_DETAILS };

int main() {
    //backend set up
    AuthSystem authSystem;
    User* currentUser = nullptr;
    Task* currentSelectedTask = nullptr; 
    int selectedSubTaskIndex = -1; 
    
    //dashboard filter
    std::string currentFilter = "All";
    bool showFilterDropdown = false;
    
    //dashboard delete button for the task/activity
    bool showDeletePopup = false;
    std::string taskToDeleteTitle = "";

    //Json file handling checks (can remove later but i think we should keep it incase some issue happens during demo)
    std::cout << "Attempting to load data.json..." << std::endl;
    bool loadSuccess = JSONHandler::loadFromJSON("data.json", authSystem.getUserDatabase());
    if (loadSuccess) {
        std::cout << "SUCCESS: Loaded " << authSystem.getUserDatabase().size() << " users." << std::endl;
    } else {
        std::cout << "WARNING: Could not find data.json. Starting with empty database." << std::endl;
    }

    //SFML SET UP STUFF + font error check
    sf::RenderWindow window(sf::VideoMode(1200, 700), "Task-Tracker");
    window.setFramerateLimit(60);
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) {
        std::cerr << "Error: arial.ttf not found!" << std::endl;
        return -1;
    }

    //loading the character pngs + error check
    std::vector<sf::Texture> emotionTextures(5);
    std::vector<std::string> emotionFiles = { "1.png", "2.png", "3.png", "4.png", "5.png" };
    for (size_t i = 0; i < emotionFiles.size(); ++i) {
        if (!emotionTextures[i].loadFromFile(emotionFiles[i])) {
            std::cerr << "Warning: Could not load " << emotionFiles[i] << std::endl;
        }
    }

    AppState currentState = SIGNUP; 

    //==================== UI STUFF STARTS FROM THIS POINT ===========================================

    //SIGN UP SCREEN
    InputBox signupName(300, 150, 600, 50, "Full Name", font);
    InputBox signupEmail(300, 220, 600, 50, "Email Address", font);
    InputBox signupUser(300, 290, 600, 50, "Choose a Username (Unique)", font);
    InputBox signupPass(300, 360, 600, 50, "Password", font);
    Button btnSignUpConfirm(300, 450, 600, 50, "Sign Up", font);
    Button btnGoToLogin(300, 520, 600, 40, "Already have an account? Login", font);

    //LOGIN SCREEN
    InputBox loginUser(300, 250, 600, 50, "Username", font);
    InputBox loginPass(300, 330, 600, 50, "Password", font);
    Button btnLoginConfirm(300, 430, 600, 50, "Login", font);
    Button btnGoToSignUp(300, 500, 600, 40, "Don't have an account? Sign Up", font);
    sf::Text statusMsg("", font, 18);
    statusMsg.setPosition(400, 600);

    //WELCOME SCREEN
    Button welcomeProfileBtn(100, 300, 1000, 120, "Profile - View/edit your info", font);
    Button welcomeDashboardBtn(100, 480, 1000, 120, "Dashboard - See all your activities", font);
    Button logoutBtn(1050, 30, 100, 40, "Logout", font);

    //DASHBOARD SCREEN STUFF
    InputBox activityNameInput(50, 160, 350, 40, "New Activity Name", font);
    InputBox deadlineInput(50, 220, 350, 40, "Category (e.g. Project)", font); // Acts as Category input
    Button addActivityBtn(50, 280, 150, 40, "Add Activity", font);
    Button clearActivityBtn(210, 280, 100, 40, "Clear", font); // NEW CLEAR BUTTON
    Button backToWelcomeFromDash(50, 620, 100, 40, "Back", font);
    Button filterBtn(1000, 80, 150, 40, "Filter: All", font); // NEW FILTER BUTTON

    //PROFILE DETAILS SCREEN STUFF
    InputBox profileName(200, 180, 800, 50, "Name", font);
    InputBox profileEmail(200, 260, 800, 50, "Email", font);
    InputBox profileBio(200, 340, 800, 120, "Bio", font);
    Button saveProfileBtn(300, 520, 600, 50, "Save Profile", font);
    Button backToWelcomeFromProfile(50, 50, 100, 40, "Back", font);

    //TASK DETAIL SCREEN STUFF
    InputBox subTaskInput(150, 500, 600, 40, "Add Subtask", font);
    Button addSubTaskBtn(770, 500, 100, 40, "Add", font);
    Button closeDetailsBtn(1080, 20, 100, 40, "Close", font); 
    Button deleteSubTaskBtn(770, 570, 150, 40, "Delete Task", font);

    //DELETING TASK/ACTIVITY POPUP (on dashboard)
    sf::RectangleShape popupOverlay(sf::Vector2f(1200, 700));
    popupOverlay.setFillColor(sf::Color(0, 0, 0, 150));   
    //turning rest of the screen low opacity black for fancy vibe^
    sf::RectangleShape popupBox(sf::Vector2f(400, 200));
    popupBox.setPosition(400, 250);
    popupBox.setFillColor(sf::Color::White);
    popupBox.setOutlineColor(sf::Color::Black);
    popupBox.setOutlineThickness(2);
    Button btnConfirmDelete(450, 380, 100, 40, "Yes", font);
    Button btnCancelDelete(650, 380, 100, 40, "No", font);


    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                std::cout << "Saving data..." << std::endl;
                JSONHandler::saveToJSON("data.json", authSystem.getUserDatabase());
                window.close();
            }

            //if popup is open then we block input handling
            if (showDeletePopup) {
                 if (event.type == sf::Event::MouseButtonPressed) {
                     if (btnConfirmDelete.isClicked(mousePos)) {
                         currentUser->deleteTask(taskToDeleteTitle);
                         showDeletePopup = false;
                     }
                     if (btnCancelDelete.isClicked(mousePos)) {
                         showDeletePopup = false;
                     }
                 }
                 continue; //go back to normal stuff
            }

            //normal input handling stuff
            if (currentState == SIGNUP) 
            {
                signupName.handleClick(mousePos); signupName.handleKeyPress(event);
                signupEmail.handleClick(mousePos); signupEmail.handleKeyPress(event);
                signupUser.handleClick(mousePos); signupUser.handleKeyPress(event);
                signupPass.handleClick(mousePos); signupPass.handleKeyPress(event);
            }
            else if (currentState == LOGIN) 
            {
                loginUser.handleClick(mousePos); loginUser.handleKeyPress(event);
                loginPass.handleClick(mousePos); loginPass.handleKeyPress(event);
            }
            else if (currentState == DASHBOARD) 
            {
                activityNameInput.handleClick(mousePos); activityNameInput.handleKeyPress(event);
                deadlineInput.handleClick(mousePos); deadlineInput.handleKeyPress(event);
            }
            else if (currentState == PROFILE) 
            {
                profileName.handleClick(mousePos); profileName.handleKeyPress(event);
                profileEmail.handleClick(mousePos); profileEmail.handleKeyPress(event);
                profileBio.handleClick(mousePos); profileBio.handleKeyPress(event);
            }
            else if (currentState == TASK_DETAILS) 
            {
                subTaskInput.handleClick(mousePos); subTaskInput.handleKeyPress(event);
            }

            //BUTTON CLICK FUNCTIONALITY
            if (event.type == sf::Event::MouseButtonPressed) {
                
                //SIGN UP PAGE
                if (currentState == SIGNUP) {
                    if (btnGoToLogin.isClicked(mousePos)) {
                        currentState = LOGIN;
                        statusMsg.setString("");
                        loginUser.clear(); loginPass.clear();
                    }
                    if (btnSignUpConfirm.isClicked(mousePos)) {
                        std::string uName = signupUser.input;
                        std::string uKey = toLower(uName);
                        std::string uPass = signupPass.input;
                        std::string uRealName = signupName.input;
                        std::string uEmail = signupEmail.input;
                        auto& db = authSystem.getUserDatabase();
                        
                        if (uName.empty() || uPass.empty()) {
                            statusMsg.setString("Username/Password required.");
                            statusMsg.setFillColor(sf::Color::Red);
                        } else if (db.find(uKey) != db.end()) {
                            statusMsg.setString("Username taken.");
                            statusMsg.setFillColor(sf::Color::Red);
                        } else {
                            User* newUser = new User(uName, uRealName, uEmail);
                            newUser->setPassword(uPass);
                            db[uKey] = newUser;
                            statusMsg.setString("Account Created! Login.");
                            statusMsg.setFillColor(sf::Color::Green);
                            currentState = LOGIN; 
                            signupName.clear(); signupEmail.clear(); signupUser.clear(); signupPass.clear();
                        }
                    }
                }

                //LOGIN PAGE
                else if (currentState == LOGIN) {
                    if (btnGoToSignUp.isClicked(mousePos)) {
                        currentState = SIGNUP; statusMsg.setString("");
                    }
                    if (btnLoginConfirm.isClicked(mousePos)) {
                        auto& db = authSystem.getUserDatabase();
                        std::string uKey = toLower(loginUser.input);
                        if (db.find(uKey) != db.end() && db[uKey]->getPassword() == loginPass.input) {
                            currentUser = db[uKey];
                            currentState = WELCOME;
                            statusMsg.setString("");
                            profileName.setText(currentUser->getName());
                            profileEmail.setText(currentUser->getEmail());
                            profileBio.setText(currentUser->getBio());
                        } else {
                            statusMsg.setString("Invalid Credentials.");
                            statusMsg.setFillColor(sf::Color::Red);
                        }
                    }
                }

                //WELCOME PAGE
                else if (currentState == WELCOME) {
                    if (welcomeProfileBtn.isClicked(mousePos)) currentState = PROFILE;
                    if (welcomeDashboardBtn.isClicked(mousePos)) currentState = DASHBOARD;
                    if (logoutBtn.isClicked(mousePos)) {
                        currentUser = nullptr; currentState = LOGIN;
                        loginUser.clear(); loginPass.clear();
                    }
                }

                //DASHBOARD SCREEN
                else if (currentState == DASHBOARD) {
                    if (backToWelcomeFromDash.isClicked(mousePos)) currentState = WELCOME;
                    
                    if (clearActivityBtn.isClicked(mousePos)) {
                        activityNameInput.clear();
                        deadlineInput.clear();
                    }

                    if (addActivityBtn.isClicked(mousePos) && !activityNameInput.input.empty()) {
                        std::string cat = deadlineInput.input.empty() ? "General" : deadlineInput.input;
                        currentUser->createTask(activityNameInput.input, cat);
                        activityNameInput.clear();
                    }

                    //filter drop down logic
                    if (filterBtn.isClicked(mousePos)) {
                        showFilterDropdown = !showFilterDropdown;
                    }
                    else if (showFilterDropdown) 
                    {
                        std::vector<std::string> cats = currentUser->getAllCategories();
                        cats.insert(cats.begin(), "All"); 
                        
                        for (size_t i = 0; i < cats.size(); ++i) {
                            sf::RectangleShape item(sf::Vector2f(150, 30));
                            item.setPosition(1000, 120 + i * 30);
                            if (item.getGlobalBounds().contains(mousePos)) {
                                currentFilter = cats[i];
                                filterBtn.setText("Filter: " + currentFilter);
                                showFilterDropdown = false;
                            }
                        }
                    }

                    //Task list stuff
                    float activityY = 180;
                    auto tasks = currentUser->getTasks();
                    for (auto task : tasks) {
                        //filter logic
                        if (currentFilter != "All" && task->getCategory() != currentFilter) 
                            continue;

                        //open task button
                        sf::RectangleShape openBtn(sf::Vector2f(60, 30));
                        openBtn.setPosition(1120, activityY + 50); 

                        //red square delete task button
                        sf::RectangleShape delBtn(sf::Vector2f(30, 30));
                        delBtn.setPosition(1080, activityY + 50);

                        if (openBtn.getGlobalBounds().contains(mousePos)) {
                            currentSelectedTask = task;
                            selectedSubTaskIndex = -1; 
                            currentState = TASK_DETAILS;
                        }

                        if (delBtn.getGlobalBounds().contains(mousePos)) {
                            taskToDeleteTitle = task->getTitle();
                            showDeletePopup = true;
                        }

                        activityY += 100;
                    }
                }

                //PROFILE DETAILS PAGE
                else if (currentState == PROFILE) {
                    if (backToWelcomeFromProfile.isClicked(mousePos)) currentState = WELCOME;
                    if (saveProfileBtn.isClicked(mousePos)) {
                        currentUser->updateProfile(profileName.input, profileEmail.input, profileBio.input);
                    }
                }

                //TASK DETAILS ( SUB-TASK PAGE)
                else if (currentState == TASK_DETAILS && currentSelectedTask) {
                    if (closeDetailsBtn.isClicked(mousePos)) currentState = DASHBOARD;
                    
                    if (addSubTaskBtn.isClicked(mousePos) && !subTaskInput.input.empty()) {
                        SubTask* newSt = new SimpleSubTask(subTaskInput.input);
                        currentSelectedTask->addSubTask(newSt);
                        subTaskInput.clear();
                    }

                    if (deleteSubTaskBtn.isClicked(mousePos)) {
                        if (selectedSubTaskIndex != -1) {
                            currentSelectedTask->removeSubTask(selectedSubTaskIndex + 1);
                            selectedSubTaskIndex = -1; 
                        }
                    }

                    //CHECKBOX FOR SUB-TASK COMPLETION
                    float taskY = 150;
                    bool clickedRow = false;
                    for (int i = 1; i <= currentSelectedTask->getSubTaskCount(); i++) {
                        sf::FloatRect rowBounds(40, taskY, 900, 50);
                        if (rowBounds.contains(mousePos)) {
                            selectedSubTaskIndex = i - 1; 
                            clickedRow = true;
                        }

                        SubTask* st = currentSelectedTask->getSubTask(i);
                        Checkbox cb(40, taskY, st->isCompleted());
                        
                        if (cb.isClicked(mousePos)) {
                            struct SubTaskAccess : public SubTask { using SubTask::completed; };
                            SubTaskAccess* access = static_cast<SubTaskAccess*>(st);
                            if (st->isCompleted()) access->completed = false;
                            else st->markCompleted();
                            currentSelectedTask->updateProgressCount();
                            clickedRow = true; 
                        }
                        taskY += 60;
                    }
                }
            }
        }

        //actually drawing the sfml stuff now:
        window.clear(sf::Color::White);

        if (currentState == SIGNUP) {
            sf::Text title("Create Account", font, 40); title.setFillColor(sf::Color::Black); title.setPosition(450, 50); window.draw(title);
            signupName.draw(window); signupEmail.draw(window); signupUser.draw(window); signupPass.draw(window);
            btnSignUpConfirm.update(mousePos); btnSignUpConfirm.draw(window);
            btnGoToLogin.update(mousePos); btnGoToLogin.draw(window);
            window.draw(statusMsg);
        }
        else if (currentState == LOGIN) {
            sf::Text title("Welcome Back", font, 40); title.setFillColor(sf::Color::Black); title.setPosition(450, 100); window.draw(title);
            loginUser.draw(window); loginPass.draw(window);
            btnLoginConfirm.update(mousePos); btnLoginConfirm.draw(window);
            btnGoToSignUp.update(mousePos); btnGoToSignUp.draw(window);
            window.draw(statusMsg);
        }
        else if (currentState == WELCOME && currentUser) {
            sf::Text title("Task-Tracker", font, 50); title.setFillColor(sf::Color::Black); title.setPosition(480, 80); window.draw(title);
            sf::Text subtitle("Welcome, " + currentUser->getName(), font, 24); subtitle.setFillColor(sf::Color(150, 150, 150)); subtitle.setPosition(490, 150); window.draw(subtitle);
            welcomeProfileBtn.update(mousePos); welcomeProfileBtn.draw(window);
            welcomeDashboardBtn.update(mousePos); welcomeDashboardBtn.draw(window);
            logoutBtn.update(mousePos); logoutBtn.draw(window);
        }
        else if (currentState == DASHBOARD && currentUser) {
            sf::Text title("My Dashboard", font, 24); title.setFillColor(sf::Color::Black); title.setPosition(50, 30); window.draw(title);
            activityNameInput.draw(window); deadlineInput.draw(window);
            addActivityBtn.update(mousePos); addActivityBtn.draw(window);
            clearActivityBtn.update(mousePos); clearActivityBtn.draw(window); 
            backToWelcomeFromDash.update(mousePos); backToWelcomeFromDash.draw(window);
            filterBtn.update(mousePos); filterBtn.draw(window);

            float activityY = 180;
            auto tasks = currentUser->getTasks();
            for (auto task : tasks) {
                //apply filter
                if (currentFilter != "All" && task->getCategory() != currentFilter) continue;

                //blue box for activity/main task
                sf::RectangleShape activityBox(sf::Vector2f(580, 80)); 
                activityBox.setPosition(600, activityY); 
                activityBox.setFillColor(sf::Color(200, 220, 255)); 
                window.draw(activityBox);
                
                //activity/task text
                sf::Text taskName(task->getTitle(), font, 18); taskName.setFillColor(sf::Color::Black); taskName.setPosition(620, activityY + 10); window.draw(taskName);
                sf::Text catName(task->getCategory(), font, 14); catName.setFillColor(sf::Color::Black); catName.setPosition(620, activityY + 35); window.draw(catName);
                
                //progress bar of task
                float prog = task->getProgressPercent();
                sf::RectangleShape progBar(sf::Vector2f((prog / 100.0f) * 350, 10)); 
                progBar.setPosition(620, activityY + 60);
                progBar.setFillColor(sf::Color(255, 192, 203)); 
                window.draw(progBar);
                
                //open task/activity button on far right of blue box
                sf::RectangleShape openBtn(sf::Vector2f(60, 30)); 
                openBtn.setPosition(1120, activityY + 50); 
                openBtn.setFillColor(sf::Color(255, 182, 193));
                if (openBtn.getGlobalBounds().contains(mousePos)) openBtn.setFillColor(sf::Color(255, 160, 180));
                window.draw(openBtn);
                sf::Text openText("Open", font, 14); 
                openText.setFillColor(sf::Color::Black); 
                openText.setPosition(1130, activityY + 55);
                window.draw(openText);

                //delete button beside the open button
                sf::RectangleShape delBtn(sf::Vector2f(30, 30));
                delBtn.setPosition(1080, activityY + 50);
                delBtn.setFillColor(sf::Color(255, 100, 100)); // Red
                window.draw(delBtn);
                sf::Text delText("X", font, 16);
                delText.setFillColor(sf::Color::White);
                delText.setPosition(1089, activityY + 53);
                window.draw(delText);

                activityY += 100;
            }

            //filter dropdown
            if (showFilterDropdown) {
                std::vector<std::string> cats = currentUser->getAllCategories();
                cats.insert(cats.begin(), "All");
                for (size_t i = 0; i < cats.size(); ++i) {
                    sf::RectangleShape item(sf::Vector2f(150, 30));
                    item.setPosition(1000, 120 + i * 30);
                    item.setFillColor(sf::Color(240, 240, 240));
                    item.setOutlineColor(sf::Color(200, 200, 200));
                    item.setOutlineThickness(1);
                    window.draw(item);
                    
                    sf::Text catText(cats[i], font, 14);
                    catText.setFillColor(sf::Color::Black);
                    catText.setPosition(1010, 125 + i * 30);
                    window.draw(catText);
                }
            }

            //delete popup overlay in low opacity black
            if (showDeletePopup) {
                window.draw(popupOverlay);
                window.draw(popupBox);
    
                sf::Text warnTitle("Delete Activity?", font, 24);
                warnTitle.setFillColor(sf::Color::Black);
                warnTitle.setPosition(520, 280);
                window.draw(warnTitle);

                sf::Text warnMsg("Are you sure you want to delete:\n" + taskToDeleteTitle, font, 18);
                warnMsg.setFillColor(sf::Color(100, 100, 100));
                warnMsg.setPosition(430, 330);
                window.draw(warnMsg);

                btnConfirmDelete.update(mousePos); btnConfirmDelete.draw(window);
                btnCancelDelete.update(mousePos); btnCancelDelete.draw(window);
            }
        }
        else if (currentState == PROFILE && currentUser) {
            sf::Text title("Profile", font, 40); title.setFillColor(sf::Color::Black); title.setPosition(540, 30); window.draw(title);
            sf::Text subtitle("Edit your personal information", font, 18); subtitle.setFillColor(sf::Color(150, 150, 150)); subtitle.setPosition(480, 80); window.draw(subtitle);
            sf::CircleShape avatar(50); avatar.setFillColor(sf::Color(221, 160, 221)); avatar.setPosition(550, 120); window.draw(avatar);
            profileName.draw(window); profileEmail.draw(window); profileBio.draw(window);
            saveProfileBtn.update(mousePos); saveProfileBtn.draw(window);
            backToWelcomeFromProfile.update(mousePos); backToWelcomeFromProfile.draw(window);
        }
        else if (currentState == TASK_DETAILS && currentSelectedTask) {
             sf::RectangleShape topBar(sf::Vector2f(1200, 80)); topBar.setFillColor(sf::Color(240, 240, 250)); window.draw(topBar);
             sf::Text title(currentSelectedTask->getTitle(), font, 32); title.setFillColor(sf::Color::Black); title.setPosition(40, 15); window.draw(title);
             sf::Text info(currentSelectedTask->getCategory(), font, 14); info.setFillColor(sf::Color(150, 150, 150)); info.setPosition(40, 50); window.draw(info);
             closeDetailsBtn.update(mousePos); closeDetailsBtn.draw(window);
             
             float taskY = 150;
             for (int i = 1; i <= currentSelectedTask->getSubTaskCount(); i++) {
                 if ((i - 1) == selectedSubTaskIndex) {
                     sf::RectangleShape highlight(sf::Vector2f(900, 50)); highlight.setPosition(40, taskY); highlight.setFillColor(sf::Color(255, 240, 245)); window.draw(highlight);
                 }
                 SubTask* st = currentSelectedTask->getSubTask(i);
                 Checkbox cb(40, taskY, st->isCompleted());
                 cb.draw(window);
                 sf::Text tName(st->getName(), font, 18); tName.setFillColor(sf::Color::Black); tName.setPosition(80, taskY + 2); window.draw(tName);
                 taskY += 60;
             }
             float progress = currentSelectedTask->getProgressPercent();
             int emoIndex = getEmotionIndex(progress);
             if (emoIndex >= 0 && emoIndex < 5) {
                 sf::Sprite emotionSprite(emotionTextures[emoIndex]); emotionSprite.setPosition(800, 140); emotionSprite.setScale(0.7f, 0.7f); window.draw(emotionSprite);
             }
             sf::Text addTaskLabel("Add a subtask", font, 14); addTaskLabel.setFillColor(sf::Color(150, 150, 150)); addTaskLabel.setPosition(40, 465); window.draw(addTaskLabel);
             subTaskInput.draw(window);
             addSubTaskBtn.update(mousePos); addSubTaskBtn.draw(window);
             deleteSubTaskBtn.update(mousePos); deleteSubTaskBtn.draw(window);
             sf::RectangleShape progressBarBg(sf::Vector2f(400, 20)); progressBarBg.setPosition(250, 600); progressBarBg.setFillColor(sf::Color(220, 220, 220)); window.draw(progressBarBg);
             sf::RectangleShape progressBar(sf::Vector2f((progress / 100.0f) * 400, 20)); progressBar.setPosition(250, 600); progressBar.setFillColor(sf::Color(255, 192, 203)); window.draw(progressBar);
             int completed = currentSelectedTask->getCompletedCount();
             int total = currentSelectedTask->getSubTaskCount();
             sf::Text progressText(std::to_string(completed) + " / " + std::to_string(total) + " tasks completed", font, 14); progressText.setFillColor(sf::Color::Black); progressText.setPosition(40, 600); window.draw(progressText);
             sf::Text percentText(std::to_string((int)progress) + "%", font, 14); percentText.setFillColor(sf::Color::Black); percentText.setPosition(660, 600); window.draw(percentText);
        }

        window.display();
    }
    return 0;
}