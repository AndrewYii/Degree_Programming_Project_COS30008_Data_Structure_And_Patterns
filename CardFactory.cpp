#include "CardFactory.h"
#include "AttackCard.h"
#include "DefenseCard.h"
#include "SkillCard.h"
#include "PowerCard.h"
#include "BinarySearchCard.h"
#include "NeuralNetworkCard.h"
#include "CacheHitCard.h"
#include "GarbageCollectionCard.h"
#include "LoadBalancerCard.h"
#include <iostream>
#include <random>

// Initialise static instance pointer [Implementation]
CardFactory *CardFactory::instance = static_cast<CardFactory *>(0);
// Constructor [Implementation]
CardFactory::CardFactory()
{
}
// getInstance - Singleton access [Implementation]
CardFactory *CardFactory::getInstance()
{
    if (!instance)
    {
        instance = new CardFactory();
    }
    return instance;
}
// Destructor - Cleanup prototypes [Implementation]
CardFactory::~CardFactory()
{
    clearRegistry();
}
// Register a card prototype in the registry [Implementation]
void CardFactory::registerCard(Card *cardPrototype)
{
    if (!cardPrototype)
        return;

    int cardID = cardPrototype->getID();
    std::string cardName = cardPrototype->getName();
    if (cardRegistry.find(cardID) != cardRegistry.end())
    {
        std::cout << "[FACTORY] Warning: Card ID " << cardID << " already registered!\n";
        return;
    }
    cardRegistry[cardID] = cardPrototype;
    nameToID[cardName] = cardID;
    std::cout << "[FACTORY] Registered card: " << cardName << " (ID: " << cardID << ")\n";
}
// Clear registry (cleanup) [Implementation]
Card *CardFactory::createCard(int cardID)
{
    // Lookup in hash table
    auto it = cardRegistry.find(cardID);

    if (it != cardRegistry.end())
    {
        // Clone the prototype
        Card *prototype = it->second;
        return prototype->clone();
    }

    std::cout << "[FACTORY] Error: Card ID " << cardID << " not found!\n";
    return static_cast<Card *>(0);
}
// Create card by Name [Implementation]
Card *CardFactory::createCard(const std::string &cardName)
{
    // Lookup name in hash table
    auto it = nameToID.find(cardName);

    if (it != nameToID.end())
    {
        int cardID = it->second;
        return createCard(cardID);
    }

    std::cout << "[FACTORY] Error: Card '" << cardName << "' not found!\n";
    return static_cast<Card *>(0);
}
// Get random card by rarity [Implementation]
Card *CardFactory::getRandomCard(Card::CardRarity rarity)
{
    std::vector<Card *> matchingCards;
    for (auto &pair : cardRegistry)
    {
        if (pair.second->getRarity() == rarity)
        {
            matchingCards.push_back(pair.second);
        }
    }
    if (matchingCards.empty())
    {
        return static_cast<Card *>(0);
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(0, matchingCards.size() - 1);
    size_t randomIndex = dis(gen);
    return matchingCards[randomIndex]->clone();
}

// Get all cards of a specific type [Implementation]
std::vector<Card *> CardFactory::getCardsByType(Card::CardType type)
{
    std::vector<Card *> result;

    for (auto &pair : cardRegistry)
    {
        if (pair.second->getType() == type)
        {
            result.push_back(pair.second->clone());
        }
    }

    return result;
}
// Initialise card database [Implementation]
void CardFactory::initializeCardDatabase()
{
    std::cout << "[FACTORY] Initializing card database...\n";
    initAttackCards();
    initDefenseCards();
    initSkillCards();
    initPowerCards();
    std::cout << "[FACTORY] Initialized " << cardRegistry.size() << " cards!\n";
}

// Initialize attack cards [Implementation]
void CardFactory::initAttackCards()
{
    // STACK CARD - Strike (LIFO pattern for bonus)
    AttackCard *strike = new AttackCard(
        1, "Stack Strike",
        "[Cost: 1] Deal 6 damage. STACK: +3 bonus if LIFO pattern.",
        Card::CardRarity::COMMON, 1, 6);
    strike->setDataType(Card::CardDataType::STACK);
    registerCard(strike);

    // QUEUE CARD - Heavy Strike (FIFO pattern for bonus)
    AttackCard *heavyStrike = new AttackCard(
        2, "Queue Strike",
        "[Cost: 2] Deal 14 damage. QUEUE: +2 bonus if FIFO pattern.",
        Card::CardRarity::COMMON, 2, 14);
    heavyStrike->setDataType(Card::CardDataType::QUEUE);
    registerCard(heavyStrike);

    // TREE CARD - Code Slash (branches to multiple)
    AttackCard *codeSlash = new AttackCard(
        3, "Tree Slash",
        "[Cost: 2] Deal 9 damage. TREE: Branches to adjacent enemies.",
        Card::CardRarity::UNCOMMON, 2, 9);
    codeSlash->setDataType(Card::CardDataType::TREE);
    registerCard(codeSlash);

    // HASH CARD - Syntax Error (hash grouping)
    AttackCard *syntaxError = new AttackCard(
        4, "Hash Attack",
        "[Cost: 1] Deal 3 damage. HASH: Hits all enemies with same name.",
        Card::CardRarity::COMMON, 1, 3);
    syntaxError->setDataType(Card::CardDataType::HASH_TABLE);
    registerCard(syntaxError);

    // DLL CARD - Critical Bug (forward+backward)
    AttackCard *criticalBug = new AttackCard(
        5, "DLL Strike",
        "[Cost: 3] Deal 20 damage. DLL: Hits twice (forward+backward).",
        Card::CardRarity::RARE, 3, 20);
    criticalBug->setDataType(Card::CardDataType::DLL);
    registerCard(criticalBug);

    // NODE CARD - Recursive Strike (chains with others)
    AttackCard *recursiveStrike = new AttackCard(
        6, "Node Strike",
        "[Cost: 2] Deal 10 damage. NODE: Chains with other cards.",
        Card::CardRarity::UNCOMMON, 2, 10);
    recursiveStrike->setDataType(Card::CardDataType::NODE);
    registerCard(recursiveStrike);

    // EPIC CARDS
    // HASH CARD - Concurrent Attack (epic hash)
    AttackCard *concurrentAttack = new AttackCard(
        7, "Concurrent Blast",
        "[Cost: 3] Deal 18 damage. HASH: Hits all enemies with same name.",
        Card::CardRarity::EPIC, 3, 18);
    concurrentAttack->setDataType(Card::CardDataType::HASH_TABLE);
    registerCard(concurrentAttack);

    // TREE CARD - Recursive Bomb (epic tree)
    AttackCard *recursiveBomb = new AttackCard(
        8, "Recursive Bomb",
        "[Cost: 4] Deal 25 damage. TREE: Branches to ALL enemies.",
        Card::CardRarity::EPIC, 4, 25);
    recursiveBomb->setDataType(Card::CardDataType::TREE);
    registerCard(recursiveBomb);

    // LEGENDARY CARDS
    // DLL CARD - Quantum Strike (legendary dll)
    AttackCard *quantumStrike = new AttackCard(
        9, "Quantum Strike",
        "[Cost: 5] Deal 40 damage. DLL: Hits 3 times (forward+backward+pivot).",
        Card::CardRarity::LEGENDARY, 5, 40);
    quantumStrike->setDataType(Card::CardDataType::DLL);
    registerCard(quantumStrike);

    // STACK CARD - Overflow Attack (legendary stack)
    AttackCard *overflowAttack = new AttackCard(
        10, "Stack Overflow",
        "[Cost: 4] Deal 35 damage. STACK: +10 bonus if LIFO pattern.",
        Card::CardRarity::LEGENDARY, 4, 35);
    overflowAttack->setDataType(Card::CardDataType::STACK);
    registerCard(overflowAttack);
}

// Initialize defense cards [Implementation]
void CardFactory::initDefenseCards()
{
    // Defend (Starter) - (id, name, desc, rarity, cost, block)
    DefenseCard *defend = new DefenseCard(
        101, "Defend", "[Cost: 1] Gain 5 Block.",
        Card::CardRarity::COMMON, 1, 5);
    defend->setDataType(Card::CardDataType::STACK);
    registerCard(defend);

    // Try-Catch Block
    DefenseCard *tryCatchBlock = new DefenseCard(
        102, "Try-Catch Block", "[Cost: 2] Gain 12 Block. Prevents one debuff this turn.",
        Card::CardRarity::UNCOMMON, 2, 12);
    tryCatchBlock->setDataType(Card::CardDataType::QUEUE);
    registerCard(tryCatchBlock);

    // Firewall
    DefenseCard *firewall = new DefenseCard(
        103, "Firewall", "[Cost: 3] Gain 20 Block.",
        Card::CardRarity::RARE, 3, 20);
    firewall->setDataType(Card::CardDataType::TREE);
    registerCard(firewall);

    // Code Review
    DefenseCard *codeReview = new DefenseCard(
        104, "Code Review", "[Cost: 2] Gain 8 Block. Draw 1 card.",
        Card::CardRarity::COMMON, 2, 8);
    codeReview->setDataType(Card::CardDataType::HASH_TABLE);
    registerCard(codeReview);

    // EPIC DEFENSE CARDS
    // Encryption
    DefenseCard *encryption = new DefenseCard(
        105, "Encryption", "[Cost: 3] Gain 25 Block. Prevents all debuffs this turn.",
        Card::CardRarity::EPIC, 3, 25);
    encryption->setDataType(Card::CardDataType::TREE);
    registerCard(encryption);

    // LEGENDARY DEFENSE CARDS
    // Perfect Security
    DefenseCard *perfectSecurity = new DefenseCard(
        106, "Perfect Security", "[Cost: 4] Gain 35 Block. Immune to damage next turn.",
        Card::CardRarity::LEGENDARY, 4, 35);
    perfectSecurity->setDataType(Card::CardDataType::DLL);
    registerCard(perfectSecurity);
}

// Initialize skill cards [Implementation]
void CardFactory::initSkillCards()
{

    BinarySearchCard *binarySearch = new BinarySearchCard(
        201, "Binary Search", "[Cost: 2] Find weakness. Deal 12 CRITICAL damage.",
        Card::CardRarity::UNCOMMON, 2, 12);
    registerCard(binarySearch);

    NeuralNetworkCard *neuralNet = new NeuralNetworkCard(
        202, "Neural Network", "[Cost: 2] Adaptive damage. +2 per time played.",
        Card::CardRarity::RARE, 2, 8);
    registerCard(neuralNet);

    CacheHitCard *cacheHit = new CacheHitCard(
        203, "Cache Hit", "[Cost: 0] Fast access! Draw 2 cards next turn.",
        Card::CardRarity::COMMON, 0, 2);
    registerCard(cacheHit);

    GarbageCollectionCard *garbageCollection = new GarbageCollectionCard(
        204, "Garbage Collection", "[Cost: 1] Remove all debuffs.",
        Card::CardRarity::UNCOMMON, 1);
    registerCard(garbageCollection);

    LoadBalancerCard *loadBalancer = new LoadBalancerCard(
        205, "Load Balancer", "[Cost: 2] Distribute 15 damage.",
        Card::CardRarity::RARE, 2, 15);
    registerCard(loadBalancer);

    // EPIC SKILL CARDS
    // Quantum Computing
    NeuralNetworkCard *quantumComputing = new NeuralNetworkCard(
        206, "Quantum Computing", "[Cost: 3] Adaptive damage. +4 per time played.",
        Card::CardRarity::EPIC, 3, 15);
    registerCard(quantumComputing);

    // LEGENDARY SKILL CARDS
    // Master Algorithm
    BinarySearchCard *masterAlgorithm = new BinarySearchCard(
        207, "Master Algorithm", "[Cost: 4] Perfect weakness exploit. Deal 30 CRITICAL damage.",
        Card::CardRarity::LEGENDARY, 4, 30);
    registerCard(masterAlgorithm);
}

// Initialize power cards [Implementation]
void CardFactory::initPowerCards()
{

    // Agile Development
    PowerCard *agileDev = new PowerCard(
        301, "Agile Development", "[Cost: 1] Gain 2 Dexterity (+2 Block per card).",
        Card::CardRarity::UNCOMMON, 1, 0, 2, -1);
    agileDev->setDataType(Card::CardDataType::QUEUE);
    registerCard(agileDev);

    // Pair Programming
    PowerCard *pairProgramming = new PowerCard(
        302, "Pair Programming", "[Cost: 1] Gain 2 Strength (+2 Damage per card).",
        Card::CardRarity::UNCOMMON, 1, 2, 0, -1);
    pairProgramming->setDataType(Card::CardDataType::STACK);
    registerCard(pairProgramming);

    // Clean Code
    PowerCard *cleanCode = new PowerCard(
        303, "Clean Code", "[Cost: 2] Gain 1 Strength and 1 Dexterity.",
        Card::CardRarity::RARE, 2, 1, 1, -1);
    cleanCode->setDataType(Card::CardDataType::TREE);
    registerCard(cleanCode);

    // Sprint Mode
    PowerCard *sprintMode = new PowerCard(
        304, "Sprint Mode", "Gain 3 Strength for 3 turns.",
        Card::CardRarity::UNCOMMON, 2, 3, 0, 3);
    sprintMode->setDataType(Card::CardDataType::DLL);
    registerCard(sprintMode);

    // EPIC POWER CARDS
    // Compiler Optimization
    PowerCard *compilerOpt = new PowerCard(
        305, "Compiler Optimization", "[Cost: 3] Gain 3 Strength and 3 Dexterity.",
        Card::CardRarity::EPIC, 3, 3, 3, -1);
    compilerOpt->setDataType(Card::CardDataType::HASH_TABLE);
    registerCard(compilerOpt);

    // LEGENDARY POWER CARDS
    // Ultimate Algorithm
    PowerCard *ultimateAlgo = new PowerCard(
        306, "Ultimate Algorithm", "[Cost: 4] Gain 5 Strength and 5 Dexterity permanently.",
        Card::CardRarity::LEGENDARY, 4, 5, 5, -1);
    ultimateAlgo->setDataType(Card::CardDataType::NODE);
    registerCard(ultimateAlgo);
}

// Clear registry (cleanup) [Implementation]
void CardFactory::clearRegistry()
{
    // Delete all prototype cards
    for (auto &pair : cardRegistry)
    {
        delete pair.second;
    }

    cardRegistry.clear();
    nameToID.clear();
}

// Debug: Print all registered cards [Implementation]
void CardFactory::printCardRegistry()
{
    std::cout << "=== CARD REGISTRY (" << cardRegistry.size() << " cards) ===\n";

    for (auto &pair : cardRegistry)
    {
        Card *card = pair.second;
        std::cout << "[" << card->getID() << "] " << card->getName()
                  << " - " << card->getDescription() << "\n";
    }

    std::cout << "==============================\n";
}
