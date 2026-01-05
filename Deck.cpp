#include "Deck.h"
#include "CardIterator.h"
#include <algorithm>
#include <random>
#include <iostream>

// Constructor [Implementation]
Deck::Deck() : drawPileTop(static_cast<Card *>(0)), drawPileCount(0), discardPileTop(static_cast<Card *>(0)), discardPileCount(0), handHead(static_cast<Card *>(0)), handTail(static_cast<Card *>(0)), handCount(0), maxHandSize(10), exhaustPileTop(static_cast<Card *>(0)), exhaustPileCount(0)
{

    cardHashTable = new CardHashTable();
}

// Destructor [Implementation]
Deck::~Deck()
{
    clearAllPiles();
    delete cardHashTable;
}

// Add card to draw pile (stack push) [Implementation]
void Deck::pushToDrawPile(Card *card)
{
    if (!card)
        return;

    card->next = drawPileTop;
    card->prev = static_cast<Card *>(0);

    drawPileTop = card;
    drawPileCount++;
}

// Pop card from draw pile (stack pop) [Implementation]
Card *Deck::popFromDrawPile()
{
    if (!drawPileTop)
    {
        return static_cast<Card *>(0);
    }

    Card *card = drawPileTop;
    drawPileTop = drawPileTop->next;
    drawPileCount--;

    card->next = static_cast<Card *>(0);
    card->prev = static_cast<Card *>(0);

    return card;
}

// Peek at top card of draw pile without removing [Implementation]
Card *Deck::peekDrawPile() const
{
    return drawPileTop;
}

// Shuffle draw pile [Implementation]
void Deck::shuffleDrawPile()
{
    if (drawPileCount <= 1)
        return;

    std::vector<Card *> cards = stackToVector(drawPileTop);

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);

    vectorToStack(cards, drawPileTop, drawPileCount);
}

// Push card to discard pile  [Implementation]
void Deck::pushToDiscardPile(Card *card)
{
    if (!card)
        return;

    card->next = discardPileTop;
    card->prev = static_cast<Card *>(0);

    discardPileTop = card;
    discardPileCount++;
}

// Pop card from discard pile [Implementation]
Card *Deck::popFromDiscardPile()
{
    if (!discardPileTop)
    {
        return static_cast<Card *>(0);
    }

    Card *card = discardPileTop;
    discardPileTop = discardPileTop->next;
    discardPileCount--;

    card->next = static_cast<Card *>(0);
    card->prev = static_cast<Card *>(0);

    return card;
}

// Recycle discard pile into draw pile and shuffle [Implementation]
void Deck::recycleDiscardPile()
{
    std::cout << "[DECK] Recycling discard pile into draw pile...\n";

    // Pop all cards from discard, push to draw
    while (discardPileTop)
    {
        Card *card = popFromDiscardPile();
        pushToDrawPile(card);
    }

    // Shuffle the draw pile
    shuffleDrawPile();

    std::cout << "[DECK] Recycled! Draw pile now has " << drawPileCount << " cards.\n";
}

// Add to hand  [Implementation]
bool Deck::addToHand(Card *card)
{
    if (!card)
        return false;

    // Check hand size limit
    if (handCount >= maxHandSize)
    {
        std::cout << "[DECK] Hand full! Discarding card.\n";
        pushToDiscardPile(card);
        return false;
    }

    // SLL insertion at tail
    card->next = static_cast<Card *>(0);
    card->prev = static_cast<Card *>(0);

    if (!handHead)
    {
        // Empty list
        handHead = card;
        handTail = card;
    }
    else
    {
        // Add to tail
        handTail->next = card;
        handTail = card;
    }

    handCount++;

    cardHashTable->insert(card->getName(), card);

    return true;
}

// Remove from hand [Implementation]
Card *Deck::removeFromHand(Card *card)
{
    if (!card || !handHead)
        return static_cast<Card *>(0);

    Card *current = handHead;
    Card *previous = static_cast<Card *>(0);

    while (current)
    {
        if (current == card)
        {
            if (previous)
            {
                previous->next = current->next;
            }
            else
            {
                handHead = current->next;
            }

            if (current == handTail)
            {
                handTail = previous;
            }

            current->next = static_cast<Card *>(0);
            current->prev = static_cast<Card *>(0);

            handCount--;

            cardHashTable->remove(card->getName(), card);

            return current;
        }

        previous = current;
        current = current->next;
    }

    return static_cast<Card *>(0);
}

// Remove from hand by index [Implementation]
Card *Deck::removeFromHandAt(int index)
{
    if (index < 0 || index >= handCount || !handHead)
    {
        return static_cast<Card *>(0);
    }

    Card *current = handHead;
    Card *previous = static_cast<Card *>(0);
    int currentIndex = 0;

    // Traverse to index
    while (current && currentIndex < index)
    {
        previous = current;
        current = current->next;
        currentIndex++;
    }

    if (!current)
        return static_cast<Card *>(0);

    // Remove card
    if (previous)
    {
        previous->next = current->next;
    }
    else
    {
        handHead = current->next;
    }

    if (current == handTail)
    {
        handTail = previous;
    }

    current->next = static_cast<Card *>(0);
    current->prev = static_cast<Card *>(0);

    handCount--;
    return current;
}

// GET card at index [Implementation]
Card *Deck::getCardAt(int index) const
{
    if (index < 0 || index >= handCount || !handHead)
    {
        return static_cast<Card *>(0);
    }

    Card *current = handHead;
    int currentIndex = 0;

    while (current && currentIndex < index)
    {
        current = current->next;
        currentIndex++;
    }

    return current;
}

// Find index of card in hand [Implementation]
int Deck::findCardIndex(Card *card) const
{
    if (!card || !handHead)
        return -1;

    Card *current = handHead;
    int index = 0;

    while (current)
    {
        if (current == card)
        {
            return index;
        }
        current = current->next;
        index++;
    }

    return -1; // Not found
}

// Exhaust a card (remove from combat) [Implementation]
void Deck::exhaustCard(Card *card)
{
    if (!card)
        return;

    card->next = exhaustPileTop;
    card->prev = static_cast<Card *>(0);

    exhaustPileTop = card;
    exhaustPileCount++;

    std::cout << "[DECK] Card exhausted: " << card->getName() << "\n";
}

// DRAW N cards from draw pile to hand [Implementation]
void Deck::drawCards(int count)
{
    for (int i = 0; i < count; i++)
    {
        // If draw pile empty, recycle discard
        if (!drawPileTop)
        {
            if (discardPileCount > 0)
            {
                recycleDiscardPile();
            }
            else
            {
                std::cout << "[DECK] No more cards to draw!\n";
                return;
            }
        }

        // Pop from draw pile
        Card *card = popFromDrawPile();

        if (card)
        {
            // Add to hand
            if (!addToHand(card))
            {
                // Hand full, card already discarded
                break;
            }
        }
    }
}

// Discard entire hand [Implementation]
void Deck::discardHand()
{
    while (handHead)
    {
        Card *card = removeFromHandAt(0);
        if (card)
        {
            pushToDiscardPile(card);
        }
    }

    // Clear custom hash table when hand is empty
    cardHashTable->clear();
}

// Initialise deck for combat from master deck [Implementation]
void Deck::initialiseForCombat(std::vector<Card *> &masterDeck)
{
    // Clear all piles first
    clearAllPiles();

    // Clone all cards from master deck and add to draw pile
    for (Card *card : masterDeck)
    {
        if (card)
        {
            Card *clonedCard = card->clone();
            pushToDrawPile(clonedCard);
        }
    }

    // Shuffle draw pile
    shuffleDrawPile();

    std::cout << "[DECK] Initialized for combat with " << drawPileCount << " cards.\n";
}

// Clear all piles [Implementation]
void Deck::clearAllPiles()
{
    // Clear draw pile
    while (drawPileTop)
    {
        Card *card = popFromDrawPile();
        delete card;
    }

    // Clear discard pile
    while (discardPileTop)
    {
        Card *card = popFromDiscardPile();
        delete card;
    }

    // Clear hand
    while (handHead)
    {
        Card *card = removeFromHandAt(0);
        delete card;
    }

    // Clear exhaust pile
    while (exhaustPileTop)
    {
        Card *card = exhaustPileTop;
        exhaustPileTop = exhaustPileTop->next;
        exhaustPileCount--;
        delete card;
    }

    // Clear custom hash table
    cardHashTable->clear();
}

// Print deck status [Implementation]
void Deck::printDeckStatus() const
{
    std::cout << "=== DECK STATUS ===\n";
    std::cout << "Draw Pile: " << drawPileCount << " cards\n";
    std::cout << "Discard Pile: " << discardPileCount << " cards\n";
    std::cout << "Hand: " << handCount << "/" << maxHandSize << " cards\n";
    std::cout << "Exhaust: " << exhaustPileCount << " cards\n";
    std::cout << "===================\n";
}

// Convert stack to vector
std::vector<Card *> Deck::stackToVector(Card *top)
{
    std::vector<Card *> cards;
    Card *current = top;

    while (current)
    {
        cards.push_back(current);
        current = current->next;
    }

    return cards;
}

// Convert vector to stack
void Deck::vectorToStack(const std::vector<Card *> &cards, Card *&top, int &count)
{
    top = static_cast<Card *>(0);
    count = 0;

    // Build stack from vector (reverse order)
    for (int i = static_cast<int>(cards.size()) - 1; i >= 0; i--)
    {
        Card *card = cards[i];
        card->next = top;
        card->prev = static_cast<Card *>(0);
        top = card;
        count++;
    }
}

// Create hand iterator [Implementation]
CardIterator *Deck::createHandIterator() const
{
    return new CardIterator(handHead);
}

// Get card by name (first match)
Card *Deck::getCardByName(const std::string &cardName) const
{
    return cardHashTable->get(cardName);
}

// Get all cards by name
Card **Deck::getAllCardsByName(const std::string &cardName, int &count) const
{
    return cardHashTable->getAll(cardName, count);
}

// Count how many cards of a specific name in hand
int Deck::countCardsByName(const std::string &cardName) const
{
    return cardHashTable->countByName(cardName);
}

// Check if hand has any card with this name
bool Deck::hasCardNamed(const std::string &cardName) const
{
    return cardHashTable->contains(cardName);
}

// Debug: Print hash table contents [Implementation]
void Deck::printHashTableDebug() const
{
    cardHashTable->printDebug();
}

// Get the counts of draw piles [Implementation]
int Deck::getDrawPileCount() const
{
    return drawPileCount;
}
// Get the counts of discard piles [Implementation]
int Deck::getDiscardPileCount() const
{
    return discardPileCount;
}
// Get the counts of hand piles [Implementation]
int Deck::getHandCount() const
{
    return handCount;
}
// Get the counts of exhaust piles [Implementation]
int Deck::getExhaustPileCount() const
{
    return exhaustPileCount;
}
// Get max hand size [Implementation]
int Deck::getMaxHandSize() const
{
    return maxHandSize;
}
// Get hand head [Implementation]
Card *Deck::getHandHead() const
{
    return handHead;
}

// Set max hand size [Implementation]
void Deck::setMaxHandSize(int size)
{
    maxHandSize = size;
}
