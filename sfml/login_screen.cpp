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
    sf::RenderWindow window(sf::VideoMode(1200, 700), "Task-Tracker - Login/Sign Up");
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    // Input boxes
    InputBox usernameInput(300, 200, 600, 50, "Username", font);
    InputBox passwordInput(300, 280, 600, 50, "Password", font);
    InputBox emailInput(300, 360, 600, 50, "Email", font);

    // Buttons
    Button loginBtn(300, 450, 280, 50, "Login", font);
    Button signupBtn(620, 450, 280, 50, "Sign Up", font);

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            usernameInput.handleClick(mousePos);
            passwordInput.handleClick(mousePos);
            emailInput.handleClick(mousePos);

            if (event.type == sf::Event::MouseButtonPressed) {
                if (loginBtn.isClicked(mousePos)) {
                    std::cout << "Login clicked with username: " << usernameInput.input << std::endl;
                }
                if (signupBtn.isClicked(mousePos)) {
                    std::cout << "Sign Up clicked with email: " << emailInput.input << std::endl;
                }
            }

            usernameInput.handleKeyPress(event);
            passwordInput.handleKeyPress(event);
            emailInput.handleKeyPress(event);
        }

        // Update button hover states
        loginBtn.update(mousePos);
        signupBtn.update(mousePos);

        // Draw
        window.clear(sf::Color::White);

        // Title
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
        emailInput.draw(window);
        loginBtn.draw(window);
        signupBtn.draw(window);

        window.display();
    }

    return 0;
}