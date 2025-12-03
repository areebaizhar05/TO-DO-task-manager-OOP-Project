#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <set>

#include "AuthSystem.hpp"
#include "JSONHandler.hpp"
#include "User.hpp"
#include "Task.hpp"           
#include "SimpleSubTask.hpp"
#include "UI_Components.hpp" 
#include "DateHelper.hpp" // ADDED FOR DATE THING: Need DateHelper for Date class
#include "SubTask.hpp"    // ADDED FOR DATE THING: Need Priority enum
 
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

// ADDED FOR DATE THING: Priority color helper
sf::Color getPriorityColor(Priority p) {
    switch (p) {
        case Priority::HIGH:   return sf::Color(255, 100, 100); // Red (Overdue/Urgent)
        case Priority::MEDIUM: return sf::Color(255, 200, 100); // Orange/Yellow
        case Priority::LOW:    return sf::Color(150, 200, 255); // Light Blue/Calm
        default:               return sf::Color(200, 200, 200); // Grey
    }
}

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

    //scrolling variables
    float dashboardScroll = 0.0f;
    float detailsScroll = 0.0f;

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

    //loading the app logo i made on canva
    sf::Texture logoImage;
    sf::Sprite logo;
    if (logoImage.loadFromFile("logo.png")) {
        logo.setTexture(logoImage);
        logo.setPosition(490, 8); // Top Left Padding
        logo.setScale(0.18f, 0.18f); // Scale down (Change this number to resize logo)
    } else {
        std::cerr << "Warning: logo.png not found!" << std::endl;
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
    
    // ADDED FOR DATE THING: New date inputs for the main Task
    InputBox taskDayInput(50, 340, 70, 40, "DD", font);
    InputBox taskMonthInput(130, 340, 70, 40, "MM", font);
    InputBox taskYearInput(210, 340, 100, 40, "YYYY", font);

    Button addActivityBtn(320, 340, 80, 40, "Add", font); // Adjusted position for date inputs
    Button clearActivityBtn(50, 400, 100, 40, "Clear", font); // Adjusted position
    Button backToWelcomeFromDash(50, 620, 100, 40, "Back", font);
    Button filterBtn(1000, 80, 150, 40, "Filter: All", font); // NEW FILTER BUTTON
    
    // ADDED FOR DATE THING: Label for date inputs
    sf::Text dueDateLabel("Due Date (Optional):", font, 14); 
    dueDateLabel.setFillColor(sf::Color(150, 150, 150)); 
    dueDateLabel.setPosition(50, 320); 

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

            //scrolling Logic
            if (event.type == sf::Event::MouseWheelScrolled) {
                if (currentState == DASHBOARD) {
                    dashboardScroll += event.mouseWheelScroll.delta * 20.0f; //scrolling speed
                    if (dashboardScroll > 0) dashboardScroll = 0; 
                } else if (currentState == TASK_DETAILS) {
                    detailsScroll += event.mouseWheelScroll.delta * 20.0f;
                    if (detailsScroll > 0) detailsScroll = 0;
                }
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
                // ADDED FOR DATE THING: Handle date inputs
                taskDayInput.handleClick(mousePos); taskDayInput.handleKeyPress(event);
                taskMonthInput.handleClick(mousePos); taskMonthInput.handleKeyPress(event);
                taskYearInput.handleClick(mousePos); taskYearInput.handleKeyPress(event);
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
                    
                    // ADDED FOR DATE THING: Handle date inputs
                    taskDayInput.handleClick(mousePos); taskDayInput.handleKeyPress(event);
                    taskMonthInput.handleClick(mousePos); taskMonthInput.handleKeyPress(event);
                    taskYearInput.handleClick(mousePos); taskYearInput.handleKeyPress(event);
                    
                    if (clearActivityBtn.isClicked(mousePos)) {
                        activityNameInput.clear();
                        deadlineInput.clear();
                        // ADDED FOR DATE THING: Clear date inputs
                        taskDayInput.clear();
                        taskMonthInput.clear();
                        taskYearInput.clear();
                    }

                    if (addActivityBtn.isClicked(mousePos) && !activityNameInput.input.empty()) {
                        std::string cat = deadlineInput.input.empty() ? "General" : deadlineInput.input;
                        
                        // ADDED FOR DATE THING: Parse date inputs
                        Date taskDueDate;
                        int d = 0, m = 0, y = 0; // Initialize to zero (not set)

try {
    // Attempt to parse each field separately
    if (!taskDayInput.input.empty()) d = std::stoi(taskDayInput.input);
    if (!taskMonthInput.input.empty()) m = std::stoi(taskMonthInput.input);
    if (!taskYearInput.input.empty()) y = std::stoi(taskYearInput.input);
    
    // Create Date ONLY if all fields resulted in non-zero values
    if (d > 0 && m > 0 && y > 0) {
        taskDueDate = Date(d, m, y); 
    }
} catch (const std::exception& e) {}
                        
                        // UPDATED: Pass taskDueDate to createTask
                        currentUser->createTask(activityNameInput.input, cat, taskDueDate); 
                        
                        activityNameInput.clear();
                        deadlineInput.clear();
                        // ADDED FOR DATE THING: Clear date inputs
                        taskDayInput.clear();
                        taskMonthInput.clear();
                        taskYearInput.clear();
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
                    float activityY = 180 + dashboardScroll; //added scrolling
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
                            detailsScroll = 0; // Reset details scroll
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
                        // UPDATED: SimpleSubTask no longer takes a Date argument
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
                    float taskY = 150 + detailsScroll; // Apply Scroll
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
            window.draw(logo);
            sf::Text title("Welcome Back", font, 40); title.setFillColor(sf::Color::Black); title.setPosition(450, 170); window.draw(title);
            loginUser.draw(window); loginPass.draw(window);
            btnLoginConfirm.update(mousePos); btnLoginConfirm.draw(window);
            btnGoToSignUp.update(mousePos); btnGoToSignUp.draw(window);
            window.draw(statusMsg);
        }
        else if (currentState == WELCOME && currentUser) {
            window.draw(logo);
            sf::Text title("TASK-TRACKER", font, 50); title.setFillColor(sf::Color::Black); title.setPosition(400, 180); window.draw(title);
            sf::Text subtitle("Welcome, " + currentUser->getName(), font, 24); subtitle.setFillColor(sf::Color(150, 150, 150)); subtitle.setPosition(450, 240); window.draw(subtitle);
            welcomeProfileBtn.update(mousePos); welcomeProfileBtn.draw(window);
            welcomeDashboardBtn.update(mousePos); welcomeDashboardBtn.draw(window);
            logoutBtn.update(mousePos); logoutBtn.draw(window);
        }
        else if (currentState == DASHBOARD && currentUser) {
            
            //draw tasks FIRST (so they scroll under header)
            float activityY = 180 + dashboardScroll;
            auto tasks = currentUser->getTasks();
            
            //calculate total progress for character
            float totalProgSum = 0;
            int totalTasks = 0;

            for (auto task : tasks) {
                //apply filter
                if (currentFilter != "All" && task->getCategory() != currentFilter) continue;
                
                totalTasks++;
                totalProgSum += task->getProgressPercent();

                //blue box for activity/main task
                sf::RectangleShape activityBox(sf::Vector2f(580, 80)); 
                activityBox.setPosition(600, activityY); 
                activityBox.setFillColor(sf::Color(200, 220, 255)); 
                window.draw(activityBox);
                
                // ADDED FOR DATE THING: Color-coded Priority Border on the right
                sf::RectangleShape priorityBar(sf::Vector2f(5, 80)); // 5px wide bar
                priorityBar.setPosition(activityBox.getPosition().x + activityBox.getSize().x - 5, activityBox.getPosition().y); 
                priorityBar.setFillColor(getPriorityColor(task->getPriority())); 
                window.draw(priorityBar);
                
                //activity/task text
                sf::Text taskName(task->getTitle(), font, 18); taskName.setFillColor(sf::Color::Black); taskName.setPosition(620, activityY + 10); window.draw(taskName);
                sf::Text catName(task->getCategory(), font, 14); catName.setFillColor(sf::Color::Black); catName.setPosition(620, activityY + 35); window.draw(catName);
                
                // ADDED FOR DATE THING: Display Due Date Info
                std::string dueDateText;
                if (task->getDueDate().isSet()) {
                    int daysUntil = task->daysUntilDue();
                    if (daysUntil < 0) {
                        dueDateText = "OVERDUE! (" + std::to_string(std::abs(daysUntil)) + " days ago)";
                    } else if (daysUntil == 0) {
                        dueDateText = "Due TODAY!";
                    } else {
                        dueDateText = "Due in " + std::to_string(daysUntil) + " days (" + task->getDueDate().toString() + ")";
                    }
                } else {
                    dueDateText = "Priority: " + task->getPriorityString();
                }
                
                sf::Text dateText(dueDateText, font, 14); 
                dateText.setFillColor(getPriorityColor(task->getPriority())); // Color text as well
                dateText.setPosition(800, activityY + 10); // Adjust position as needed
                window.draw(dateText);

                //progress bar of task
                float prog = task->getProgressPercent();
                sf::RectangleShape progBar(sf::Vector2f((prog / 100.0f) * 350, 10)); 
                progBar.setPosition(620, activityY + 60);
                progBar.setFillColor(sf::Color(255, 192, 203)); 
                window.draw(progBar);
                
                //progress % text beside bar
                sf::Text taskProgText(std::to_string((int)prog) + "%", font, 14);
                taskProgText.setFillColor(sf::Color::Black);
                taskProgText.setPosition(980, activityY + 55);
                window.draw(taskProgText);

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

            //header thing to hide scrolling items at top
            sf::RectangleShape headerBg(sf::Vector2f(1200, 160));
            headerBg.setFillColor(sf::Color::White);
            window.draw(headerBg);

            sf::Text title("My Dashboard", font, 24); title.setFillColor(sf::Color::Black); title.setPosition(50, 30); window.draw(title);
            activityNameInput.draw(window); 
            deadlineInput.draw(window);
            
            // ADDED FOR DATE THING: Draw date inputs and label
            window.draw(dueDateLabel);
            taskDayInput.draw(window);
            taskMonthInput.draw(window);
            taskYearInput.draw(window);
            
            addActivityBtn.update(mousePos); addActivityBtn.draw(window);
            clearActivityBtn.update(mousePos); clearActivityBtn.draw(window); 
            backToWelcomeFromDash.update(mousePos); backToWelcomeFromDash.draw(window);
            filterBtn.update(mousePos); filterBtn.draw(window);

            //dashboard Character Logic
            float mainAvg = (totalTasks > 0) ? (totalProgSum / totalTasks) : 0;
            int mainEmo = getEmotionIndex(mainAvg);
            if (mainEmo >= 0 && mainEmo < 5) {
                sf::Sprite charSprite(emotionTextures[mainEmo]);
                charSprite.setPosition(250, 350); 
                charSprite.setScale(0.6f, 0.6f);
                window.draw(charSprite);
            }
            //total progress bar under character
            sf::RectangleShape totalBg(sf::Vector2f(200, 15));
            totalBg.setPosition(270, 660); totalBg.setFillColor(sf::Color(220, 220, 220)); window.draw(totalBg);
            sf::RectangleShape totalFill(sf::Vector2f((mainAvg / 100.0f) * 200, 15));
            totalFill.setPosition(270, 660); totalFill.setFillColor(sf::Color(255, 192, 203)); window.draw(totalFill);
            sf::Text totalText("Total Progress: " + std::to_string((int)mainAvg) + "%", font, 14);
            totalText.setFillColor(sf::Color::Black); totalText.setPosition(300, 680); window.draw(totalText);

            //filter dropdown (Draw last)
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
            
            //tasks list drawn first for scrolling
            float taskY = 150 + detailsScroll;
            for (int i = 1; i <= currentSelectedTask->getSubTaskCount(); i++) {
                if ((i - 1) == selectedSubTaskIndex) {
                    sf::RectangleShape highlight(sf::Vector2f(900, 50)); highlight.setPosition(40, taskY); highlight.setFillColor(sf::Color(255, 240, 245)); window.draw(highlight);
                }
                SubTask* st = currentSelectedTask->getSubTask(i);
                Checkbox cb(40, taskY, st->isCompleted());
                cb.draw(window);
                
                // Subtask name (only display name and check mark, no individual dates/priority)
                sf::Text tName(st->getName(), font, 18); 
                tName.setFillColor(sf::Color::Black); 
                tName.setPosition(80, taskY + 2); 
                window.draw(tName);
                
                taskY += 60;
            }

            //scrolling bar
            sf::RectangleShape topBar(sf::Vector2f(1200, 80)); topBar.setFillColor(sf::Color(240, 240, 250)); window.draw(topBar);
            sf::Text title(currentSelectedTask->getTitle(), font, 32); title.setFillColor(sf::Color::Black); title.setPosition(40, 15); window.draw(title);
            
            // UPDATED: Show Task-level priority/date info in the header
            std::string taskInfo = currentSelectedTask->getCategory();
            if (currentSelectedTask->getDueDate().isSet()) {
                taskInfo += " | Due: " + currentSelectedTask->getDueDate().toString() + " (" + currentSelectedTask->getPriorityString() + ")";
            } else {
                taskInfo += " | Priority: " + currentSelectedTask->getPriorityString();
            }

            sf::Text info(taskInfo, font, 14); 
            info.setFillColor(getPriorityColor(currentSelectedTask->getPriority())); 
            info.setPosition(40, 50); window.draw(info);
            closeDetailsBtn.update(mousePos); closeDetailsBtn.draw(window);

            //bottom part stays still
            sf::RectangleShape bottomCover(sf::Vector2f(1200, 300)); bottomCover.setPosition(0, 450); bottomCover.setFillColor(sf::Color::White); window.draw(bottomCover);

            float progress = currentSelectedTask->getProgressPercent();
            int emoIndex = getEmotionIndex(progress);
            if (emoIndex >= 0 && emoIndex < 5) {
                sf::Sprite emotionSprite(emotionTextures[emoIndex]); emotionSprite.setPosition(800, 140); emotionSprite.setScale(0.7f, 0.7f); window.draw(emotionSprite);
            }
            
            sf::Text addTaskLabel("Add a subtask ->", font, 14); addTaskLabel.setFillColor(sf::Color(150, 150, 150)); addTaskLabel.setPosition(31, 510); window.draw(addTaskLabel);
            subTaskInput.draw(window);
            
            // ADJUSTED ADD BUTTON POSITION
            addSubTaskBtn.rect.setPosition(770, 500);
            auto bounds = addSubTaskBtn.text.getLocalBounds();
            addSubTaskBtn.text.setPosition(addSubTaskBtn.rect.getPosition().x + (addSubTaskBtn.rect.getSize().x - bounds.width) / 2, addSubTaskBtn.rect.getPosition().y + (addSubTaskBtn.rect.getSize().y - bounds.height) / 2 - 5);
            addSubTaskBtn.draw(window);

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