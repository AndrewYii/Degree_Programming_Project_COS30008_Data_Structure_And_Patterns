#pragma once

#include "Entity.h"
#include <iostream>


class TurnQueue
{
private:
    Entity *front; 
    Entity *rear;  
    int size;

public:
    // Constructor
    TurnQueue();

    // Destructor
    ~TurnQueue();

    // Enqueue - Add entity to back of queue
    void enqueue(Entity *entity);

    // Dequeue - Remove and return entity from front
    Entity *dequeue();

    // Peek - Look at front entity without removing
    Entity *peek() const;

    // Check if queue is empty
    bool isEmpty() const;

    // Get queue size
    int getSize() const { return size; }

    // Clear queue (doesn't delete entities)
    void clear();

    // Initialize turn order (add all entities)
    void initializeTurnOrder(Entity *player, Entity **enemies, int enemyCount);

    // Next turn - dequeue current, enqueue at back (circular)
    void nextTurn();

    // Remove dead entities from queue
    void removeDeadEntities();

    // Check if only player or only enemies remain
    bool onlyPlayerRemains() const;
    bool onlyEnemiesRemain() const;

    // Print queue order (for debugging)
    void printQueue() const;
};
