#pragma once

#include "Card.h"
#include <string>

class HashNode
{
public:
    // Card name as key
    std::string key;
    // Pointer to the card
    Card *card;
    // Next node in chain (for collision handling)
    HashNode *next;
    // Constructor
    HashNode(const std::string &k, Card *c): key(k), card(c), next((HashNode*)0) {}
};

class CardHashTable
{
private:
    // Hash table size
    static const int TABLE_SIZE = 20;
    // Array of pointers to HashNode 
    HashNode **table;
    // Total entries in table              
    int count;                 
    // Hash function - convert string to index
    int hashFunction(const std::string &key) const;
public:
    // Constructor [Header]
    CardHashTable();
    // Destructor [Header]
    ~CardHashTable();
    // Insert card into hash table [Header]
    void insert(const std::string &cardName, Card *card);
    // Remove specific card from hash table [Header]
    bool remove(const std::string &cardName, Card *card);
    // Get card by name (first match) [Header]
    Card *get(const std::string &cardName) const;
    // Get all cards with this name [Header]
    Card **getAll(const std::string &cardName, int &outCount) const;
    // Count cards with specific name [Header]
    int countByName(const std::string &cardName) const;
    // Check if table has card with this name [Header]
    bool contains(const std::string &cardName) const;
    // Clear entire table [Header]
    void clear();
    // Get total card count [Header]
    int getCount() const { return count; }
    // Debug: Print table contents
    void printDebug() const;
};
