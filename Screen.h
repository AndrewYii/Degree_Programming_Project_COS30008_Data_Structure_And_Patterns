#pragma once

#include <SFML/Graphics.hpp>

// Forward declaration
class Game;


class Screen {
protected:
    Game* game;  // Reference to Game manager
    
public:
    Screen(Game* gameInstance);
    virtual ~Screen();
    virtual void handleEvents() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window, sf::Font& font) = 0;
    virtual void onEnter() {}
    virtual void onExit() {}
};
