#pragma once

#include "Card.h"
#include "CardHashTable.h"
#include "CardIterator.h"
#include <vector>

class Deck
{
private:
    // Draw pile
    Card *drawPileTop;
    // Number of cards in draw pile
    int drawPileCount;
    // Discard pile
    Card *discardPileTop;
    // Number of cards in discard pile
    int discardPileCount;
    // Hand (doubly linked list)
    Card *handHead;
    // Tail of hand DLL
    Card *handTail;
    // Number of cards in hand
    int handCount;
    // Maximum hand size
    int maxHandSize;
    // Exhaust pile
    Card *exhaustPileTop;
    // Number of cards in exhaust pile
    int exhaustPileCount;
    // Custom hash table for hand cards by name
    CardHashTable *cardHashTable;

public:
    // Constructor [Header]
    Deck();
    // Destructor [Header]
    ~Deck();
    // Push card to top of draw pile [Header]
    void pushToDrawPile(Card *card);
    // Pop card from top of draw pile [Header]
    Card *popFromDrawPile();
    // Peek at top card without removing [Header]
    Card *peekDrawPile() const;
    // Shuffle draw pile [Header]
    void shuffleDrawPile();
    // Push card to discard pile [Header]
    void pushToDiscardPile(Card *card);
    // Pop from discard pile [Header]   
    Card *popFromDiscardPile();
    // Move all discard pile cards to draw pile and shuffle [Header]
    void recycleDiscardPile();
    // Add card to hand [Header]
    bool addToHand(Card *card);
    // Remove card from hand (returns the card) [Header]
    Card *removeFromHand(Card *card);
    // Remove card from hand by position [Header]
    Card *removeFromHandAt(int index);
    // Get card at index [Header]   
    Card *getCardAt(int index) const;
    // Find card in hand and return its index [Header]
    int findCardIndex(Card *card) const;
    // Exhaust a card (remove from combat) [Header]
    void exhaustCard(Card *card);
    // Draw N cards from draw pile to hand [Header]
    void drawCards(int count);
    // Discard entire hand [Header]
    void discardHand();
    // Start of combat: shuffle all cards into draw pile [Header]
    void initialiseForCombat(std::vector<Card *> &masterDeck);
    // End of combat: clear all piles [Header]
    void clearAllPiles();
    // Get the counts of draw piles [Header]
    int getDrawPileCount() const ;
    // Get the counts of discard piles [Header]
    int getDiscardPileCount() const ;
    // Get the counts of hand piles [Header]
    int getHandCount() const ;
    // Get the counts of exhaust piles [Header]
    int getExhaustPileCount() const ;
    // Get max hand size [Header]
    int getMaxHandSize() const ;
    // Get hand head [Header]
    Card *getHandHead() const ;
    // Get card by name  [Header]
    Card *getCardByName(const std::string &cardName) const;
    // Get all cards by name [Header]
    Card **getAllCardsByName(const std::string &cardName, int &count) const;
    // Count how many cards of a specific name in hand [Header]
    int countCardsByName(const std::string &cardName) const;
    // Check if hand has any card with this name [Header]
    bool hasCardNamed(const std::string &cardName) const;
    // Debug: Print hash table contents [Header]
    void printHashTableDebug() const;
    // Create hand iterator [Header]
    class CardIterator *createHandIterator() const;
    // Set max hand size [Header]
    void setMaxHandSize(int size);
    // Print deck status [Header]
    void printDeckStatus() const;
private:
    // Helper functions to convert between stack and vector
    std::vector<Card *> stackToVector(Card *top);
    // Helper function to convert vector to stack
    void vectorToStack(const std::vector<Card *> &cards, Card *&top, int &count);
};
