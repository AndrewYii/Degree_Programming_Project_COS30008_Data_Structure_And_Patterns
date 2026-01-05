#pragma once

#include "Screen.h"
#include "Card.h"
#include "AIStoryteller.h"
#include <vector>

// Forward declarations
class Game;
class Player;


class VictoryScreen : public Screen
{
private:
    Game *game;
    Player *playerRef;

    // Rewards
    int bitcoinReward;
    std::vector<Card *> cardOptions; 
    int selectedCardIndex;
    bool rewardsClaimed;

    // UI state
    bool hoveredContinue;
    int hoveredCardIndex;

    // Card detail popup (click to preview, then confirm)
    bool showCardDetail;
    int selectedCardForDetail;
    float cardDetailAlpha;
    bool hoveredConfirmButton;

    // Animation
    float fadeAlpha;

public:
    VictoryScreen(Game *g);
    ~VictoryScreen();

    void handleEvents() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window, sf::Font &font) override;
    void onEnter() override;
    void onExit() override;

    // Reward generation
    void generateRewards();
    void claimReward(int cardIndex);

    // Card detail popup
    void drawCardDetailPopup();
};
