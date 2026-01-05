#pragma once

#include "Screen.h"
#include "Player.h"
#include "Enemy.h"
#include "Card.h"
#include "TurnQueue.h"
#include "ActiveBar.h"
#include "UndoStack.h"
#include "CombatLogNode.h"
#include "CardHashTable.h"
#include "AIStoryteller.h"
#include <vector>
#include <unordered_map>
#include <SFML/Graphics.hpp>

class CombatScreen : public Screen
{
private:
    // Reference to the player
    Player *playerRef;
    // Store the player's base maxBaseCost before scenario modifiers
    int baseMaxBaseCost;
    // List of enemies in combat
    std::vector<Enemy *> enemies;
    // Hash table for enemy name to Enemy* list mapping
    std::unordered_map<std::string, std::vector<Enemy *>> enemyHashTable;
    // Turn order queue
    TurnQueue turnQueue;
    // Current entity whose turn it is
    Entity *currentTurnEntity;
    // Card piles
    std::vector<Card *> hand;
    // Draw pile
    std::vector<Card *> drawPile;
    // Discard pile
    std::vector<Card *> discardPile;
    // Hash table for hand cards (name to Card* list)
    CardHashTable *handHashTable;
    // Active Bar for card
    ActiveBar activeBar;
    // Undo stack for card plays
    UndoStack undoStack;
    // Number of turns elapsed
    int turnNumber;
    // Hovered card index in hand
    int hoveredCardIndex;
    // Hovered active bar card index
    int hoveredActiveBarIndex;
    // Hovered button index (-1 if none)
    int hoveredButton;
    // Mouse pressed state
    bool mousePressed;
    // Turn animation timer
    float turnAnimationTimer;
    // Dragged card from hand
    Card *draggedCard;
    // Index of dragged card in hand
    int draggedCardIndex;
    // Offset within card where mouse grabbed
    sf::Vector2f dragOffset;
    // Dragged card from Active Bar
    Card *draggedActiveBarCard;
    // Position index of dragged Active Bar card
    int draggedActiveBarPosition;
    // Hovered slot index in Active Bar
    int hoveredSlotIndex;
    // Legend panel state
    bool legendPanelOpen;
    // Legend panel offset
    float legendPanelOffsetX;
    // Legend panel width constants
    const float LEGEND_PANEL_WIDTH = 350.0f;
    // Legend panel slide speed
    const float LEGEND_SLIDE_SPEED = 1500.0f;
    // Card detail popup state
    bool showCardDetail;
    // Selected card for detail popup
    Card *selectedCardForDetail;
    // Card detail popup alpha for fade in/out
    float cardDetailAlpha;
    // Fade speed for popup
    const float FADE_SPEED = 800.0f;
    // Player texture
    sf::Texture playerTexture;
    // Player sprite
    sf::Sprite *playerSprite;
    // Current frame for player animation
    int currentFrame;
    // Animation timer for player sprite
    float animationTimer;
    // Frame time for player animation
    const float FRAME_TIME = 0.5f;
    // Enemy textures map (name to texture)
    std::unordered_map<std::string, sf::Texture> enemyTextures;
    // Enemy sprite
    sf::Sprite *enemySprite;
    // Current frame for Dr. Mark
    int drMarkFrame;
    // Animation timer for Dr. Mark sprite
    float drMarkAnimationTimer;
    // Combat log linked list
    CombatLogList *combatLog;
    // Maximum log entries to display
    int maxLogEntries;
    // Scroll offset for combat log
    int logScrollOffset;
    // Scroll offset for buff/debuff display
    int buffScrollOffset;
    // constant for max visible buffs
    const int MAX_VISIBLE_BUFFS = 5;
    // Card width
    static constexpr float CARD_WIDTH = 180.0f;
    // Card height
    static constexpr float CARD_HEIGHT = 250.0f;
    // Card spacing in hand
    static constexpr float CARD_SPACING = 20.0f;
    // Maximum hand size
    static constexpr int MAX_HAND_SIZE = 5;

public:
    // Constructor [Header]
    CombatScreen(Game *gameInstance);
    // Destructor [Header]
    ~CombatScreen();
    // Navigate to this screen override [Header]
    void onEnter() override;
    // Leave this screen override [Header]
    void onExit() override;
    // Handle events override [Header]
    void handleEvents() override;
    // Update screen override [Header]
    void update(float deltaTime) override;
    // Render screen override [Header]
    void render(sf::RenderWindow &window, sf::Font &font) override;
    // Get enemies by name from hash table
    std::vector<Enemy *> getEnemiesByName(const std::string &name);

private:
    // Start combat setup [Header]
    void startCombat();
    // Create enemies for combat [Header]
    void createEnemies(int count, int level);
    // Start a new turn [Header]
    void startTurn();
    // End the player's turn [Header]
    void endPlayerTurn();
    // Execute enemy turn [Header]
    void executeEnemyTurn(Enemy *enemy);
    // Draw cards to player's hand [Header]
    void drawCards(int count);
    // Play a card from hand [Header]
    void playCard(int cardIndex);
    // Discard player's hand [Header]
    void discardHand();
    // Check if combat has ended [Header]
    bool checkCombatEnd();
    // Add card to Active Bar [Header]
    void addCardToActiveBar(int handIndex, int slotPosition = -1);
    // Remove card from Active Bar [Header]
    void removeCardFromActiveBar(Card *card);
    // Execute cards in Active Bar [Header]
    void executeActiveBar();
    // Undo last played card [Header]
    void undoLastCard();
    // Load player sprite and setup animation [Header]
    void loadPlayerSprite();
    // Draw Turn Order Bar [Header]
    void drawTurnOrderBar(sf::RenderWindow &window, sf::Font &font);
    // Draw Active Bar [Header]
    void drawActiveBar(sf::RenderWindow &window, sf::Font &font);
    // Draw a single card [Header]
    void drawCard(sf::RenderWindow &window, sf::Font &font, Card *card, float x, float y, bool isHovered);
    /// Draw health bar for entity [Header]
    void drawHealthBar(sf::RenderWindow &window, sf::Font &font, float x, float y, float width, int current, int max, const std::string &label);
    // Draw defense (block) bar for entity [Header]
    void drawDefenseBar(sf::RenderWindow &window, sf::Font &font, float x, float y, float width, int blockAmount);
    // Draw energy display for player [Header]
    void drawEnergyDisplay(sf::RenderWindow &window, sf::Font &font);
    // Draw entity (player/enemy) [Header]
    void drawEntity(sf::RenderWindow &window, sf::Font &font, Entity *entity, float x, float y, bool isActive);
    // Check if mouse is over card [Header]
    bool isMouseOverCard(float cardX, float cardY, sf::Vector2f mousePos);
    // Check if mouse is over button [Header]
    bool isMouseOverButton(float x, float y, float width, float height, sf::Vector2f mousePos);
    // Draw legend panel [Header]
    void drawLegendPanel(sf::RenderWindow &window, sf::Font &font);
    // Draw card detail popup [Header]
    void drawCardDetailPopup(sf::RenderWindow &window, sf::Font &font);
    // Load enemy textures from files [Header]
    void loadEnemyTextures();
    // Build enemy hash table for quick lookup [Header]
    void buildEnemyHashTable();
    // Print hash table for debugging [Header]
    void printHashTableDebug() const;
    // Check for card combos [Header]
    void checkForCombos();
    // Add entry to combat log [Header]
    void addCombatLog(const std::string &message);
    // Draw combat log on screen [Header]
    void drawCombatLog(sf::RenderWindow &window, sf::Font &font);
    // Create starter deck for player [Header]
    void createStarterDeck();
};
