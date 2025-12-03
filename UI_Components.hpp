#ifndef UI_COMPONENTS_HPP
#define UI_COMPONENTS_HPP

#include <SFML/Graphics.hpp>
#include <string>

class Button {
public:
    sf::RectangleShape rect;
    sf::Text text;
    bool hovered = false;

    Button(float x, float y, float w, float h, const std::string& label, const sf::Font& font) {
        rect.setSize(sf::Vector2f(w, h));
        rect.setPosition(x, y);
        rect.setFillColor(sf::Color(255, 182, 193)); // Pink
        
        text.setString(label);
        text.setFont(font);
        text.setCharacterSize(18);
        text.setFillColor(sf::Color::Black);
        
        centerText();
    }

    void centerText() {
        auto bounds = text.getLocalBounds();
        text.setPosition(rect.getPosition().x + (rect.getSize().x - bounds.width) / 2, 
                         rect.getPosition().y + (rect.getSize().y - bounds.height) / 2 - 5);
    }

    void setText(std::string s) {
        text.setString(s);
        centerText();
    }

    bool isClicked(sf::Vector2f pos) {
        return rect.getGlobalBounds().contains(pos);
    }

    void update(sf::Vector2f pos) {
        hovered = rect.getGlobalBounds().contains(pos);
        rect.setFillColor(hovered ? sf::Color(255, 160, 180) : sf::Color(255, 182, 193));
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
        rect.setOutlineColor(focused ? sf::Color(150, 150, 255) : sf::Color(200, 200, 200));
    }
    
    void setText(std::string s) {
        input = s;
        text.setString(input);
    }

    void handleKeyPress(sf::Event& event) {
        if (!focused) return;
        if (event.type == sf::Event::TextEntered) {
            if (event.text.unicode == 8) { // Backspace
                if (!input.empty()) input.pop_back();
            } else if (event.text.unicode >= 32 && event.text.unicode < 127 && input.length() < 50) {
                input += static_cast<char>(event.text.unicode);
            }
        }
        text.setString(input);
    }

    void draw(sf::RenderWindow& window) {
        window.draw(rect);
        window.draw(input.empty() ? placeholder : text);
    }

    void clear() {
        input.clear();
        text.setString("");
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
    
    bool isClicked(sf::Vector2f pos) { return box.getGlobalBounds().contains(pos); }
    void toggle() { checked = !checked; }
    
    void draw(sf::RenderWindow& window) {
        window.draw(box);
        if (checked) {
            sf::RectangleShape checkmark(sf::Vector2f(10, 10));
            checkmark.setFillColor(sf::Color(100, 149, 237));
            checkmark.setPosition(box.getPosition().x + 7, box.getPosition().y + 7);
            window.draw(checkmark);
        }
    }
};

#endif