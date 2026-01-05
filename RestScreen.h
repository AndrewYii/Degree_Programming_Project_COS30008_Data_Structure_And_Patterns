#pragma once

#include "Screen.h"

// Forward declarations
class Game;
class Player;

class RestScreen : public Screen
{
private:
    Game *game;
    Player *playerRef;
    int hoveredButton; 
    bool actionTaken;
    bool viewingDeck;
    int hoveredCardIndex;
    int selectedCardIndex;
    bool showUpgradeConfirm;
    float confirmAlpha;
    float deckScrollOffset;
    float fadeAlpha;

public:
    // Constructor [Header]
    RestScreen(Game *g);
    // Destructor [Header]
    ~RestScreen();
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
    // Heal player [Header]
    void healPlayer();
    // Open upgrade card selection [Header]
    void openUpgradeSelection();
    // Upgrade selected card [Header]
    void upgradeCard(int cardIndex);
    // Draw upgrade overlay [Header]
    void drawUpgradeOverlay(sf::RenderWindow &window, sf::Font &font);
    // Draw upgrade confirmation popup [Header]
    void drawUpgradeConfirmPopup(sf::RenderWindow &window, sf::Font &font);
};
