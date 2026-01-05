#include "CardIterator.h"
#include <iostream>

// Constructor [Implementation]
CardIterator::CardIterator(Card *start): current(start), startNode(start)
{
    std::cout << "[CardIterator] Created iterator at node: " << (current ? current->getName() : "nullptr") << std::endl;
}

// Check if there is a next card [Implementation]
bool CardIterator::hasNext() const
{
    return (current != (Card*)0 && current->next != (Card*)0);
}

// Move to next card and return it [Implementation]
Card *CardIterator::next()
{
    if (!hasNext())
    {
        std::cout << "[CardIterator] No more cards to iterate!" << std::endl;
        return (Card*)0;
    }

    current = current->next;
    std::cout << "[CardIterator] Moved to: " << (current ? current->getName() : "nullptr") << std::endl;

    return current;
}

// Get current card without moving [Implementation]
Card *CardIterator::getCurrent() const
{
    return current;
}

// Reset iterator to start node [Implementation]
void CardIterator::reset()
{
    current = startNode;
    std::cout << "[CardIterator] Reset to start: " << (current ? current->getName() : "nullptr") << std::endl;
}

// Check if at end [Implementation]
bool CardIterator::isEnd() const
{
    return (current == (Card*)0);
}
