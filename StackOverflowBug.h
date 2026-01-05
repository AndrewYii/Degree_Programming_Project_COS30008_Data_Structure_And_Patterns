#pragma once

#include "Enemy.h"

class StackOverflowBug : public Enemy
{
private:
    int stackDepth;           // Current recursion depth
    int stackGrowthRate;      // How much stack grows per turn
    const int MAX_STACK = 10; // Maximum stack depth before overflow

public:
    // Constructor [Header]
    StackOverflowBug(const std::string &enemyName, int health, int attackDmg, int growthRate = 1);
    // Destructor [Header]
    virtual ~StackOverflowBug() override;
    // Take turn method - increases stack depth [Header]
    void takeTurn() override;
    // Override calculateAttackDamage to apply stack multiplier
    int calculateAttackDamage() const override;
    // Get current stack depth [Header]
    int getStackDepth() const;
};
