#include "AIStoryteller.h"
#include "Player.h"
#include "Enemy.h"
#include "Logger.h"
#include <iostream>
#include <cmath>

// Create singleton instance [Implementation]
AIStoryteller *AIStoryteller::instance = (AIStoryteller *)0;

// Constructor [Implementation]
AIStoryteller::AIStoryteller() : playerHealthRatio(1.0f), averageHealthRatio(1.0f), lowHealthCounter(0), totalGameTime(0.0f), combatTime(0.0f), currentStage(1), roomsCleared(0), combatsWon(0), combatsLost(0), consecutiveWins(0), consecutiveLosses(0), averageCombatDuration(0.0f), cardsPlayed(0), damageDealt(0), damageTaken(0), currentDeckSize(0), rareCardCount(0), currentBitcoin(0), currentTension(TensionLevel::BALANCED), pendingEvent(StoryEvent::NONE), tensionUpdateTimer(0.0f), eventTriggered(false), enemyStatMultiplier(1.0f), lootQualityMultiplier(1.0f), eventFrequency(1.0f)
{
}

// Destructor [Implementation]
AIStoryteller::~AIStoryteller()
{
}

// Get singleton instance [Implementation]
AIStoryteller *AIStoryteller::getInstance()
{
    if (!instance)
    {
        instance = new AIStoryteller();
    }
    return instance;
}

// Destroy singleton instance [Implementation]
void AIStoryteller::destroy()
{
    if (instance)
    {
        delete instance;
        instance = (AIStoryteller *)0;
    }
}

// Update AI state each frame [Implementation]
void AIStoryteller::update(float deltaTime, Player *player)
{
    if (!player)
    {
        return;
    }
    totalGameTime += deltaTime;
    tensionUpdateTimer += deltaTime;

    // Update health tracking
    playerHealthRatio = static_cast<float>(player->getHP()) / static_cast<float>(player->getMaxHP());

    // Running average of health (weighted towards recent)
    averageHealthRatio = (averageHealthRatio * 0.95f) + (playerHealthRatio * 0.05f);

    // Track low health duration
    if (playerHealthRatio < 0.3f)
    {
        lowHealthCounter++;
    }
    else
    {
        lowHealthCounter = 0;
    }

    // Update resource state
    currentBitcoin = player->getGold();
    // Note: currentDeckSize would need to be set when deck changes

    // Update tension level every 5 seconds
    if (tensionUpdateTimer >= 5.0f)
    {
        updateTensionLevel(player);
        calculateInfluenceMultipliers();
        checkForEventTriggers(player);
        tensionUpdateTimer = 0.0f;
    }
}

void AIStoryteller::updateTensionLevel(Player *)
{
    float difficultyScore = calculateDifficultyScore();

    TensionLevel oldTension = currentTension;
    // Determine tension based on multiple criteria
    if (difficultyScore < -0.3f)
    {
        // Player is doing too well - increase difficulty
        currentTension = TensionLevel::INTENSE;
    }
    else if (difficultyScore > 0.3f)
    {
        // Player is struggling - ease up
        currentTension = TensionLevel::RELAXED;
    }
    else if (currentStage >= 3)
    {
        // Final stage - make it dramatic
        currentTension = TensionLevel::CLIMACTIC;
    }
    else
    {
        currentTension = TensionLevel::BALANCED;
    }

    // Log tension changes
    if (oldTension != currentTension)
    {
        Logger::getInstance()->logAITensionChange(
            getTensionStringFromLevel(oldTension),
            getTensionStringFromLevel(currentTension),
            difficultyScore);
    }
}

std::string AIStoryteller::getTensionStringFromLevel(TensionLevel level) const
{
    switch (level)
    {
    case TensionLevel::RELAXED:
        return "RELAXED";
    case TensionLevel::BALANCED:
        return "BALANCED";
    case TensionLevel::INTENSE:
        return "INTENSE";
    case TensionLevel::CLIMACTIC:
        return "CLIMACTIC";
    default:
        return "UNKNOWN";
    }
}

// Calculate overall difficulty score based on stats [Implementation]
float AIStoryteller::calculateDifficultyScore()
{
    float score = 0.0f;

    if (averageHealthRatio < 0.4f)
        score += 0.3f;
    else if (averageHealthRatio > 0.8f)
        score -= 0.3f;

    if (consecutiveLosses >= 2)
        score += 0.4f;
    else if (consecutiveWins >= 3)
        score -= 0.4f;

    if (averageCombatDuration > 180.0f)
        score += 0.2f;
    else if (averageCombatDuration < 60.0f)
        score -= 0.2f;

    if (currentBitcoin < 50)
        score += 0.1f;
    else if (currentBitcoin > 200)
        score -= 0.1f;

    return score;
}

// Calculate influence multipliers based on tension level [Implementation]
void AIStoryteller::calculateInfluenceMultipliers()
{
    switch (currentTension)
    {
    case TensionLevel::RELAXED:
        enemyStatMultiplier = 0.85f;
        lootQualityMultiplier = 1.2f;
        eventFrequency = 1.5f;
        break;

    case TensionLevel::BALANCED:
        enemyStatMultiplier = 1.0f;
        lootQualityMultiplier = 1.0f;
        eventFrequency = 1.0f;
        break;

    case TensionLevel::INTENSE:
        enemyStatMultiplier = 1.3f;
        lootQualityMultiplier = 0.9f;
        eventFrequency = 0.7f;
        break;

    case TensionLevel::CLIMACTIC:
        enemyStatMultiplier = 1.4f;
        lootQualityMultiplier = 1.3f;
        eventFrequency = 2.0f;
        break;
    }
}

void AIStoryteller::checkForEventTriggers(Player *)
{
    if (eventTriggered)
    {
        return;
    }

    if (lowHealthCounter > 100 && playerHealthRatio < 0.3f)
    {
        pendingEvent = StoryEvent::HEALING_FOUNTAIN;
        eventTriggered = true;
        Logger::getInstance()->logAIEventTrigger("Healing Fountain",
                                                 "Player low HP for extended period (HP: " + std::to_string(static_cast<int>(playerHealthRatio * 100)) + "%)");
    }

    else if (consecutiveWins >= 4)
    {
        pendingEvent = StoryEvent::ELITE_ENCOUNTER;
        eventTriggered = true;
        Logger::getInstance()->logAIEventTrigger("Elite Encounter",
                                                 "Win streak detected (" + std::to_string(consecutiveWins) + " wins)");
    }
    else if (playerHealthRatio > 0.8f && currentBitcoin > 150)
    {
        pendingEvent = StoryEvent::CURSED_ENCOUNTER;
        eventTriggered = true;
        Logger::getInstance()->logAIEventTrigger("Cursed Encounter",
                                                 "Player too strong (HP: " + std::to_string(static_cast<int>(playerHealthRatio * 100)) + "%, Bitcoin: " + std::to_string(currentBitcoin) + ")");
    }
    else if (currentBitcoin < 30 && roomsCleared > 3)
    {
        pendingEvent = StoryEvent::BONUS_TREASURE;
        eventTriggered = true;
        Logger::getInstance()->logAIEventTrigger("Bonus Treasure",
                                                 "Low resources (Bitcoin: " + std::to_string(currentBitcoin) + ")");
    }
}

void AIStoryteller::influenceEnemyStats(Enemy *enemy, int)
{
    if (!enemy)
    {
        return;
    }
    int currentHP = enemy->getMaxHP();
    int currentDamage = enemy->getAttackDamage();
    int adjustedHP = static_cast<int>(currentHP * enemyStatMultiplier);
    int adjustedDamage = static_cast<int>(currentDamage * enemyStatMultiplier);
    if (adjustedHP < 1)
        adjustedHP = 1;
    if (adjustedDamage < 1)
        adjustedDamage = 1;
    enemy->setMaxHP(adjustedHP);
    enemy->setHP(adjustedHP);
    enemy->setAttackDamage(adjustedDamage);

    Logger::getInstance()->logAIInfluence(enemy->getName(), adjustedHP, adjustedDamage, enemyStatMultiplier);
}
// Get adjusted enemy health [Implementation]
int AIStoryteller::getAdjustedEnemyHealth(int baseHealth)
{
    int adjusted = static_cast<int>(baseHealth * enemyStatMultiplier);
    return adjusted < 1 ? 1 : adjusted;
}
// Get adjusted enemy damage [Implementation]
int AIStoryteller::getAdjustedEnemyDamage(int baseDamage)
{
    int adjusted = static_cast<int>(baseDamage * enemyStatMultiplier);
    return adjusted < 1 ? 1 : adjusted;
}
// Get enemy count based on tension [Implementation]
int AIStoryteller::getEnemyCount(int baseCount)
{
    // Add/remove enemies based on tension
    if (currentTension == TensionLevel::RELAXED && baseCount > 1)
        return baseCount - 1;
    else if (currentTension == TensionLevel::INTENSE)
        return baseCount + 1;

    return baseCount;
}
// Get card reward count based on tension [Implementation]
int AIStoryteller::getCardRewardCount(int baseCount)
{
    float multiplier = lootQualityMultiplier;

    if (multiplier >= 1.2f)
        return baseCount + 1;
    else if (multiplier <= 0.9f && baseCount > 1)
        return baseCount - 1;

    return baseCount;
}
// Determine if reward should be upgraded [Implementation]
bool AIStoryteller::shouldUpgradeReward()
{
    float chance = (lootQualityMultiplier - 1.0f) * 100.0f;
    float roll = static_cast<float>(rand() % 100);
    return roll < (10.0f + chance);
}

// Get adjusted bitcoin reward based on tension [Implementation]
int AIStoryteller::getAdjustedBitcoinReward(int baseAmount)
{
    int adjusted = static_cast<int>(baseAmount * lootQualityMultiplier);
    return adjusted < 1 ? 1 : adjusted;
}

// Determine if bonus card should be granted [Implementation]
bool AIStoryteller::shouldGrantBonusCard()
{
    if (currentTension == TensionLevel::RELAXED)
    {
        return (rand() % 100) < 20;
    }
    return false;
}

// Check for special event to trigger [Implementation]
AIStoryteller::StoryEvent AIStoryteller::checkForSpecialEvent()
{
    StoryEvent event = pendingEvent;
    pendingEvent = StoryEvent::NONE;
    return event;
}

// Get string representation of story event [Implementation]
void AIStoryteller::triggerEvent(StoryEvent event)
{
    (void)event;
}

// Determine if elite enemy should spawn [Implementation]
bool AIStoryteller::shouldSpawnEliteEnemy()
{
    bool shouldSpawn;
    if (consecutiveWins >= 3)
        shouldSpawn = (rand() % 100) < 40;
    else
        shouldSpawn = (rand() % 100) < 10;

    if (shouldSpawn)
    {
        Logger::getInstance()->logAIDecision("Spawn Elite Enemy",
                                             "Consecutive wins: " + std::to_string(consecutiveWins));
    }
    return shouldSpawn;
}

// Determine if extra rest should be offered [Implementation]
bool AIStoryteller::shouldOfferExtraRest()
{
    bool shouldOffer = (currentTension == TensionLevel::RELAXED) && (rand() % 100) < 30;
    if (shouldOffer)
    {
        Logger::getInstance()->logAIDecision("Offer Extra Rest",
                                             "Tension: " + getTensionString() + ", Player struggling");
    }
    return shouldOffer;
}

// Determine if discount should be given in shop [Implementation]
float AIStoryteller::getTreasureRoomChance(float baseChance)
{
    if (currentBitcoin < 50 || currentTension == TensionLevel::RELAXED)
        return baseChance * 1.5f;
    else if (currentBitcoin > 200)
        return baseChance * 0.7f;

    return baseChance;
}

// Determine if rest room should appear [Implementation]
float AIStoryteller::getRestRoomChance(float baseChance)
{
    if (averageHealthRatio < 0.5f || lowHealthCounter > 50)
        return baseChance * 1.8f;
    else if (averageHealthRatio > 0.9f)
        return baseChance * 0.6f;

    return baseChance;
}

// Determine if shop room should appear [Implementation]
float AIStoryteller::getShopRoomChance(float baseChance)
{
    return baseChance * eventFrequency;
}

void AIStoryteller::onCombatStart(int stage, int roomNumber)
{
    (void)roomNumber;
    currentStage = stage;
}

// End of combat event [Implementation]
void AIStoryteller::onCombatEnd(bool playerWon, float duration)
{
    combatTime += duration;

    if (playerWon)
    {
        combatsWon++;
        consecutiveWins++;
        consecutiveLosses = 0;
    }
    else
    {
        combatsLost++;
        consecutiveLosses++;
        consecutiveWins = 0;
    }
    int totalCombats = combatsWon + combatsLost;
    if (totalCombats > 0)
    {
        averageCombatDuration = combatTime / totalCombats;
    }
}

// Room cleared event [Implementation]
void AIStoryteller::onRoomCleared()
{
    roomsCleared++;
}

// Card played event [Implementation]
void AIStoryteller::onCardPlayed(int damage)
{
    cardsPlayed++;
    damageDealt += damage;
}

// Player damaged event [Implementation]
void AIStoryteller::onPlayerDamaged(int damage)
{
    damageTaken += damage;
}

// Stage completed event [Implementation]
void AIStoryteller::onStageComplete(int stage)
{
    (void)stage;
    eventTriggered = false;
}

// Player death event [Implementation]
void AIStoryteller::onPlayerDeath()
{

    currentTension = TensionLevel::RELAXED;
    calculateInfluenceMultipliers();
}

// Shop visit event [Implementation]
void AIStoryteller::onShopVisit(int bitcoinSpent)
{
    currentBitcoin -= bitcoinSpent;
}

// Card acquired event [Implementation]
void AIStoryteller::onCardAcquired(bool isRare)
{
    currentDeckSize++;
    if (isRare)
        rareCardCount++;
}

// Get string representation of tension level [Implementation]
std::string AIStoryteller::getTensionString() const
{
    switch (currentTension)
    {
    case TensionLevel::RELAXED:
        return "RELAXED";
    case TensionLevel::BALANCED:
        return "BALANCED";
    case TensionLevel::INTENSE:
        return "INTENSE";
    case TensionLevel::CLIMACTIC:
        return "CLIMACTIC";
    default:
        return "UNKNOWN";
    }
}
// Get current tension level [Implementation]
AIStoryteller::TensionLevel AIStoryteller::getCurrentTension() const
{
    return currentTension;
}
// Get influence multipliers [Implementation]
float AIStoryteller::getEnemyMultiplier() const
{
    return enemyStatMultiplier;
}
// Get loot quality multiplier [Implementation]
float AIStoryteller::getLootMultiplier() const
{
    return lootQualityMultiplier;
}
// Get consecutive wins [Implementation]
int AIStoryteller::getConsecutiveWins() const
{
    return consecutiveWins;
}
// Get consecutive losses [Implementation]
int AIStoryteller::getConsecutiveLosses() const
{
    return consecutiveLosses;
}
// Get player health ratio [Implementation]
float AIStoryteller::getAverageHealthRatio() const
{
    return averageHealthRatio;
}
// Get pending event [Implementation]
AIStoryteller::StoryEvent AIStoryteller::getPendingEvent() const
{
    return pendingEvent;
}
// Get current stage [Implementation]
int AIStoryteller::getCurrentStage() const
{
    return currentStage;
}
// Set current stage [Implementation]
void AIStoryteller::setCurrentStage(int stage)
{
    currentStage = stage;
    std::cout << "[AIStoryteller] Current stage set to: " << stage << std::endl;
}
// Get string representation of story event [Implementation]
std::string AIStoryteller::getEventString(StoryEvent event) const
{
    switch (event)
    {
    case StoryEvent::NONE:
        return "None";
    case StoryEvent::BONUS_TREASURE:
        return "Bonus Treasure";
    case StoryEvent::HEALING_FOUNTAIN:
        return "Healing Fountain";
    case StoryEvent::ELITE_ENCOUNTER:
        return "Elite Encounter";
    case StoryEvent::CURSED_ENCOUNTER:
        return "Cursed Encounter";
    case StoryEvent::MERCHANT_DISCOUNT:
        return "Merchant Discount";
    case StoryEvent::BOSS_WARNING:
        return "Boss Warning";
    default:
        return "Unknown";
    }
}
// Reset AI state [Implementation]
void AIStoryteller::reset()
{
    playerHealthRatio = 1.0f;
    averageHealthRatio = 1.0f;
    lowHealthCounter = 0;
    totalGameTime = 0.0f;
    combatTime = 0.0f;
    currentStage = 1;
    roomsCleared = 0;
    combatsWon = 0;
    combatsLost = 0;
    consecutiveWins = 0;
    consecutiveLosses = 0;
    averageCombatDuration = 0.0f;
    cardsPlayed = 0;
    damageDealt = 0;
    damageTaken = 0;
    currentDeckSize = 0;
    rareCardCount = 0;
    currentBitcoin = 0;
    currentTension = TensionLevel::BALANCED;
    pendingEvent = StoryEvent::NONE;
    tensionUpdateTimer = 0.0f;
    eventTriggered = false;
    enemyStatMultiplier = 1.0f;
    lootQualityMultiplier = 1.0f;
    eventFrequency = 1.0f;
}
