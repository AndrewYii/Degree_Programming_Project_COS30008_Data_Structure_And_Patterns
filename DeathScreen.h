#pragma once

#include "Screen.h"

// Forward declaration to avoid circular dependency
class Game;

class DeathScreen : public Screen
{
private:
    // Reference to Game 
    Game *game;
    // hovered button index (-1 = none)
    int hoveredButton; 
    // Fade-in alpha value 
    float fadeAlpha;

public:
    // Constructor [Header]
    DeathScreen(Game *g);
    // Destructor [Header]
    ~DeathScreen();
    // Handle events [Header]
    void handleEvents() override;
    // Update logic [Header]
    void update(float deltaTime) override;
    // Render screen [Header]
    void render(sf::RenderWindow &window, sf::Font &font) override;
    // Navigate to this screen [Header]
    void onEnter() override;
    // Exit from this screen [Header]
    void onExit() override;
};
