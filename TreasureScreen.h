#pragma once

#include "Screen.h"
#include "Card.h"
#include <vector>

// Forward declarations
class Game;
class Player;


class TreasureScreen : public Screen
{
private:
    Game *game;
    Player *playerRef;

    // Rewards
    std::vector<Card *> cardOptions;
    int selectedCardIndex;
    bool rewardClaimed;
    // UI state
    bool hoveredContinue;
    int hoveredCardIndex;
    bool showCardDetail;
    Card *selectedCardForDetail;
    float cardDetailAlpha;
    bool hoveredConfirmButton;

    // Animation
    float fadeAlpha;

public:
    TreasureScreen(Game *g);
    ~TreasureScreen();
    void handleEvents() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window, sf::Font &font) override;
    void onEnter() override;
    void onExit() override;
    // Reward generation
    void generateRewards();
    void claimReward(int cardIndex);
    // Card detail popup
    void drawCardDetailPopup(sf::RenderWindow &window, sf::Font &font);
};
