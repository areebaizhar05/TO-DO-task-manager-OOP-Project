#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>

struct Task {
    std::string name;
    bool completed;
};

struct Activity {
    std::string name;
    std::string category;
    std::string dueDate;
    std::vector<Task> tasks;
    int getProgress() const {
        if (tasks.empty()) return 0;
        int completedCount = 0;
        for (const auto& task : tasks) {
            if (task.completed) completedCount++;
        }
        return (completedCount * 100) / tasks.size();
    }
    
    char getEmotion() const {
        int progress = getProgress();
        if (progress <= 25) return '1'; // Sad
        if (progress <= 50) return '2'; // Neutral
        if (progress <= 75) return '3'; // Excited
        return '4'; // Ecstatic
    }
};

class Checkbox {
public:
    sf::RectangleShape box;
    bool checked;
    
    Checkbox(float x, float y, bool isChecked = false) : checked(isChecked) {
        box.setSize(sf::Vector2f(24, 24));
        box.setPosition(x, y);
        box.setFillColor(sf::Color::White);
        box.setOutlineThickness(2);
        box.setOutlineColor(sf::Color(100, 149, 237));
    }
    
    bool isClicked(sf::Vector2f pos) {
        return box.getGlobalBounds().contains(pos);
    }
    
    void toggle() {
        checked = !checked;
    }
    
    void draw(sf::RenderWindow& window) {
        window.draw(box);
        
        if (checked) {
            sf::RectangleShape checkmark(sf::Vector2f(4, 12));
            checkmark.setFillColor(sf::Color(100, 149, 237));
            checkmark.setPosition(box.getPosition().x + 8, box.getPosition().y + 6);
            window.draw(checkmark);
            
            sf::RectangleShape checkmark2(sf::Vector2f(12, 4));
            checkmark2.setFillColor(sf::Color(100, 149, 237));
            checkmark2.setPosition(box.getPosition().x + 10, box.getPosition().y + 14);
            window.draw(checkmark2);
        }
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
        placeholder.setCharacterSize(14);
        placeholder.setFillColor(sf::Color(180, 180, 180));
        placeholder.setPosition(x + 10, y + 5);

        text.setFont(font);
        text.setCharacterSize(14);
        text.setFillColor(sf::Color::Black);
        text.setPosition(x + 10, y + 5);
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

std::string getEmotionEmoji(char emotion) {
    if (emotion == '1') return ":("; // Sad
    if (emotion == '2') return ":|"; // Neutral
    if (emotion == '3') return ":O"; // Excited
    return ":D"; // Ecstatic
}

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 750), "Task-Tracker - Activity Details");
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    // Sample activity with tasks
    Activity activity;
    activity.name = "Final Project";
    activity.category = "Project";
    activity.dueDate = "2025-11-13";
    activity.tasks = {
        {"research", true},
        {"proposal", true},
        {"demo", false}
    };

    std::vector<Checkbox> checkboxes;
    for (size_t i = 0; i < activity.tasks.size(); ++i) {
        checkboxes.push_back(Checkbox(40, 150 + i * 60, activity.tasks[i].completed));
    }

    InputBox addTaskInput(150, 500, 600, 40, "Add a subtask (e.g., research)", font);
    Button addTaskBtn(770, 500, 100, 40, "Add Task", font);
    Button deleteActivityBtn(770, 570, 150, 40, "Delete Activity", font);
    Button closeBtn(880, 30, 100, 40, "Close", font);

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            addTaskInput.handleClick(mousePos);

            if (event.type == sf::Event::MouseButtonPressed) {
                // Check checkbox clicks
                for (size_t i = 0; i < checkboxes.size(); ++i) {
                    if (checkboxes[i].isClicked(mousePos)) {
                        checkboxes[i].toggle();
                        activity.tasks[i].completed = checkboxes[i].checked;
                        std::cout << "Task '" << activity.tasks[i].name << "' toggled to " 
                                  << (activity.tasks[i].completed ? "completed" : "incomplete") << std::endl;
                    }
                }

                if (addTaskBtn.isClicked(mousePos)) {
                    if (!addTaskInput.input.empty()) {
                        activity.tasks.push_back({addTaskInput.input, false});
                        checkboxes.push_back(Checkbox(40, 150 + (activity.tasks.size() - 1) * 60, false));
                        addTaskInput.clear();
                        std::cout << "New task added: " << activity.tasks.back().name << std::endl;
                    }
                }

                if (deleteActivityBtn.isClicked(mousePos)) {
                    std::cout << "Delete activity clicked" << std::endl;
                }

                if (closeBtn.isClicked(mousePos)) {
                    std::cout << "Close activity details" << std::endl;
                    window.close();
                }
            }

            addTaskInput.handleKeyPress(event);
        }

        addTaskBtn.update(mousePos);
        deleteActivityBtn.update(mousePos);
        closeBtn.update(mousePos);

        // Draw
        window.clear(sf::Color::White);

        // Top bar with activity title and close button
        sf::RectangleShape topBar(sf::Vector2f(1000, 80));
        topBar.setFillColor(sf::Color(240, 240, 250));
        window.draw(topBar);

        // Emotion emoji
        sf::Text emotion(getEmotionEmoji(activity.getEmotion()), font, 48);
        emotion.setFillColor(sf::Color::Black);
        emotion.setPosition(40, 15);
        window.draw(emotion);

        // Activity title
        sf::Text title(activity.name, font, 32);
        title.setFillColor(sf::Color::Black);
        title.setPosition(120, 15);
        window.draw(title);

        // Activity info (category and due date)
        sf::Text info(activity.category + " • Due " + activity.dueDate, font, 14);
        info.setFillColor(sf::Color(150, 150, 150));
        info.setPosition(120, 50);
        window.draw(info);

        closeBtn.draw(window);

        // Tasks section
        float taskY = 150;
        for (size_t i = 0; i < activity.tasks.size(); ++i) {
            checkboxes[i].draw(window);

            sf::Text taskName(activity.tasks[i].name, font, 18);
            taskName.setFillColor(sf::Color::Black);
            taskName.setPosition(80, taskY + 2);
            window.draw(taskName);

            taskY += 60;
        }

        // Add task section
        sf::Text addTaskLabel("Add a subtask (e.g., research)", font, 14);
        addTaskLabel.setFillColor(sf::Color(150, 150, 150));
        addTaskLabel.setPosition(40, 465);
        window.draw(addTaskLabel);

        addTaskInput.draw(window);
        addTaskBtn.draw(window);

        // Progress section
        float progress = activity.getProgress();
        
        sf::RectangleShape progressBarBg(sf::Vector2f(400, 20));
        progressBarBg.setPosition(150, 600);
        progressBarBg.setFillColor(sf::Color(220, 220, 220));
        window.draw(progressBarBg);

        sf::RectangleShape progressBar(sf::Vector2f((progress / 100.0f) * 400, 20));
        progressBar.setPosition(150, 600);
        progressBar.setFillColor(sf::Color(255, 192, 203));
        window.draw(progressBar);

        sf::Text progressText(std::to_string((int)activity.tasks.size()) + " / " + 
                            std::to_string((int)activity.tasks.size()) + " tasks complete", font, 14);
        progressText.setFillColor(sf::Color::Black);
        progressText.setPosition(40, 600);
        window.draw(progressText);

        deleteActivityBtn.draw(window);

        window.display();
    }

    return 0;
}