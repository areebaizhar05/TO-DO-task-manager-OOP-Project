#include "Character.hpp"
#include <iostream>
using namespace std;

Character::Character(string name)
    : name(name), currentEmotion(Emotion::NEUTRAL) {}

void Character::updateEmotion(float progressPercentage) {
    if (progressPercentage >= 80.0f) {
        currentEmotion = Emotion::EXCITED;
    } else if (progressPercentage >= 60.0f) {
        currentEmotion = Emotion::HAPPY;
    } else if (progressPercentage >= 40.0f) {
        currentEmotion = Emotion::NEUTRAL;
    } else if (progressPercentage >= 20.0f) {
        currentEmotion = Emotion::WORRIED;
    } else {
        currentEmotion = Emotion::SAD;
    }
}

void Character::display(float progressPercentage) {
    updateEmotion(progressPercentage);

    cout << "\n+------------------------------------+" << endl;
    cout << "|   Your Companion: " << name << "            |" << endl;
    cout << "+------------------------------------+" << endl;

    switch(currentEmotion) {
        case Emotion::EXCITED:
            cout << "      +-------+\n";
            cout << "      | ^   ^ |\n";
            cout << "      |   o   |  Wow! Amazing progress!\n";
            cout << "      +-------+  You're crushing it!\n";
            break;

        case Emotion::HAPPY:
            cout << "      +-------+\n";
            cout << "      | ^   ^ |\n";
            cout << "      |   v   |  Great job!\n";
            cout << "      +-------+  Keep up the good work!\n";
            break;

        case Emotion::NEUTRAL:
            cout << "      +-------+\n";
            cout << "      | .   . |\n";
            cout << "      |   -   |  Steady progress.\n";
            cout << "      +-------+  Let's keep going!\n";
            break;

        case Emotion::WORRIED:
            cout << "      +-------+\n";
            cout << "      | -   - |\n";
            cout << "      |   ~   |  Hey, need a boost?\n";
            cout << "      +-------+  You can do this!\n";
            break;

        case Emotion::SAD:
            cout << "      +-------+\n";
            cout << "      | .   . |\n";
            cout << "      |   (   |  Don't worry!\n";
            cout << "      +-------+  Let's start small!\n";
            break;
    }

    cout << "====================================\n";
}
