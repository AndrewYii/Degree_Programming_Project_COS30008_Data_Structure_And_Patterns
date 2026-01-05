#include "CardHashTable.h"
#include <iostream>

// Constructor [Implementation]
CardHashTable::CardHashTable() : count(0)
{
    table = new HashNode *[TABLE_SIZE];
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        table[i] = (HashNode *)0;
    }
    std::cout << "[HASH TABLE] Created with " << TABLE_SIZE << " buckets.\n";
}

// Destructor [Implementation]
CardHashTable::~CardHashTable()
{
    clear();
    delete[] table;
}

// Hash function - convert string to index [Implementation]
int CardHashTable::hashFunction(const std::string &key) const
{
    int hash = 0;
    for (char c : key)
    {
        hash += static_cast<int>(c);
    }
    return hash % TABLE_SIZE;
}

// Insert card into hash table [Implementation]
void CardHashTable::insert(const std::string &cardName, Card *card)
{
    if (!card)
        return;

    int index = hashFunction(cardName);

    // Create new node
    HashNode *newNode = new HashNode(cardName, card);

    // Insert at head of linked list (chaining)
    newNode->next = table[index];
    table[index] = newNode;

    count++;

    std::cout << "[HASH TABLE] Inserted '" << cardName << "' at bucket " << index << "\n";
}

// Remove specific card from hash table [Implementation]
bool CardHashTable::remove(const std::string &cardName, Card *card)
{
    if (!card)
        return false;

    int index = hashFunction(cardName);
    HashNode *current = table[index];
    HashNode *previous = (HashNode *)0;

    // Search linked list at this bucket
    while (current)
    {
        if (current->key == cardName && current->card == card)
        {
            // Found it! Remove from list
            if (previous)
            {
                previous->next = current->next;
            }
            else
            {
                // Removing head
                table[index] = current->next;
            }

            delete current;
            count--;

            std::cout << "[HASH TABLE] Removed '" << cardName << "' from bucket " << index << "\n";
            return true;
        }

        previous = current;
        current = current->next;
    }

    return false;
}

// get card by name (first match) [Implementation]
Card *CardHashTable::get(const std::string &cardName) const
{
    int index = hashFunction(cardName);
    HashNode *current = table[index];

    // Search linked list at this bucket
    while (current)
    {
        if (current->key == cardName)
        {
            return current->card;
        }
        current = current->next;
    }

    return (Card *)0;
}

// Get all cards with this name [Implementation]
Card **CardHashTable::getAll(const std::string &cardName, int &outCount) const
{
    outCount = countByName(cardName);

    if (outCount == 0)
        return (Card **)0;

    // Allocate array for results
    Card **results = new Card *[outCount];

    int index = hashFunction(cardName);
    HashNode *current = table[index];
    int resultIndex = 0;

    // Collect all matching cards
    while (current && resultIndex < outCount)
    {
        if (current->key == cardName)
        {
            results[resultIndex] = current->card;
            resultIndex++;
        }
        current = current->next;
    }

    return results;
}

// Count cards with specific name [Implementation]
int CardHashTable::countByName(const std::string &cardName) const
{
    int index = hashFunction(cardName);
    HashNode *current = table[index];
    int matchCount = 0;

    // Count matching cards in this bucket
    while (current)
    {
        if (current->key == cardName)
        {
            matchCount++;
        }
        current = current->next;
    }

    return matchCount;
}

// Check if table has card with this name  [Implementation]
bool CardHashTable::contains(const std::string &cardName) const
{
    return get(cardName) != (Card *)0;
}

// Clear entire table [Implementation]
void CardHashTable::clear()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        HashNode *current = table[i];

        // Delete entire linked list at this bucket
        while (current)
        {
            HashNode *toDelete = current;
            current = current->next;
            delete toDelete;
        }

        table[i] = (HashNode *)0;
    }

    count = 0;
    std::cout << "[HASH TABLE] Cleared all entries.\n";
}

// Debug: Print table contents [Implementation]
void CardHashTable::printDebug() const
{
    std::cout << "=== HASH TABLE DEBUG ===\n";
    std::cout << "Total cards: " << count << "\n";

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (table[i])
        {
            std::cout << "Bucket " << i << ": ";

            HashNode *current = table[i];
            while (current)
            {
                std::cout << current->key << " -> ";
                current = current->next;
            }
            std::cout << "NULL\n";
        }
    }

    std::cout << "=======================\n";
}
