#pragma once

#include <string>
#include <vector>

// Forward declarations for Player and Enemy classes to avoid circular dependencies due to they including AIStoryteller.h
class Player;
class Enemy;

class AIStoryteller
{
public:
    // Different tension levels the AI can set
    enum class TensionLevel
    {
        RELAXED,
        BALANCED,
        INTENSE,
        CLIMACTIC
    };
    // Different story events the AI can trigger
    enum class StoryEvent
    {
        NONE,
        // Treasure room with extra rewards
        BONUS_TREASURE,
        // Healing fountain appears
        HEALING_FOUNTAIN,
        // Elite enemy encounter
        ELITE_ENCOUNTER,
        // Cursed enemy encounter
        CURSED_ENCOUNTER,
        // Merchant offers discounts
        MERCHANT_DISCOUNT,
        // Warning before boss fight
        BOSS_WARNING
    };

private:
    // Innitialise the singleton instance
    static AIStoryteller *instance;
    // Constructor
    AIStoryteller();
    // Copy constructor and assignment operator (deleted)
    AIStoryteller(const AIStoryteller &) = delete;
    // Assignment operator
    AIStoryteller &operator=(const AIStoryteller &) = delete;
    // Track the player's hp ratio over time
    float playerHealthRatio;
    // Track the player's average hp ratio
    float averageHealthRatio;
    // Track how long player has been low on health
    int lowHealthCounter;
    // Track total game time
    float totalGameTime;
    // Track total combat time
    float combatTime;
    // Track current stage and room info
    int currentStage;
    // Track rooms cleared
    int roomsCleared;
    // Track combats won
    int combatsWon;
    // Track combats lost
    int combatsLost;
    // Track consecutive wins
    int consecutiveWins;
    // Track consecutive losses
    int consecutiveLosses;
    // Track average combat duration
    float averageCombatDuration;
    // Track cards played
    int cardsPlayed;
    // Track damage dealt
    int damageDealt;
    // Track damage taken
    int damageTaken;
    // Track deck size
    int currentDeckSize;
    // Track rare card count
    int rareCardCount;
    // Track current bitcoin (currency)
    int currentBitcoin;
    // Current tension level
    TensionLevel currentTension;
    // Pending event to trigger
    StoryEvent pendingEvent;
    // Timer for tension updates
    float tensionUpdateTimer;
    bool eventTriggered;
    // Influence multipliers (how much AI affects game)
    float enemyStatMultiplier;
    float lootQualityMultiplier;
    float eventFrequency;

public:
    // Singleton access [Header]
    static AIStoryteller *getInstance();
    // Destroy singleton [Header]
    static void destroy();
    // Destructor
    ~AIStoryteller();
    // Update AI state each frame [Header]
    void update(float deltaTime, Player *player);
    // Start of combat event [Header]
    void onCombatStart(int stage, int roomNumber);
    // End of combat event [Header]
    void onCombatEnd(bool playerWon, float duration);
    // Room cleared event [Header]
    void onRoomCleared();
    // Card played event [Header]
    void onCardPlayed(int damage);
    // Player damaged event [Header]
    void onPlayerDamaged(int damage);
    // Stage complete event [Header]
    void onStageComplete(int stage);
    // Player death event [Header]
    void onPlayerDeath();
    // Shop visit event [Header]
    void onShopVisit(int bitcoinSpent);
    // Card acquired event [Header]
    void onCardAcquired(bool isRare);
    // Influence enemy stats based on tension [Header]
    void influenceEnemyStats(Enemy *enemy, int baseLevel);
    // Get adjusted enemy health [Header]
    int getAdjustedEnemyHealth(int baseHealth);
    // Get adjusted enemy damage [Header]
    int getAdjustedEnemyDamage(int baseDamage);
    // Get enemy count based on tension [Header]
    int getEnemyCount(int baseCount);
    // Get card reward count based on tension [Header]
    int getCardRewardCount(int baseCount);
    // Determine if reward should be upgraded [Header]
    bool shouldUpgradeReward();
    // Get adjusted bitcoin reward based on tension [Header]
    int getAdjustedBitcoinReward(int baseAmount);
    // Determine if bonus card should be granted [Header]
    bool shouldGrantBonusCard();
    // Check for special event triggers [Header]
    StoryEvent checkForSpecialEvent();
    // Trigger a specific event [Header]
    void triggerEvent(StoryEvent event);
    // Determine if elite enemy should spawn [Header]
    bool shouldSpawnEliteEnemy();
    // Determine if extra rest should be offered [Header]
    bool shouldOfferExtraRest();
    // get treasure room chance based on tension [Header]
    float getTreasureRoomChance(float baseChance);
    // get rest room chance based on tension [Header]
    float getRestRoomChance(float baseChance);
    // get shop room chance based on tension [Header]
    float getShopRoomChance(float baseChance);
    // Get current tension level [Header]
    TensionLevel getCurrentTension() const;
    // Get string representation of tension level [Header]
    std::string getTensionString() const;
    // Get string representation of story event [Header]
    std::string getEventString(StoryEvent event) const;
    // Get influence multipliers [Header]
    float getEnemyMultiplier() const;
    // Get loot quality multiplier [Header]
    float getLootMultiplier() const;
    // Get consecutive wins [Header]
    int getConsecutiveWins() const;
    // Get consecutive losses [Header]
    int getConsecutiveLosses() const;
    // Get player health ratio [Header]
    float getAverageHealthRatio() const;
    // Get pending event [Header]
    StoryEvent getPendingEvent() const;
    // Get current stage [Header]
    int getCurrentStage() const;
    // Set current stage [Header]
    void setCurrentStage(int stage);
    // Reset for new game
    void reset();

private:
    // Update tension level based on player performance [Header]
    void updateTensionLevel(Player *player);
    // Calculate influence multipliers based on tension level [Header]
    void calculateInfluenceMultipliers();
    // Check for event triggers based on game state [Header]
    void checkForEventTriggers(Player *player);
    // Calculate overall difficulty score based on stats [Header]
    float calculateDifficultyScore();
    // Helper to get tension string from level
    std::string getTensionStringFromLevel(TensionLevel level) const;
};
