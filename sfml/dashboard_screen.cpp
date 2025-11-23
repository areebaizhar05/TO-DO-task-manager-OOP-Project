#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>

struct Activity {
    std::string name;
    std::string category;
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

int main() {
    sf::RenderWindow window(sf::VideoMode(1200, 700), "Task-Tracker - Dashboard");
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    // Sample activities
    std::vector<Activity> activities;
    activities.push_back({"Final Project", "Project", 0});
    activities.push_back({"Grocery", "Personal", 50});

    InputBox activityNameInput(50, 140, 400, 40, "Activity name", font);
    Button addActivityBtn(50, 240, 120, 40, "Add Activity", font);
    Button clearBtn(200, 240, 80, 40, "Clear", font);
    Button backBtn(50, 50, 100, 40, "Back", font);

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            activityNameInput.handleClick(mousePos);

            if (event.type == sf::Event::MouseButtonPressed) {
                if (addActivityBtn.isClicked(mousePos)) {
                    std::cout << "Add Activity clicked" << std::endl;
                }
                if (clearBtn.isClicked(mousePos)) {
                    std::cout << "Clear clicked" << std::endl;
                    activityNameInput.clear();
                }
                if (backBtn.isClicked(mousePos)) {
                    std::cout << "Back clicked" << std::endl;
                }
            }

            activityNameInput.handleKeyPress(event);
        }

        addActivityBtn.update(mousePos);
        clearBtn.update(mousePos);
        backBtn.update(mousePos);

        // Draw
        window.clear(sf::Color::White);

        sf::Text title("Task-Tracker - Username's Activity Dashboard", font, 24);
        title.setFillColor(sf::Color::Black);
        title.setPosition(200, 30);
        window.draw(title);

        sf::Text subtitle("cute pastel task manager - escape deadlines with style", font, 14);
        subtitle.setFillColor(sf::Color(150, 150, 150));
        subtitle.setPosition(200, 70);
        window.draw(subtitle);

        // Left panel - Add Activity
        sf::Text addActivityLabel("Add Activity", font, 20);
        addActivityLabel.setFillColor(sf::Color::Black);
        addActivityLabel.setPosition(50, 110);
        window.draw(addActivityLabel);

        activityNameInput.draw(window);
        addActivityBtn.draw(window);
        clearBtn.draw(window);

        // Tips section
        sf::Text tipsLabel("Tips", font, 20);
        tipsLabel.setFillColor(sf::Color::Black);
        tipsLabel.setPosition(50, 310);
        window.draw(tipsLabel);

        sf::Text tipsText("Use the list to quickly open an activity. Click the\nmood emoji in the activity panel to celebrate\nwhen it's done", font, 14);
        tipsText.setFillColor(sf::Color(150, 150, 150));
        tipsText.setPosition(50, 350);
        window.draw(tipsText);

        // Right panel - All Activities
        sf::Text allActivitiesLabel("All Activities", font, 20);
        allActivitiesLabel.setFillColor(sf::Color::Black);
        allActivitiesLabel.setPosition(600, 110);
        window.draw(allActivitiesLabel);

        // Draw activities
        float activityY = 160;
        for (const auto& activity : activities) {
            sf::RectangleShape activityBox(sf::Vector2f(600, 80));
            activityBox.setPosition(600, activityY);
            activityBox.setFillColor(sf::Color(200, 220, 255));
            window.draw(activityBox);

            // Progress bar
            sf::RectangleShape progressBar(sf::Vector2f((activity.progress / 100.0f) * 500, 10));
            progressBar.setPosition(620, activityY + 50);
            progressBar.setFillColor(sf::Color(255, 192, 203));
            window.draw(progressBar);

            sf::Text activityName(activity.name, font, 18);
            activityName.setFillColor(sf::Color::Black);
            activityName.setPosition(620, activityY + 10);
            window.draw(activityName);

            sf::Text activityInfo(activity.category + "- no deadline", font, 14);
            activityInfo.setFillColor(sf::Color(100, 100, 100));
            activityInfo.setPosition(620, activityY + 35);
            window.draw(activityInfo);

            sf::Text progressText(std::to_string(activity.progress) + "%", font, 14);
            progressText.setFillColor(sf::Color::Black);
            progressText.setPosition(1100, activityY + 30);
            window.draw(progressText);

            sf::RectangleShape openBtn(sf::Vector2f(80, 30));
            openBtn.setPosition(1100, activityY + 50);
            openBtn.setFillColor(sf::Color(255, 182, 193));
            window.draw(openBtn);

            sf::Text openBtnText("Open", font, 14);
            openBtnText.setFillColor(sf::Color::Black);
            openBtnText.setPosition(1115, activityY + 52);
            window.draw(openBtnText);

            activityY += 100;
        }

        backBtn.draw(window);

        window.display();
    }

    return 0;
}