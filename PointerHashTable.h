#pragma once

#include "Card.h"
#include "ActiveBar.h"

// Forward declaration
struct ActiveCardMetadata;

class PointerHashNode
{
public:
    Card *key;                   
    ActiveCardMetadata *metadata; 
    PointerHashNode *next;        
    PointerHashNode(Card *k, ActiveCardMetadata *m): key(k), metadata(m), next(nullptr) {}
};

class PointerHashTable
{
private:
    static const int TABLE_SIZE = 20; 
    PointerHashNode **table;         
    int count;                        

    int hashFunction(Card *key) const;

public:
    PointerHashTable();

    ~PointerHashTable();

    void insert(Card *card, ActiveCardMetadata *metadata);

    bool remove(Card *card);

    ActiveCardMetadata *get(Card *card) const;

    bool contains(Card *card) const;

    void clear();

    int getCount() const { return count; }
    void printDebug() const;
};
