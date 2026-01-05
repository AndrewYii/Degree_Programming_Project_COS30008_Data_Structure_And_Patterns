#pragma once
#include "Screen.h"
#include <vector>
#include <string>

class Game;
class Card;

class TutorialScreen : public Screen
{
public:
    TutorialScreen(Game *g);
    ~TutorialScreen() override;

    void onEnter() override;
    void onExit() override;
    void handleEvents() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window, sf::Font &font) override;

private:
    Game *game;

    enum class TutorialStage
    {
        WELCOME,           
        EXPLAIN_GOAL,       
        EXPLAIN_CARDS,      
        EXPLAIN_ENERGY,     
        EXPLAIN_ACTIVE_BAR, 
        DEMONSTRATE_COMBAT, 
        EXPLAIN_ROOMS,      
        COMPLETE          
    };

    TutorialStage currentStage;
    float fadeAlpha;
    float stageTimer;
    bool hoveredNext;
    std::vector<Card *> demoCards;
    std::vector<Card *> activeBar;
    int hoveredCardIndex;
    int hoveredActiveIndex;
    int demoPlayerHP;
    int demoPlayerMaxHP;
    int demoPlayerMemory;
    int demoPlayerMaxMemory;
    int demoPlayerBlock;
    int demoEnemyHP;
    int demoEnemyMaxHP;
    float animationTimer;
    bool animationPlaying;
    std::string animationMessage;
    void advanceStage();
    void setupDemoCards();
    void clearDemoCards();
    void renderWelcome(sf::RenderWindow &window, sf::Font &font);
    void renderExplainGoal(sf::RenderWindow &window, sf::Font &font);
    void renderExplainCards(sf::RenderWindow &window, sf::Font &font);
    void renderExplainEnergy(sf::RenderWindow &window, sf::Font &font);
    void renderExplainActiveBar(sf::RenderWindow &window, sf::Font &font);
    void renderDemonstrateCombat(sf::RenderWindow &window, sf::Font &font);
    void renderExplainRooms(sf::RenderWindow &window, sf::Font &font);
    void renderNextButton(sf::RenderWindow &window, sf::Font &font);
    void simulateCombatAction();
};
