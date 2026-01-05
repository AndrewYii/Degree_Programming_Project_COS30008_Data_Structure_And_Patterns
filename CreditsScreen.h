#pragma once

#include "Screen.h"
#include <vector>
#include <string>

class CreditsScreen : public Screen
{
private:
    // Credit y position and scrolling
    float scrollY;
    // Credit scrolling speed
    float scrollSpeed;
    // Credit scrolling completion status
    bool scrollComplete;
    struct CreditLine
    {
        std::string text;
        unsigned int fontSize;
        sf::Color color;
        bool isBold;
        float spacing;
        CreditLine(const std::string &txt, unsigned int size, sf::Color col = sf::Color::White, bool bold = false, float space = 0.f) : text(txt), fontSize(size), color(col), isBold(bold), spacing(space) {}
    };
    // List of credit lines to display
    std::vector<CreditLine> creditLines;
    // Initialize credit lines
    void initializeCredits();

public:
    // Constructor [Header]
    CreditsScreen(Game *gameInstance);
    // Destructor [Header]
    ~CreditsScreen() override;
    // Event handling [Header]
    void handleEvents() override;
    // Update logic [Header]
    void update(float deltaTime) override;
    // Rendering [Header]
    void render(sf::RenderWindow &window, sf::Font &font) override;
    // Navigate to this screen [Header]
    void onEnter() override;
    // Leave this screen [Header]
    void onExit() override;
};
