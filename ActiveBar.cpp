#include "ActiveBar.h"
#include "PointerHashTable.h"
#include "Player.h"
#include <iostream>

// Constructor [Implementation]
ActiveBar::ActiveBar(int capacity) : head((Card *)0), tail((Card *)0), cardCount(0), maxCapacity(capacity), singletonSlotIndex(-1), observerSlotIndex(-1), singletonCard((Card *)0), observerCard((Card *)0), nextInsertionOrder(0), lastPlayedCard((Card *)0), adapterBuffActive(false)
{
    slotTypes.resize(capacity, SlotType::NORMAL);
    metadataHashTable = new PointerHashTable();
    metadataStorage.reserve(capacity + 10);
}

// Destructor [Implementation]
ActiveBar::~ActiveBar()
{
    clear();
    delete metadataHashTable;
}

// Add card to tail (normal slots) - append [Implementation]
bool ActiveBar::addCard(Card *card)
{
    if (!card || isFull())
    {
        return false;
    }
    // DLL insertion at tail
    if (isEmpty())
    {
        head = tail = card;
        card->prev = card->next = (Card *)0;
    }
    else
    {
        tail->next = card;
        card->prev = tail;
        card->next = (Card *)0;
        tail = card;
    }
    cardCount++;
    // Initialise metadata and store in vector
    ActiveCardMetadata metadata;
    metadata.card = card;
    metadata.insertionOrder = nextInsertionOrder++;
    metadata.treeLevel = 0;
    metadata.treeParent = (Card *)0;
    metadata.canPlayWithBonus = false;
    metadata.bonusDamage = 0;
    metadata.slotType = SlotType::NORMAL;
    metadataStorage.push_back(metadata);
    metadataHashTable->insert(card, &metadataStorage.back());
    std::cout << "[ActiveBar] Added " << card->getName() << " (order: " << metadata.insertionOrder << ")" << std::endl;

    // Update bonus validation after structure changes
    updateBonusValidation();

    return true;
}

// Add card to specific slot (including special slots) [Implementation]
bool ActiveBar::addCardToSlot(Card *card, int position)
{
    if (!card)
    {
        return false;
    }
    // Check if this is a special slot
    if (position == singletonSlotIndex && singletonSlotIndex >= 0)
    {
        // Add to singleton slot
        if (singletonCard != (Card *)0)
        {
            std::cout << "[ActiveBar] Singleton slot already occupied!" << std::endl;
            return false;
        }
        singletonCard = card;
        card->prev = card->next = (Card *)0;
        // Initialise metadata and store in vector
        ActiveCardMetadata metadata;
        metadata.card = card;
        metadata.insertionOrder = nextInsertionOrder++;
        metadata.treeLevel = 0;
        metadata.treeParent = (Card *)0;
        metadata.canPlayWithBonus = false;
        metadata.bonusDamage = 0;
        metadata.slotType = SlotType::SINGLETON;
        metadataStorage.push_back(metadata);
        metadataHashTable->insert(card, &metadataStorage.back());
        std::cout << "[ActiveBar] Added " << card->getName() << " to SINGLETON slot (FREE energy!)" << std::endl;
        updateBonusValidation();
        return true;
    }
    if (position == observerSlotIndex && observerSlotIndex >= 0)
    {
        // Add to observer slot
        if (observerCard != (Card *)0)
        {
            std::cout << "[ActiveBar] Observer slot already occupied!" << std::endl;
            return false;
        }
        observerCard = card;
        card->prev = card->next = (Card *)0;
        // Initialise metadata and store in vector
        ActiveCardMetadata metadata;
        metadata.card = card;
        metadata.insertionOrder = nextInsertionOrder++;
        metadata.treeLevel = 0;
        metadata.treeParent = (Card *)0;
        metadata.canPlayWithBonus = false;
        metadata.bonusDamage = 0;
        metadata.slotType = SlotType::OBSERVER;
        metadataStorage.push_back(metadata);
        metadataHashTable->insert(card, &metadataStorage.back());
        std::cout << "[ActiveBar] Added " << card->getName() << " to OBSERVER slot (3x activation!)" << std::endl;
        updateBonusValidation();
        return true;
    }
    // Normal slot - check if within normal slot range
    if (position >= maxCapacity)
    {
        std::cout << "[ActiveBar] Position " << position << " is out of normal slot range!" << std::endl;
        return false;
    }
    // Check if we're at capacity for normal slots
    if (isFull())
    {
        std::cout << "[ActiveBar] Normal slots are full!" << std::endl;
        return false;
    }
    // Add to normal DLL at specific position
    return insertCardAt(card, position);
}
// Insert at specific position [Implementation]
bool ActiveBar::insertCardAt(Card *card, int position)
{
    if (!card || isFull() || position < 0 || position > cardCount)
    {
        return false;
    }

    if (position == 0)
    {
        if (isEmpty())
        {
            head = tail = card;
            card->prev = card->next = (Card *)0;
        }
        else
        {
            card->next = head;
            card->prev = (Card *)0;
            head->prev = card;
            head = card;
        }
        cardCount++;

        ActiveCardMetadata metadata;
        metadata.card = card;
        metadata.insertionOrder = nextInsertionOrder++;
        metadata.treeLevel = 0;
        metadata.treeParent = (Card *)0;
        metadata.canPlayWithBonus = false;
        metadata.bonusDamage = 0;
        metadata.slotType = SlotType::NORMAL;
        metadataStorage.push_back(metadata);
        metadataHashTable->insert(card, &metadataStorage.back());

        updateBonusValidation();
        return true;
    }

    if (position == cardCount)
    {
        return addCard(card);
    }

    Card *current = head;
    for (int i = 0; i < position; i++)
    {
        current = current->next;
    }
    card->prev = current->prev;
    card->next = current;
    current->prev->next = card;
    current->prev = card;
    cardCount++;

    ActiveCardMetadata metadata;
    metadata.card = card;
    metadata.insertionOrder = nextInsertionOrder++;
    metadata.treeLevel = 0;
    metadata.treeParent = (Card *)0;
    metadata.canPlayWithBonus = false;
    metadata.bonusDamage = 0;
    metadata.slotType = SlotType::NORMAL;
    metadataStorage.push_back(metadata);
    metadataHashTable->insert(card, &metadataStorage.back());

    updateBonusValidation();

    return true;
}
// Remove specific card [Implementation]
bool ActiveBar::removeCard(Card *card)
{
    if (!card)
    {
        return false;
    }
    // Check if it's in singleton slot
    if (card == singletonCard)
    {
        singletonCard = (Card *)0;
        card->prev = card->next = (Card *)0;
        metadataHashTable->remove(card);
        std::cout << "[ActiveBar] Removed from SINGLETON slot" << std::endl;
        updateBonusValidation();
        return true;
    }
    // Check if it's in observer slot
    if (card == observerCard)
    {
        observerCard = (Card *)0;
        card->prev = card->next = (Card *)0;
        metadataHashTable->remove(card);
        std::cout << "[ActiveBar] Removed from OBSERVER slot" << std::endl;
        updateBonusValidation();
        return true;
    }
    // Remove from normal DLL
    if (isEmpty())
    {
        return false;
    }
    // Find and remove
    Card *current = head;
    while (current)
    {
        if (current == card)
        {
            // Remove from DLL
            if (current == head && current == tail)
            {
                head = tail = (Card *)0;
            }
            else if (current == head)
            {
                head = head->next;
                head->prev = (Card *)0;
            }
            else if (current == tail)
            {
                tail = tail->prev;
                tail->next = (Card *)0;
            }
            else
            {
                current->prev->next = current->next;
                current->next->prev = current->prev;
            }
            current->prev = current->next = (Card *)0;
            cardCount--;
            metadataHashTable->remove(card);

            // Update bonus validation after structure changes
            updateBonusValidation();

            return true;
        }
        current = current->next;
    }
    return false;
}

// Remove at position [Implementation]
Card *ActiveBar::removeCardAt(int position)
{
    if (position < 0 || position >= cardCount || isEmpty())
    {
        return (Card *)0;
    }
    Card *card = getCardAt(position);
    if (card && removeCard(card))
    {
        return card;
    }

    return (Card *)0;
}

bool ActiveBar::moveCard(int fromPosition, int toPosition)
{
    if (fromPosition < 0 || fromPosition >= cardCount || toPosition < 0 || toPosition >= cardCount)
    {
        return false;
    }

    if (fromPosition == toPosition)
    {
        return true;
    }

    Card *cardToMove = getCardAt(fromPosition);
    if (!cardToMove)
    {
        return false;
    }

    if (cardToMove == head && cardToMove == tail)
    {
        return true;
    }

    if (cardToMove == head)
    {
        head = head->next;
        head->prev = (Card *)0;
    }
    else if (cardToMove == tail)
    {
        tail = tail->prev;
        tail->next = (Card *)0;
    }
    else
    {
        cardToMove->prev->next = cardToMove->next;
        cardToMove->next->prev = cardToMove->prev;
    }

    if (toPosition == 0)
    {
        cardToMove->next = head;
        cardToMove->prev = (Card *)0;
        head->prev = cardToMove;
        head = cardToMove;
    }
    else
    {
        Card *insertAfter = head;
        for (int i = 0; i < toPosition - 1; i++)
        {
            insertAfter = insertAfter->next;
        }

        cardToMove->next = insertAfter->next;
        cardToMove->prev = insertAfter;

        if (insertAfter->next)
        {
            insertAfter->next->prev = cardToMove;
        }
        else
        {
            tail = cardToMove;
        }

        insertAfter->next = cardToMove;
    }

    updateBonusValidation();
    return true;
}

void ActiveBar::clear()
{
    while (head)
    {
        Card *temp = head;
        head = head->next;
        temp->prev = temp->next = (Card *)0;
    }
    head = tail = (Card *)0;
    cardCount = 0;
    // Clear special slots
    if (singletonCard)
    {
        singletonCard->prev = singletonCard->next = (Card *)0;
        singletonCard = (Card *)0;
    }
    if (observerCard)
    {
        observerCard->prev = observerCard->next = (Card *)0;
        observerCard = (Card *)0;
    }
    // Clear hash table and storage
    metadataHashTable->clear();
    metadataStorage.clear();
    nextInsertionOrder = 0;
    lastPlayedCard = (Card *)0;
}

// Clear normal + observer, keep singleton [Implementation]
void ActiveBar::clearForNextTurn()
{
    // Clear normal DLL
    while (head)
    {
        Card *temp = head;
        head = head->next;
        temp->prev = temp->next = (Card *)0;
        metadataHashTable->remove(temp);
    }
    head = tail = (Card *)0;
    cardCount = 0;
    // Clear observer slot (consumed after use)
    if (observerCard)
    {
        observerCard->prev = observerCard->next = (Card *)0;
        metadataHashTable->remove(observerCard);
        observerCard = (Card *)0;
    }
    // Keep singleton card
    if (singletonCard)
    {
        std::cout << "[ActiveBar] Singleton card KEPT for next turn: " << singletonCard->getName() << std::endl;
    }
    lastPlayedCard = (Card *)0;
}

// Get Head [Implementation]
Card *ActiveBar::getHead() const
{
    return head;
}
// Get Tail [Implementation]
Card *ActiveBar::getTail() const
{
    return tail;
}
// Get card count [Implementation]
int ActiveBar::getCardCount() const
{
    return cardCount;
}
// Get Max Capacity [Implementation]
int ActiveBar::getMaxCapacity() const
{
    return maxCapacity;
}
// Check if full [Implementation]
bool ActiveBar::isFull() const
{
    return cardCount >= maxCapacity;
}
// Check if empty [Implementation]
bool ActiveBar::isEmpty() const
{
    return cardCount == 0;
}

// Get Card at position [Implementation]
Card *ActiveBar::getCardAt(int position) const
{
    if (position < 0 || position >= cardCount)
    {
        return (Card *)0;
    }
    Card *current = head;
    for (int i = 0; i < position; i++)
    {
        current = current->next;
    }
    return current;
}

// Get cards by data type [Implementation]
std::vector<Card *> ActiveBar::getCardsByDataType(Card::CardDataType type) const
{
    std::vector<Card *> result;
    Card *current = head;
    while (current)
    {
        if (current->getDataType() == type)
        {
            result.push_back(current);
        }
        current = current->next;
    }
    return result;
}

// Calculate pattern bonus for a card [Implementation]
int ActiveBar::calculatePatternBonus(Card *card) const
{
    if (!card)
        return 0;

    ActiveCardMetadata *metadata = metadataHashTable->get(card);
    if (metadata && metadata->canPlayWithBonus)
    {
        return metadata->bonusDamage;
    }

    return 0;
}

// Debug [Implementation]
void ActiveBar::printOrder() const
{
    std::cout << "[ActiveBar] Card order (HEAD->TAIL): ";
    Card *current = head;
    while (current)
    {
        std::cout << current->getName() << " -> ";
        current = current->next;
    }
    std::cout << "NULL" << std::endl;
}

// Get total energy cost of all cards in active bar [Implementation]
int ActiveBar::getTotalEnergyCost(Player *player) const
{
    int totalCost = 0;
    Card *current = head;

    while (current)
    {
        if (player)
        {
            totalCost += player->calculateEnergyCost(current->getEnergyCost());
        }
        else
        {
            totalCost += current->getEnergyCost();
        }
        current = current->next;
    }

    if (observerCard)
    {
        if (player)
        {
            totalCost += player->calculateEnergyCost(observerCard->getEnergyCost());
        }
        else
        {
            totalCost += observerCard->getEnergyCost();
        }
    }

    return totalCost;
}

// Get metadata for a specific card [Implementation]
ActiveCardMetadata *ActiveBar::getCardMetadata(Card *card)
{
    return metadataHashTable->get(card);
}

// Check if card can play with bonus [Implementation]
bool ActiveBar::canPlayWithBonus(Card *card) const
{
    ActiveCardMetadata *metadata = metadataHashTable->get(card);
    if (metadata)
    {
        return metadata->canPlayWithBonus;
    }
    return false;
}

// Update bonus validation for all cards in active bar [Implementation]
void ActiveBar::updateBonusValidation()
{
    if (isEmpty())
        return;

    for (size_t i = 0; i < metadataStorage.size(); i++)
    {
        metadataStorage[i].canPlayWithBonus = false;
        metadataStorage[i].bonusDamage = 0;
    }

    if (adapterBuffActive)
    {
        Card *current = head;
        while (current)
        {
            ActiveCardMetadata *metadata = metadataHashTable->get(current);
            if (metadata)
            {
                Card::CardDataType dataType = current->getDataType();
                if (dataType == Card::CardDataType::STACK)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 3;
                }
                else if (dataType == Card::CardDataType::QUEUE)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 2;
                }
                else if (dataType == Card::CardDataType::SLL)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 2;
                }
                else if (dataType == Card::CardDataType::DLL)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 3;
                }
                else if (dataType == Card::CardDataType::NODE)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 1;
                }
            }
            current = current->next;
        }

        Card *firstTree = head;
        while (firstTree)
        {
            if (firstTree->getDataType() == Card::CardDataType::TREE)
            {
                ActiveCardMetadata *metadata = metadataHashTable->get(firstTree);
                if (metadata)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 4;
                }
                break;
            }
            firstTree = firstTree->next;
        }

        // Hash table bonus with adapter - NO NODE REQUIRED!
        Card *current2 = head;
        while (current2)
        {
            if (current2->getDataType() == Card::CardDataType::HASH_TABLE)
            {
                ActiveCardMetadata *metadata = metadataHashTable->get(current2);
                if (metadata)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 5;
                }
            }
            current2 = current2->next;
        }

        if (observerCard && observerCard->getDataType() == Card::CardDataType::HASH_TABLE)
        {
            ActiveCardMetadata *metadata = metadataHashTable->get(observerCard);
            if (metadata)
            {
                metadata->canPlayWithBonus = true;
                metadata->bonusDamage = 5;
            }
        }

        return;
    }

    Card *current = head;
    while (current)
    {
        if (current->getDataType() == Card::CardDataType::STACK)
        {
            ActiveCardMetadata *metadata = metadataHashTable->get(current);
            if (metadata)
            {
                metadata->canPlayWithBonus = true;
                metadata->bonusDamage = 3;
            }
        }
        else
        {
            break; // Stop at first non-STACK card
        }
        current = current->next;
    }

    current = tail;
    while (current)
    {
        if (current->getDataType() == Card::CardDataType::QUEUE)
        {
            ActiveCardMetadata *metadata = metadataHashTable->get(current);
            if (metadata)
            {
                metadata->canPlayWithBonus = true;
                metadata->bonusDamage = 2;
            }
        }
        else
        {
            break; // Stop at first non-QUEUE card
        }
        current = current->prev;
    }

    current = head;
    while (current)
    {
        if (current->getDataType() == Card::CardDataType::SLL)
        {
            // Check if this SLL card has a card after it (has a 'next' link)
            if (current->next != nullptr)
            {
                ActiveCardMetadata *metadata = metadataHashTable->get(current);
                if (metadata)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 2; // Bonus for having forward link
                }
            }
        }
        current = current->next;
    }

    current = head;
    while (current)
    {
        if (current->getDataType() == Card::CardDataType::DLL)
        {
            // Check if this DLL card has cards on both sides (true bidirectional link)
            if (current->prev != nullptr && current->next != nullptr)
            {
                ActiveCardMetadata *metadata = metadataHashTable->get(current);
                if (metadata)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 3; // Higher bonus for true bidirectional position
                }
            }
        }
        current = current->next;
    }

    current = head;
    Card *firstTree = nullptr;
    int cardsAfterFirstTree = 0;
    bool foundFirstTree = false;

    // Find first TREE card and count cards after it
    Card *scanner = head;
    while (scanner)
    {
        if (!foundFirstTree && scanner->getDataType() == Card::CardDataType::TREE)
        {
            firstTree = scanner;
            foundFirstTree = true;
        }
        else if (foundFirstTree)
        {
            cardsAfterFirstTree++;
        }
        scanner = scanner->next;
    }

    // Parent TREE gets bonus only if it has at least 2 children (true branching)
    if (firstTree && cardsAfterFirstTree >= 2)
    {
        ActiveCardMetadata *metadata = metadataHashTable->get(firstTree);
        if (metadata)
        {
            metadata->canPlayWithBonus = true;
            metadata->bonusDamage = 4; // Higher bonus for true tree structure
            metadata->treeLevel = 0;
        }
    }

    current = head;
    while (current)
    {
        if (current->getDataType() == Card::CardDataType::NODE)
        {
            ActiveCardMetadata *metadata = metadataHashTable->get(current);
            if (metadata)
            {
                metadata->canPlayWithBonus = true;
                metadata->bonusDamage = 1; // Small universal bonus
            }
        }
        current = current->next;
    }

    // Count if we have: 1 NODE, 1 HASH_TABLE, and at least 1 other card
    bool hasNode = false;
    bool hasHash = false;
    int totalCards = 0;

    current = head;
    while (current)
    {
        totalCards++;
        if (current->getDataType() == Card::CardDataType::NODE)
            hasNode = true;
        if (current->getDataType() == Card::CardDataType::HASH_TABLE)
            hasHash = true;
        current = current->next;
    }

    if (observerCard)
    {
        totalCards++;
        if (observerCard->getDataType() == Card::CardDataType::NODE)
            hasNode = true;
        if (observerCard->getDataType() == Card::CardDataType::HASH_TABLE)
            hasHash = true;
    }

    if (hasNode && hasHash && totalCards >= 3)
    {
        current = head;
        while (current)
        {
            if (current->getDataType() == Card::CardDataType::HASH_TABLE)
            {
                ActiveCardMetadata *metadata = metadataHashTable->get(current);
                if (metadata)
                {
                    metadata->canPlayWithBonus = true;
                    metadata->bonusDamage = 5;
                }
            }
            current = current->next;
        }

        if (observerCard && observerCard->getDataType() == Card::CardDataType::HASH_TABLE)
        {
            ActiveCardMetadata *metadata = metadataHashTable->get(observerCard);
            if (metadata)
            {
                metadata->canPlayWithBonus = true;
                metadata->bonusDamage = 5;
            }
        }
    }
}

// Set Last Played Card [Implementation]
void ActiveBar::setLastPlayedCard(Card *card)
{
    lastPlayedCard = card;
}

// Get Last Played Card [Implementation]
Card *ActiveBar::getLastPlayedCard() const
{
    return lastPlayedCard;
}

// Add singleton slot [Implementation]
void ActiveBar::addSingletonSlot()
{
    if (singletonSlotIndex >= 0)
        return;

    slotTypes.push_back(SlotType::SINGLETON);
    singletonSlotIndex = static_cast<int>(slotTypes.size()) - 1;
    std::cout << "[ActiveBar] Singleton slot added at position " << singletonSlotIndex << std::endl;
}

// Add observer slot [Implementation]
void ActiveBar::addObserverSlot()
{
    if (observerSlotIndex >= 0)
        return;

    slotTypes.push_back(SlotType::OBSERVER);
    observerSlotIndex = static_cast<int>(slotTypes.size()) - 1;
    std::cout << "[ActiveBar] Observer slot added at position " << observerSlotIndex << std::endl;
}

// Get slot type at position [Implementation]
SlotType ActiveBar::getSlotTypeAt(int position) const
{
    if (position < 0 || position >= static_cast<int>(slotTypes.size()))
        return SlotType::NORMAL;
    return slotTypes[position];
}

// Get singleton slot index [Implementation]
int ActiveBar::getSingletonSlotIndex() const
{
    return singletonSlotIndex;
}
// Get observer slot index [Implementation]
int ActiveBar::getObserverSlotIndex() const
{
    return observerSlotIndex;
}
// Check if position is special slot [Implementation]
bool ActiveBar::isSpecialSlot(int position) const
{
    return position == singletonSlotIndex || position == observerSlotIndex;
}
// Get singleton and observer cards [Implementation]
Card *ActiveBar::getSingletonCard() const
{
    return singletonCard;
}
// Get observer card [Implementation]
Card *ActiveBar::getObserverCard() const
{
    return observerCard;
}
// Get total slot count [Implementation]
int ActiveBar::getTotalSlotCount() const
{
    return static_cast<int>(slotTypes.size());
}

void ActiveBar::setAdapterBuff(bool active)
{
    adapterBuffActive = active;
}

bool ActiveBar::getAdapterBuff() const
{
    return adapterBuffActive;
}
