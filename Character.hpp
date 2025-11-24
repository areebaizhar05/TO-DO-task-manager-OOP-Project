#ifndef CHARACTER_HPP
#define CHARACTER_HPP

#include <string>
using namespace std;

enum class Emotion {
    EXCITED,
    HAPPY,
    NEUTRAL,
    WORRIED,
    SAD
};

class Character {
private:
    string name;
    Emotion currentEmotion;

public:
    Character(string name);

    void updateEmotion(float progressPercentage);
    void display(float progressPercentage);

    string getName() const { return name; }
    Emotion getEmotion() const { return currentEmotion; }
};

#endif