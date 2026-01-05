#include "Player.h"
#include <iostream>
#include <sstream>
#include "CardFactory.h"
#include <algorithm>
#include <random>

// Constructor [Implementation]
Player::Player() : Entity("Unknown", 100), roleName("None"), scenarioDescription(""), maxBaseCost(3), currentBaseCost(3), gold(100), spriteLoaded(false), currentFrame(0), hasFrames(false), bonusDrawNextTurn(0), isBossRoom(false)
{
    std::cout << "[Player] Default constructor (Starting gold: 100)" << std::endl;
}

// Parameterized Constructor [Implementation]
Player::Player(const std::string &playerName, const std::string &role, int health, int baseCost) : Entity(playerName, health), roleName(role), scenarioDescription(""), maxBaseCost(baseCost), currentBaseCost(baseCost), gold(100000), spriteLoaded(false), currentFrame(0), hasFrames(false), bonusDrawNextTurn(0), isBossRoom(false)
{
    setRoleBonuses(role);
    std::cout << "[Player] Created: " << name << " (" << roleName << ")" << std::endl;
    std::cout << "[Player] HP: " << maxHp << " | Base Cost: " << maxBaseCost << " | Gold: 100" << std::endl;
}

// Destructor [Implementation]
Player::~Player()
{
    std::cout << "[Player] Destroyed: " << name << std::endl;
}

void Player::update(float)
{
}

void Player::render(sf::RenderWindow &window)
{
    if (spriteLoaded && sprite)
    {
        sprite->setPosition(position); // position from Entity

        // If has animation frames, use current frame
        if (hasFrames)
        {
            sprite->setTextureRect(frameRects[currentFrame]);
        }

        window.draw(*sprite);
    }
}

// Take turn  [Implementation]
void Player::takeTurn()
{
    std::cout << "[Player] " << name << "'s turn!" << std::endl;
}

// Modify base cost  [Implementation]
void Player::modifyBaseCost(int amount)
{
    currentBaseCost += amount;
    if (currentBaseCost < 0)
        currentBaseCost = 0;
    if (currentBaseCost > maxBaseCost)
        currentBaseCost = maxBaseCost;
}

// set role name and apply bonuses
void Player::setRoleName(const std::string &role)
{
    roleName = role;
    setRoleBonuses(role);
}

// Get role bonuses
void Player::setRoleBonuses(const std::string &role)
{
    roleBonuses = RoleBonuses();

    if (role == "FRONTEND DEV")
    {
        roleBonuses.domTreeSpeed = 0.10f;
        std::cout << "[Player] Role Bonus: +10% DOM Tree Speed" << std::endl;
    }
    else if (role == "BACKEND DEV")
    {
        roleBonuses.hashMapLookup = 0.15f;
        std::cout << "[Player] Role Bonus: +15% HashMap Lookup" << std::endl;
    }
    else if (role == "FULL STACK DEV")
    {
        roleBonuses.stackOverflow = 0.20f;
        std::cout << "[Player] Role Bonus: +20% Stack Overflow" << std::endl;
    }
    else if (role == "DEVOPS ENGINEER")
    {
        roleBonuses.pipelineSpeed = 0.25f;
        std::cout << "[Player] Role Bonus: +25% Pipeline Speed" << std::endl;
    }
    else if (role == "DATA SCIENTIST")
    {
        roleBonuses.priorityQueue = 0.30f;
        std::cout << "[Player] Role Bonus: +30% Priority Queue" << std::endl;
    }
}

// Set scenario modifiers
void Player::setScenarioModifiers(float cardCost, float speed, int maxEnergyBonus, float damage, float reward)
{
    scenarioModifiers.cardCostModifier = cardCost;
    scenarioModifiers.speedModifier = speed;
    scenarioModifiers.maxEnergyBonus = maxEnergyBonus;
    scenarioModifiers.damageModifier = damage;
    scenarioModifiers.rewardModifier = reward;

    std::cout << "[Player] Scenario Modifiers Set:" << std::endl;
    std::cout << "  Card Cost: " << (cardCost * 100) << "%" << std::endl;
    std::cout << "  Speed: " << (speed * 100) << "%" << std::endl;
    std::cout << "  Max Energy Bonus: +" << maxEnergyBonus << std::endl;
    std::cout << "  Damage: " << (damage * 100) << "%" << std::endl;
    std::cout << "  Reward: " << (reward * 100) << "%" << std::endl;
}

// Load sprite from file
bool Player::loadSprite(const std::string &texturePath)
{
    if (spriteTexture.loadFromFile(texturePath))
    {
        sprite = std::make_unique<sf::Sprite>(spriteTexture);
        spriteLoaded = true;
        int frameWidth = 128;
        int frameHeight = 128;
        for (int i = 0; i < 4; i++)
        {
            frameRects[i] = sf::IntRect({i * frameWidth, 0}, {frameWidth, frameHeight});
        }
        hasFrames = true;
        currentFrame = 0;
        sprite->setTextureRect(frameRects[0]);
        sprite->setScale({0.8f, 0.8f});
        sprite->setOrigin({frameWidth / 2.0f, frameHeight / 2.0f});
        std::cout << "[Player] Sprite loaded: " << texturePath << std::endl;
        return true;
    }
    else
    {
        std::cout << "[Player] Failed to load sprite: " << texturePath << std::endl;
        spriteLoaded = false;
        return false;
    }
}

// Set sprite directly
void Player::setSprite(const sf::Texture &texture)
{
    sprite = std::make_unique<sf::Sprite>(texture);
    spriteLoaded = true;
}

// Set current animation frame
void Player::setCurrentFrame(int frame)
{
    if (frame >= 0 && frame < 4 && hasFrames)
    {
        currentFrame = frame;
        if (sprite)
        {
            sprite->setTextureRect(frameRects[currentFrame]);
        }
    }
}

// Set frame rectangle for animation frames
void Player::setFrameRect(int frameIndex, int x, int y, int width, int height)
{
    if (frameIndex >= 0 && frameIndex < 4)
    {
        frameRects[frameIndex] = sf::IntRect({x, y}, {width, height});
        if (frameIndex == 0 && sprite)
        {
            sprite->setOrigin({width / 2.0f, height / 2.0f});
        }
    }
}

// Get total card cost modifier
float Player::getTotalCardCostModifier() const
{
    float total = scenarioModifiers.cardCostModifier;

    // Pipeline speed reduces card cost
    if (roleBonuses.pipelineSpeed > 0)
    {
        total -= roleBonuses.pipelineSpeed * 0.5f;
    }

    return total;
}

// Get total speed modifier
float Player::getTotalSpeedModifier() const
{
    float total = scenarioModifiers.speedModifier;

    // Add role-specific speed bonuses
    total += roleBonuses.domTreeSpeed;
    total += roleBonuses.pipelineSpeed;

    return total;
}

// Reset player to default state
void Player::reset()
{
    name = "Unknown";
    roleName = "None";
    scenarioDescription = "";
    hp = maxHp;
    currentBaseCost = maxBaseCost;
    roleBonuses = RoleBonuses();
    scenarioModifiers = ScenarioModifiers();

    std::cout << "[Player] Reset to default state" << std::endl;
}

// Get player summary
std::string Player::getSummary() const
{
    std::stringstream ss;
    ss << "Player: " << name << "\n";
    ss << "Role: " << roleName << "\n";
    ss << "HP: " << hp << "/" << maxHp << "\n"; // Use Entity's hp/maxHp
    ss << "Base Cost: " << currentBaseCost << "/" << maxBaseCost << "\n";

    if (!scenarioDescription.empty())
    {
        ss << "Scenario: " << scenarioDescription << "\n";
    }

    // Show active bonuses
    if (roleBonuses.domTreeSpeed > 0)
        ss << "Bonus: +10% DOM Tree Speed\n";
    if (roleBonuses.hashMapLookup > 0)
        ss << "Bonus: +15% HashMap Lookup\n";
    if (roleBonuses.stackOverflow > 0)
        ss << "Bonus: +20% Stack Overflow\n";
    if (roleBonuses.pipelineSpeed > 0)
        ss << "Bonus: +25% Pipeline Speed\n";
    if (roleBonuses.priorityQueue > 0)
        ss << "Bonus: +30% Priority Queue\n";

    return ss.str();
}

// Calculate damage with bonuses
int Player::calculateDamage(int baseDamage) const
{
    float multiplier = 1.0f;

    multiplier += scenarioModifiers.damageModifier;

    multiplier += roleBonuses.hashMapLookup;

    multiplier += roleBonuses.stackOverflow;

    multiplier += roleBonuses.pipelineSpeed;

    if (redeemBuffs.onePunchMan)
    {
        multiplier += 1000000.0f;
    }

    int finalDamage = static_cast<int>(baseDamage * multiplier);

    finalDamage += strength;

    if (weakStacks > 0)
    {
        finalDamage = static_cast<int>(finalDamage * 0.75f);
    }

    return (finalDamage < 0) ? 0 : finalDamage;
}

// Calculate block with bonuses
int Player::calculateBlock(int baseBlock) const
{
    float multiplier = 1.0f;

    multiplier += roleBonuses.domTreeSpeed;

    multiplier += roleBonuses.priorityQueue;

    int finalBlock = static_cast<int>(baseBlock * multiplier);
    return (finalBlock < 0) ? 0 : finalBlock;
}

// Calculate card draw with bonuses
int Player::calculateCardDraw(int baseDraw) const
{
    int bonusDraw = 0;

    if (roleBonuses.stackOverflow >= 0.20f)
    {
        bonusDraw += static_cast<int>(roleBonuses.stackOverflow / 0.20f);
    }

    if (roleBonuses.priorityQueue >= 0.30f)
    {
        bonusDraw += 1;
    }

    return baseDraw + bonusDraw;
}

// Calculate energy cost with modifiers
int Player::calculateEnergyCost(int baseCost) const
{
    if (redeemBuffs.iterator)
    {
        return 1;
    }

    float multiplier = 1.0f;

    multiplier += scenarioModifiers.cardCostModifier;

    if (roleBonuses.pipelineSpeed > 0)
    {
        multiplier -= roleBonuses.pipelineSpeed;
    }

    if (roleBonuses.domTreeSpeed > 0)
    {
        multiplier -= roleBonuses.domTreeSpeed * 0.5f;
    }

    int finalCost = static_cast<int>(baseCost * multiplier);
    return (finalCost < 1) ? 1 : finalCost;
}

// Initialize starter deck with 30 random cards
void Player::initializeStarterDeck()
{
    std::cout << "[Player] Initializing starter deck (30 cards)..." << std::endl;

    clearDeck();

    std::vector<int> validIDs = {
        1, 2, 3, 4, 5, 6,
        101, 102, 103, 104,
        201, 202, 203, 204, 205,
        301, 302, 303, 304};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> indexDist(0, validIDs.size() - 1);

    for (int i = 0; i < 30; i++)
    {
        size_t randomIndex = indexDist(gen);
        int cardId = validIDs[randomIndex];
        Card *card = CardFactory::getInstance()->createCard(cardId);
        if (card)
        {
            masterDeck.push_back(card);
            std::cout << "[Player] Added: " << card->getName() << " (ID: " << cardId << ")" << std::endl;
        }
    }

    std::cout << "[Player] Starter deck initialized with " << masterDeck.size() << " cards!" << std::endl;
}

// Add card to deck
void Player::addCardToDeck(Card *card)
{
    if (card)
    {
        masterDeck.push_back(card);
        std::cout << "[Player] Added " << card->getName() << " to deck. Total: " << masterDeck.size() << std::endl;
    }
}

// Remove card from deck
void Player::removeCardFromDeck(Card *card)
{
    if (!card)
        return;

    auto it = std::find(masterDeck.begin(), masterDeck.end(), card);
    if (it != masterDeck.end())
    {
        masterDeck.erase(it);
        std::cout << "[Player] Removed " << card->getName() << " from deck. Total: " << masterDeck.size() << std::endl;
    }
}

// Clear entire deck
void Player::clearDeck()
{
    // Delete all cards
    for (Card *card : masterDeck)
    {
        delete card;
    }
    masterDeck.clear();
    std::cout << "[Player] Deck cleared." << std::endl;
}

// Get role name  [Implementation]
std::string Player::getRoleName() const
{
    return roleName;
}
// Get scenario description  [Implementation]
std::string Player::getScenarioDescription() const
{
    return scenarioDescription;
}
// Get max base cost  [Implementation]
int Player::getMaxBaseCost() const
{
    return maxBaseCost;
}
// Get current base cost  [Implementation]
int Player::getCurrentBaseCost() const
{
    return currentBaseCost;
}
// Get role bonuses  [Implementation]
const Player::RoleBonuses &Player::getRoleBonuses() const
{
    return roleBonuses;
}
// Get scenario modifiers  [Implementation]
const Player::ScenarioModifiers &Player::getScenarioModifiers() const
{
    return scenarioModifiers;
}
// Get redeem buffs  [Implementation]
Player::RedeemBuffs &Player::getRedeemBuffs()
{
    return redeemBuffs;
}
// Get redeem buffs (const)  [Implementation]
const Player::RedeemBuffs &Player::getRedeemBuffs() const
{
    return redeemBuffs;
}
// Set name  [Implementation]
void Player::setName(const std::string &playerName)
{
    name = playerName;
}
// Set scenario description  [Implementation]
void Player::setScenarioDescription(const std::string &desc)
{
    scenarioDescription = desc;
}
// Set max health  [Implementation]
void Player::setMaxHealth(int health)
{
    maxHp = health;
    hp = health;
}
// Set max base cost  [Implementation]
void Player::setMaxBaseCost(int cost)
{
    maxBaseCost = cost;
    currentBaseCost = cost;
}
// Refresh base cost to max  [Implementation]
void Player::refreshBaseCost()
{
    currentBaseCost = maxBaseCost;
}
// Get gold  [Implementation]
int Player::getGold() const
{
    return gold;
}
// Add gold  [Implementation]
void Player::addGold(int amount)
{
    gold += amount;
}
// Spend gold  [Implementation]
void Player::spendGold(int amount)
{
    if (gold >= amount)
        gold -= amount;
}
// Set gold  [Implementation]
void Player::setGold(int amount)
{
    gold = amount;
}
// Get master deck (non-const)  [Implementation]
std::vector<Card *> &Player::getMasterDeck()
{
    return masterDeck;
}
// Get master deck (const)  [Implementation]
const std::vector<Card *> &Player::getMasterDeck() const
{
    return masterDeck;
}
// Get current frame  [Implementation]
int Player::getCurrentFrame() const
{
    return currentFrame;
}
int Player::getDeckSize() const
{
    return static_cast<int>(masterDeck.size());
}

void Player::addBonusDrawNextTurn(int amount)
{
    bonusDrawNextTurn += amount;
}

int Player::getBonusDrawNextTurn() const
{
    return bonusDrawNextTurn;
}

void Player::resetBonusDrawNextTurn()
{
    bonusDrawNextTurn = 0;
}

void Player::setIsBossRoom(bool value)
{
    isBossRoom = value;
}

bool Player::getIsBossRoom() const
{
    return isBossRoom;
}