#pragma once

#include "Card.h"

class CardIterator
{
private:
    // Current node in iteration
    Card *current;   
    // Starting node for reset
    Card *startNode; 

public:
    // Constructor [Header]
    CardIterator(Card *start);
    // Check if there is a next card [Header]
    bool hasNext() const;
    // Move to next card and return it [Header]
    Card *next();
    // Get current card without moving [Header]
    Card *getCurrent() const;
    // Reset iterator to start node [Header]
    void reset();
    // Check if at end [Header]
    bool isEnd() const;
};
