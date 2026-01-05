#pragma once

#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <string>
#include <vector>
#include <memory>
#include <cassert>

class StoryIntroScreen : public Screen
{
public:
    struct CharNode
    {
        char character;
        CharNode *next;
        CharNode(char c) : character(c), next(nullptr) {}
    };

    StoryIntroScreen(Game *gameInstance);
    ~StoryIntroScreen() override;

    void handleEvents() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window, sf::Font &font) override;

    void onEnter() override;
    void onExit() override;
    void setPlayerData(const std::string &name, const std::string &role);

private:
    // Singly linked list operations
    void appendCharacter(char c);
    std::string getDisplayedText();
    void clearList();

    // Story generation
    std::string generateStory(const std::string &pName, const std::string &roleName);
    std::string getRandomScenario(const std::string &roleName);
    void applyScenarioModifiers(const std::string &roleName, int scenarioIndex);

    // Story state
    std::string playerName;
    std::string selectedRole;
    std::string fullStoryText;

    CharNode *head;
    CharNode *tail;
    int displayedCharCount;

    // Typewriter effect
    float typewriterTimer;
    float charDelay;
    int currentCharIndex;
    bool storyComplete;

    // Challenge input
    std::string userInput;
    bool inputActive;
    const std::string correctAnswer = "cout << \"HELLO WORLD\";";

    // Shake animation (when wrong)
    bool shakeAnimation;
    float shakeTimer;
    float shakeDuration;
    float shakeIntensity;

    // Cursor blink
    float cursorBlinkTimer;
    bool cursorVisible;

    // Audio
    sf::SoundBuffer typingSoundBuffer;
    std::unique_ptr<sf::Sound> typingSound;

    sf::SoundBuffer errorSoundBuffer;
    std::unique_ptr<sf::Sound> errorSound;

    int lastCharPlayed;

    // UI state
    bool mousePressed;
};
