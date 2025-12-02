#include <SFML/Graphics.hpp>
#include <iostream>

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
        text.setCharacterSize(25);
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
    sf::RenderWindow window(sf::VideoMode(1200, 700), "Task-Tracker - Welcome");
    window.setFramerateLimit(60);

    sf::Font font;
    font.loadFromFile("arial.ttf");

    Button profileBtn(100, 300, 1000, 120, "Profile - View/edit your info", font);
    Button dashboardBtn(100, 480, 1000, 120, "Dashboard - See all your activities", font);

    while (window.isOpen()) {
        sf::Event event;
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (profileBtn.isClicked(mousePos)) {
                    std::cout << "Profile button clicked" << std::endl;
                }
                if (dashboardBtn.isClicked(mousePos)) {
                    std::cout << "Dashboard button clicked" << std::endl;
                }
            }
        }

        // Update button hover states
        profileBtn.update(mousePos);
        dashboardBtn.update(mousePos);

        // Draw
        window.clear(sf::Color::White);

        sf::Text title("Task-Tracker", font, 50);
        title.setFillColor(sf::Color::Black);
        title.setPosition(480, 80);
        window.draw(title);

        sf::Text subtitle("Welcome, Username", font, 24);
        subtitle.setFillColor(sf::Color(150, 150, 150));
        subtitle.setPosition(510, 150);
        window.draw(subtitle);

        profileBtn.draw(window);
        dashboardBtn.draw(window);

        window.display();
    }

    return 0;
}