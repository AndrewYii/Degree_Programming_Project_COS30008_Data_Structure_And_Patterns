#pragma once

#include "Card.h"
#include <vector>

// Forward declaration because the parameter will use the PointerHashTable class while PointerHashTable Class will include ActiveBar.h, it will cause circular dependency
class PointerHashTable;

// Different slot types in the Active Bar
enum class SlotType
{
    // Regular slot
    NORMAL,
    // Always active, no CPU cost (green)
    SINGLETON,
    // Activates card 3x (purple)
    OBSERVER
};

// Information associated with an active card in the Active Bar
struct ActiveCardMetadata
{
    // Pointer to the card
    Card *card;
    // Insertion order for tracking
    int insertionOrder;
    // Tree level for tree structure cards
    int treeLevel;
    Card *treeParent;
    // Update the card bonus behaviour
    bool canPlayWithBonus;
    // Bonus damage for this card
    int bonusDamage;
    // Slot type this card is in
    SlotType slotType;
};

class ActiveBar
{
private:
    // The head of the Active Bar DLL
    Card *head;
    // The tail of the Active Bar DLL
    Card *tail;
    // Card count in active bar (DLL)
    int cardCount;
    // Maximum capacity for normal slots
    int maxCapacity;
    // Special slots (from buffs) - separate from main DLL
    std::vector<SlotType> slotTypes;
    // Special slot index for the singleton
    int singletonSlotIndex;
    // Special slot index for the observer
    int observerSlotIndex;
    // Card in singleton slot
    Card *singletonCard;
    // Card in observer slot
    Card *observerCard;
    // Custom hash table for metadata lookup
    PointerHashTable *metadataHashTable;
    // Storage for actual metadata objects
    std::vector<ActiveCardMetadata> metadataStorage;
    // Next insertion order counter
    int nextInsertionOrder;
    Card *lastPlayedCard;
    bool adapterBuffActive;

public:
    // Constructor [Header]
    ActiveBar(int capacity = 10);
    // Destructor [Header]
    ~ActiveBar();
    // Add to tail (normal slots) -append [Header]
    bool addCard(Card *card);
    // Add to specific slot (including special slots) [Header]
    bool addCardToSlot(Card *card, int position);
    // Insert at specific position [Header]
    bool insertCardAt(Card *card, int position);
    bool removeCard(Card *card);
    Card *removeCardAt(int position);
    bool moveCard(int fromPosition, int toPosition);
    void clear();
    // Clear normal + observer, keep singleton [Header]
    void clearForNextTurn();
    // Get Head [Header]
    Card *getHead() const;
    // Get Tail [Header]
    Card *getTail() const;
    // Get Card at position [Header]
    Card *getCardAt(int position) const;
    // Get Card count [Header]
    int getCardCount() const;
    // Get Max Capacity [Header]
    int getMaxCapacity() const;
    // Check if full [Header]
    bool isFull() const;
    // Check if empty [Header]
    bool isEmpty() const;
    // Get cards by data type [Header]
    std::vector<Card *> getCardsByDataType(Card::CardDataType type) const;
    // Calculate pattern bonus for a card [Header]
    int calculatePatternBonus(Card *card) const;
    // Get metadata for a card [Header]
    ActiveCardMetadata *getCardMetadata(Card *card);
    // Check if card can play with bonus [Header]
    bool canPlayWithBonus(Card *card) const;
    // Update bonus validation for all cards [Header]
    void updateBonusValidation();
    // Set last played card [Header]
    void setLastPlayedCard(Card *card);
    // Get last played card [Header]
    Card *getLastPlayedCard() const;
    // Energy cost calculation [Header]
    int getTotalEnergyCost(class Player *player = nullptr) const;
    // Add singleton slot [Header]
    void addSingletonSlot();
    // Add observer slot [Header]
    void addObserverSlot();
    // Get slot type at position [Header]
    SlotType getSlotTypeAt(int position) const;
    // Get singleton slot index [Header]
    int getSingletonSlotIndex() const;
    // Get observer slot index [Header]
    int getObserverSlotIndex() const;
    // Check if slot is special [Header]
    bool isSpecialSlot(int position) const;
    // Get singleton card [Header]
    Card *getSingletonCard() const;
    // Get observer card [Header]
    Card *getObserverCard() const;
    // Get total slot count including special slots [Header]
    int getTotalSlotCount() const;
    void printOrder() const;
    void setAdapterBuff(bool active);
    bool getAdapterBuff() const;
};
