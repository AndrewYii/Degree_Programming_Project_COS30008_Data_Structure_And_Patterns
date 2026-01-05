#pragma once

#include "Screen.h"
#include <SFML/Graphics.hpp>

class MainMenuScreen : public Screen {
private:
    int selectedButton;  
    bool mousePressed;
    
public:
    // Constructor [header]
    MainMenuScreen(Game* gameInstance);
    // Destructor [header]
    ~MainMenuScreen() override;
    // Handle events, update, render [header]
    void handleEvents() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window, sf::Font& font) override;
    // Navigate to this screen [header]
    void onEnter() override;
    // Exit from this screen [header]
    void onExit() override;
    
private:
    // Helper function to check if mouse is over a button
    bool isMouseOverButton(sf::RenderWindow& window, sf::FloatRect buttonBounds);
    void drawGearIcon(sf::RenderWindow& window, float x, float y, float size, sf::Color color);
};
