#include "TurnQueue.h"
#include "Enemy.h"
#include <iostream>

// Constructor
TurnQueue::TurnQueue(): front(static_cast<Entity *>(0)),rear(static_cast<Entity *>(0)),size(0)
{
}

// Destructor
TurnQueue::~TurnQueue()
{
    clear();
}

// ENQUEUE: Add to back
void TurnQueue::enqueue(Entity *entity)
{
    if (!entity)
        return;

    // Use next pointer for queue links
    entity->next = static_cast<Entity *>(0);

    if (isEmpty())
    {
        // First entity in queue
        front = entity;
        rear = entity;
    }
    else
    {
        // Add to rear
        rear->next = entity;
        rear = entity;
    }

    size++;
}

// DEQUEUE: Remove from front
Entity *TurnQueue::dequeue()
{
    if (isEmpty())
    {
        return static_cast<Entity *>(0);
    }

    // Remove from front
    Entity *entity = front;
    front = front->next;

    // If queue becomes empty, update rear
    if (!front)
    {
        rear = static_cast<Entity *>(0);
    }

    // Disconnect entity
    entity->next = static_cast<Entity *>(0);

    size--;
    return entity;
}

// PEEK: Look at front
Entity *TurnQueue::peek() const
{
    return front;
}

// Check if empty
bool TurnQueue::isEmpty() const
{
    return front == static_cast<Entity *>(0);
}

// Clear queue
void TurnQueue::clear()
{
    while (!isEmpty())
    {
        Entity *entity = dequeue();
        // Don't delete entity, just remove from queue
        entity->next = static_cast<Entity *>(0);
    }

    front = static_cast<Entity *>(0);
    rear = static_cast<Entity *>(0);
    size = 0;
}

// initialize turn order
void TurnQueue::initializeTurnOrder(Entity *player, Entity **enemies, int enemyCount)
{
    clear();

    // Add player first
    if (player)
    {
        enqueue(player);
    }

    // Add all enemies
    for (int i = 0; i < enemyCount; i++)
    {
        if (enemies[i])
        {
            enqueue(enemies[i]);
        }
    }

    std::cout << "[TURN QUEUE] Initialized with " << size << " entities.\n";
    printQueue();
}

// NEXT TURN: Dequeue current, enqueue at back
void TurnQueue::nextTurn()
{
    if (isEmpty())
        return;

    // Get current turn entity
    Entity *current = dequeue();

    if (current)
    {
        std::cout << "[TURN QUEUE] " << current->getName() << "'s turn ended.\n";

        // If entity still alive, add back to queue
        if (current->getHP() > 0)
        {
            enqueue(current);
        }
        else
        {
            std::cout << "[TURN QUEUE] " << current->getName() << " is dead, removed from queue.\n";
        }
    }

    // Print new turn order
    if (!isEmpty())
    {
        Entity *next = peek();
        if (next)
        {
            std::cout << "[TURN QUEUE] It's now " << next->getName() << "'s turn!\n";
        }
    }
}

// Remove dead entities from queue
void TurnQueue::removeDeadEntities()
{
    if (isEmpty())
        return;

    int originalSize = size;

    // We need to rebuild the queue without dead entities
    // Create temporary array to store alive entities
    Entity **aliveEntities = new Entity *[size];
    int aliveCount = 0;

    // Dequeue all and keep alive ones
    while (!isEmpty())
    {
        Entity *entity = dequeue();
        if (entity && entity->getHP() > 0)
        {
            aliveEntities[aliveCount++] = entity;
        }
        else if (entity)
        {
            std::cout << "[TURN QUEUE] Removed dead entity: " << entity->getName() << "\n";
        }
    }

    // Re-enqueue alive entities
    for (int i = 0; i < aliveCount; i++)
    {
        enqueue(aliveEntities[i]);
    }

    delete[] aliveEntities;

    if (originalSize != size)
    {
        std::cout << "[TURN QUEUE] Cleaned up dead entities. Queue size: " << size << "\n";
    }
}

// CHECK if only player remains
bool TurnQueue::onlyPlayerRemains() const
{
    if (isEmpty())
        return false;

    int playerCount = 0;
    int enemyCount = 0;

    Entity *current = front;
    while (current)
    {
        // Use dynamic_cast to check entity type
        if (dynamic_cast<Enemy *>(current) != nullptr)
        {
            enemyCount++;
        }
        else
        {
            // If not an enemy, it's the player
            playerCount++;
        }

        current = current->next;
    }

    return (playerCount > 0 && enemyCount == 0);
}

// CHECK if only enemies remain
bool TurnQueue::onlyEnemiesRemain() const
{
    if (isEmpty())
        return false;

    int playerCount = 0;
    int enemyCount = 0;

    Entity *current = front;
    while (current)
    {
        // Use dynamic_cast to check entity type
        if (dynamic_cast<Enemy *>(current) != nullptr)
        {
            enemyCount++;
        }
        else
        {
            // If not an enemy, it's the player
            playerCount++;
        }

        current = current->next;
    }

    return (enemyCount > 0 && playerCount == 0);
}


// print queue order
void TurnQueue::printQueue() const
{
    std::cout << "=== TURN ORDER ===\n";

    if (isEmpty())
    {
        std::cout << "Queue is empty.\n";
        return;
    }

    Entity *current = front;
    int position = 1;

    while (current)
    {
        std::cout << position << ". " << current->getName()
                  << " (HP: " << current->getHP() << "/"
                  << current->getMaxHP() << ")\n";

        current = current->next;
        position++;
    }

    std::cout << "==================\n";
}
