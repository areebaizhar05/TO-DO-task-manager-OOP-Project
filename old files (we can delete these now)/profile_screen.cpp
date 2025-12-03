#include <SFML/Graphics.hpp>
#include <iostream>

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
    sf::RenderWindow window(sf::VideoMode(1200, 700), "Task-Tracker - Profile");
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    InputBox nameInput(200, 180, 800, 50, "Name", font);
    InputBox emailInput(200, 260, 800, 50, "Email", font);
    InputBox bioInput(200, 340, 800, 120, "Bio", font);

    Button saveBtn(300, 520, 600, 50, "Save Profile", font);
    Button backBtn(50, 50, 100, 40, "Back", font);

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            nameInput.handleClick(mousePos);
            emailInput.handleClick(mousePos);
            bioInput.handleClick(mousePos);

            if (event.type == sf::Event::MouseButtonPressed) {
                if (saveBtn.isClicked(mousePos)) {
                    std::cout << "Profile saved" << std::endl;
                }
                if (backBtn.isClicked(mousePos)) {
                    std::cout << "Back button clicked" << std::endl;
                }
            }

            nameInput.handleKeyPress(event);
            emailInput.handleKeyPress(event);
            bioInput.handleKeyPress(event);
        }

        saveBtn.update(mousePos);
        backBtn.update(mousePos);

        //Draw
        window.clear(sf::Color::White);

        sf::Text title("Profile", font, 40);
        title.setFillColor(sf::Color::Black);
        title.setPosition(550, 30);
        window.draw(title);

        sf::Text subtitle("Edit your personal information", font, 18);
        subtitle.setFillColor(sf::Color(150, 150, 150));
        subtitle.setPosition(480, 90);
        window.draw(subtitle);

        sf::CircleShape avatar(50);
        avatar.setFillColor(sf::Color(220, 180, 220));
        avatar.setPosition(550, 120);
        window.draw(avatar);

        nameInput.draw(window);
        emailInput.draw(window);
        bioInput.draw(window);
        saveBtn.draw(window);
        backBtn.draw(window);

        window.display();
    }

    return 0;
}