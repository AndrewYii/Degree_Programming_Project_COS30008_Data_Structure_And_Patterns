#include "PointerHashTable.h"
#include <iostream>

// Constructor
PointerHashTable::PointerHashTable(): count(0)
{
    table = new PointerHashNode *[TABLE_SIZE];

    // Initialize all buckets to nullptr
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        table[i] = nullptr;
    }

    std::cout << "[POINTER HASH TABLE] Created with " << TABLE_SIZE << " buckets.\n";
}

// Destructor
PointerHashTable::~PointerHashTable()
{
    clear();
    delete[] table;
}

// Convert pointer address to index
int PointerHashTable::hashFunction(Card *key) const
{
    if (!key)
        return 0;
    uintptr_t address = reinterpret_cast<uintptr_t>(key);
    address = (address >> 3) ^ (address >> 12);

    return static_cast<int>(address % TABLE_SIZE);
}

// Insert - Add card/metadata pair to hash table
void PointerHashTable::insert(Card *card, ActiveCardMetadata *metadata)
{
    if (!card || !metadata)
        return;

    int index = hashFunction(card);

    // Check if already exists and update
    PointerHashNode *current = table[index];
    while (current)
    {
        if (current->key == card)
        {
            // Update existing entry
            current->metadata = metadata;
            std::cout << "[POINTER HASH TABLE] Updated metadata for card at bucket " << index << "\n";
            return;
        }
        current = current->next;
    }

    // Create new node
    PointerHashNode *newNode = new PointerHashNode(card, metadata);

    // Insert at head of linked list (chaining)
    newNode->next = table[index];
    table[index] = newNode;

    count++;

    std::cout << "[POINTER HASH TABLE] Inserted card at bucket " << index << " (total: " << count << ")\n";
}

// Remove - Remove entry for specific card pointer
bool PointerHashTable::remove(Card *card)
{
    if (!card)
        return false;

    int index = hashFunction(card);
    PointerHashNode *current = table[index];
    PointerHashNode *previous = nullptr;

    // Search linked list at this bucket
    while (current)
    {
        if (current->key == card)
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

            std::cout << "[POINTER HASH TABLE] Removed card from bucket " << index << " (total: " << count << ")\n";
            return true;
        }

        previous = current;
        current = current->next;
    }

    return false; // Not found
}

// Get - Retrieve metadata for specific card pointer
ActiveCardMetadata *PointerHashTable::get(Card *card) const
{
    if (!card)
        return nullptr;

    int index = hashFunction(card);
    PointerHashNode *current = table[index];

    // Search linked list at this bucket
    while (current)
    {
        if (current->key == card)
        {
            return current->metadata;
        }
        current = current->next;
    }

    return nullptr; // Not found
}

// Contains - Check if specific card pointer exists
bool PointerHashTable::contains(Card *card) const
{
    return get(card) != nullptr;
}

// Clear - Remove all entries from the hash table
void PointerHashTable::clear()
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        PointerHashNode *current = table[i];

        // Delete entire linked list at this bucket
        while (current)
        {
            PointerHashNode *toDelete = current;
            current = current->next;
            delete toDelete;
        }

        table[i] = nullptr;
    }

    count = 0;
    std::cout << "[POINTER HASH TABLE] Cleared all entries.\n";
}

// Debug: Print table contents
void PointerHashTable::printDebug() const
{
    std::cout << "=== POINTER HASH TABLE DEBUG ===\n";
    std::cout << "Total entries: " << count << "\n";

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (table[i])
        {
            std::cout << "Bucket " << i << ": ";

            PointerHashNode *current = table[i];
            int chainLength = 0;
            while (current)
            {
                chainLength++;
                current = current->next;
            }
            std::cout << chainLength << " entries\n";
        }
    }

    std::cout << "================================\n";
}
