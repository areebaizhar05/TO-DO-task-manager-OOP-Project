#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

struct Task {
    std::string task;
    std::string status;
};

struct Activity {
    std::string name;
    std::string category;
    std::string dueDate;
    std::vector<Task> task_list;
    int progress;
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
            if (event.text.unicode == 8) {
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
        text.setCharacterSize(16);
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

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 700), "Task-Tracker - Dashboard");
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    // Sample activities with tasks
    std::vector<Activity> activities;
    activities.push_back({
        "Final Project", 
        "Project", 
        "2025-11-13",
        {{"research", "complete"}, {"proposal", "complete"}, {"demo", "incomplete"}},
        0
    });
    activities.push_back({
        "Grocery", 
        "Personal", 
        "no deadline",
        {{"milk", "incomplete"}, {"eggs", "incomplete"}},
        50
    });

    InputBox activityNameInput(50, 160, 350, 40, "Activity name", font);
    InputBox deadlineInput(50, 220, 350, 40, "Deadline (dd/mm/yyyy)", font);
    Button categoryBtn(50, 280, 80, 40, "Project", font);
    std::string selectedCategory = "Project";
    std::vector<std::string> categories = {"Project", "Personal", "General"};
    int categoryIndex = 0;
    bool showCategoryDropdown = false;

    Button addActivityBtn(50, 340, 120, 40, "Add Activity", font);
    Button clearBtn(200, 340, 80, 40, "Clear", font);
    Button backBtn(50, 620, 100, 40, "Back", font);

    Button filterBtn(1050, 110, 120, 40, "Filter: All", font);
    std::string currentFilter = "All";
    std::vector<std::string> filterOptions = {"All", "Project", "Personal", "General"};
    int filterIndex = 0;
    bool showFilterDropdown = false;

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            activityNameInput.handleClick(mousePos);
            deadlineInput.handleClick(mousePos);

            if (event.type == sf::Event::MouseButtonPressed) {
                // Category dropdown
                if (categoryBtn.isClicked(mousePos)) {
                    showCategoryDropdown = !showCategoryDropdown;
                }

                if (showCategoryDropdown) {
                    for (int i = 0; i < 3; ++i) {
                        sf::RectangleShape dropdownItem(sf::Vector2f(80, 30));
                        dropdownItem.setPosition(50, 320 + i * 30);
                        if (dropdownItem.getGlobalBounds().contains(mousePos)) {
                            selectedCategory = categories[i];
                            categoryBtn.text.setString(selectedCategory);
                            showCategoryDropdown = false;
                        }
                    }
                }

                // Filter dropdown
                if (filterBtn.isClicked(mousePos)) {
                    showFilterDropdown = !showFilterDropdown;
                }

                if (showFilterDropdown) {
                    for (int i = 0; i < 4; ++i) {
                        sf::RectangleShape dropdownItem(sf::Vector2f(120, 30));
                        dropdownItem.setPosition(1050, 150 + i * 30);
                        if (dropdownItem.getGlobalBounds().contains(mousePos)) {
                            currentFilter = filterOptions[i];
                            filterBtn.text.setString("Filter: " + currentFilter);
                            showFilterDropdown = false;
                        }
                    }
                }

                if (addActivityBtn.isClicked(mousePos)) {
                    if (!activityNameInput.input.empty()) {
                        Activity newActivity;
                        newActivity.name = activityNameInput.input;
                        newActivity.category = selectedCategory;
                        newActivity.dueDate = deadlineInput.input.empty() ? "no deadline" : deadlineInput.input;
                        newActivity.progress = 0;
                        activities.push_back(newActivity);
                        activityNameInput.clear();
                        deadlineInput.clear();
                        std::cout << "Activity added: " << newActivity.name << " (" << newActivity.category << ")" << std::endl;
                    }
                }

                if (clearBtn.isClicked(mousePos)) {
                    activityNameInput.clear();
                    deadlineInput.clear();
                    std::cout << "Clear clicked" << std::endl;
                }

                if (backBtn.isClicked(mousePos)) {
                    std::cout << "Back clicked" << std::endl;
                }

                // Check if Open button was clicked
                float activityY = 160;
                for (size_t i = 0; i < activities.size(); ++i) {
                    if (currentFilter != "All" && activities[i].category != currentFilter) {
                        continue;
                    }
                    
                    sf::RectangleShape openBtn(sf::Vector2f(80, 30));
                    openBtn.setPosition(1100, activityY + 50);
                    if (openBtn.getGlobalBounds().contains(mousePos)) {
                        std::cout << "Opening activity: " << activities[i].name << std::endl;
                    }
                    activityY += 100;
                }
            }

            activityNameInput.handleKeyPress(event);
            deadlineInput.handleKeyPress(event);
        }

        addActivityBtn.update(mousePos);
        clearBtn.update(mousePos);
        backBtn.update(mousePos);
        filterBtn.update(mousePos);

        // Draw
        window.clear(sf::Color::White);

        sf::Text title("Task-Tracker - Username's Activity Dashboard", font, 24);
        title.setFillColor(sf::Color::Black);
        title.setPosition(50, 30);
        window.draw(title);

        sf::Text subtitle("cute pastel task manager - escape deadlines with style", font, 14);
        subtitle.setFillColor(sf::Color(150, 150, 150));
        subtitle.setPosition(50, 65);
        window.draw(subtitle);

        // Left panel - Add Activity
        sf::Text addActivityLabel("Add Activity", font, 20);
        addActivityLabel.setFillColor(sf::Color::Black);
        addActivityLabel.setPosition(50, 130);
        window.draw(addActivityLabel);

        activityNameInput.draw(window);
        deadlineInput.draw(window);

        // Category button
        categoryBtn.draw(window);

        addActivityBtn.draw(window);
        clearBtn.draw(window);

        // Tips section
        sf::Text tipsLabel("Tips", font, 20);
        tipsLabel.setFillColor(sf::Color::Black);
        tipsLabel.setPosition(50, 450);
        window.draw(tipsLabel);

        sf::Text tipsText("Use the list to quickly open an activity. Click the\nmood emoji in the activity panel to celebrate\nwhen it's done", font, 14);
        tipsText.setFillColor(sf::Color(150, 150, 150));
        tipsText.setPosition(50, 490);
        window.draw(tipsText);

        // Right panel - All Activities
        sf::Text allActivitiesLabel("All Activities", font, 20);
        allActivitiesLabel.setFillColor(sf::Color::Black);
        allActivitiesLabel.setPosition(600, 130);
        window.draw(allActivitiesLabel);

        // Filter button
        filterBtn.draw(window);

        // Draw activities
        float activityY = 180;
        for (size_t i = 0; i < activities.size(); ++i) {
            // Filter activities
            if (currentFilter != "All" && activities[i].category != currentFilter) {
                continue;
            }

            sf::RectangleShape activityBox(sf::Vector2f(550, 80));
            activityBox.setPosition(600, activityY);
            activityBox.setFillColor(sf::Color(200, 220, 255));
            window.draw(activityBox);

            // Progress bar
            sf::RectangleShape progressBar(sf::Vector2f((activities[i].progress / 100.0f) * 450, 10));
            progressBar.setPosition(620, activityY + 50);
            progressBar.setFillColor(sf::Color(255, 192, 203));
            window.draw(progressBar);

            sf::Text activityName(activities[i].name, font, 18);
            activityName.setFillColor(sf::Color::Black);
            activityName.setPosition(620, activityY + 10);
            window.draw(activityName);

            sf::Text activityInfo(activities[i].category + " • " + activities[i].dueDate, font, 14);
            activityInfo.setFillColor(sf::Color(100, 100, 100));
            activityInfo.setPosition(620, activityY + 35);
            window.draw(activityInfo);

            sf::Text progressText(std::to_string(activities[i].progress) + "%", font, 14);
            progressText.setFillColor(sf::Color::Black);
            progressText.setPosition(1050, activityY + 30);
            window.draw(progressText);

            // Open button
            sf::RectangleShape openBtn(sf::Vector2f(80, 30));
            openBtn.setPosition(1100, activityY + 50);
            openBtn.setFillColor(sf::Color(255, 182, 193));
            
            if (openBtn.getGlobalBounds().contains(mousePos)) {
                openBtn.setFillColor(sf::Color(255, 160, 180));
            }
            
            window.draw(openBtn);

            sf::Text openBtnText("Open", font, 14);
            openBtnText.setFillColor(sf::Color::Black);
            openBtnText.setPosition(1115, activityY + 52);
            window.draw(openBtnText);

            activityY += 100;
        }

        // Draw dropdowns LAST so they appear on top
        // Category dropdown
        if (showCategoryDropdown) {
            for (int i = 0; i < 3; ++i) {
                sf::RectangleShape dropdownItem(sf::Vector2f(80, 30));
                dropdownItem.setPosition(50, 320 + i * 30);
                dropdownItem.setFillColor(sf::Color(240, 240, 240));
                dropdownItem.setOutlineColor(sf::Color(200, 200, 200));
                dropdownItem.setOutlineThickness(1);
                window.draw(dropdownItem);

                sf::Text dropdownText(categories[i], font, 14);
                dropdownText.setFillColor(sf::Color::Black);
                dropdownText.setPosition(60, 320 + i * 30 + 5);
                window.draw(dropdownText);
            }
        }

        // Filter dropdown
        if (showFilterDropdown) {
            for (int i = 0; i < 4; ++i) {
                sf::RectangleShape dropdownItem(sf::Vector2f(120, 30));
                dropdownItem.setPosition(1050, 150 + i * 30);
                dropdownItem.setFillColor(sf::Color(240, 240, 240));
                dropdownItem.setOutlineColor(sf::Color(200, 200, 200));
                dropdownItem.setOutlineThickness(1);
                window.draw(dropdownItem);

                sf::Text dropdownText(filterOptions[i], font, 14);
                dropdownText.setFillColor(sf::Color::Black);
                dropdownText.setPosition(1060, 150 + i * 30 + 5);
                window.draw(dropdownText);
            }
        }

        backBtn.draw(window);

        window.display();
    }

    return 0;
}