#pragma once

#include "Card.h"
#include <unordered_map>
#include <vector>
#include <string>

class CardFactory
{
private:
    // Singleton instance
    static CardFactory *instance;
    // Hash table for card prototypes (ID -> Card*)
    std::unordered_map<int, Card *> cardRegistry;
    // Hash table for name lookup (Name -> ID)
    std::unordered_map<std::string, int> nameToID;
    // Private constructor for Singleton
    CardFactory();
    // Prevent copying
    CardFactory(const CardFactory &) = delete;
    // Prevent assignment
    CardFactory &operator=(const CardFactory &) = delete;

public:
    // Get singleton instance [Header]
    static CardFactory *getInstance();
    // Destructor [Header]
    ~CardFactory();
    // Register a card prototype in the registry [Header]
    void registerCard(Card *cardPrototype);
    // Create card by ID [Header]
    Card *createCard(int cardID);
    // Create card by Name [Header]
    Card *createCard(const std::string &cardName);
    // Initialise card database [Header]
    void initializeCardDatabase();
    // Get random card by rarity [Header]
    Card *getRandomCard(Card::CardRarity rarity);
    // Get all cards of a specific type [Header]
    std::vector<Card *> getCardsByType(Card::CardType type);
    // Clear registry (cleanup) [Header]
    void clearRegistry();
    // Debug: Print all registered cards [Header]
    void printCardRegistry();
private:
    // Helper: Initialise attack cards
    void initAttackCards();
    // Helper: Initialise defense cards
    void initDefenseCards();
    // Helper: Initialise skill cards
    void initSkillCards();
    // Helper: Initialise power cards
    void initPowerCards();
};
