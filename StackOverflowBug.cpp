#include "StackOverflowBug.h"
#include "Logger.h"
#include <iostream>
#include <cmath>

StackOverflowBug::StackOverflowBug(const std::string &enemyName, int health, int attackDmg, int growthRate)
    : Enemy(enemyName, health, attackDmg), stackDepth(1), stackGrowthRate(growthRate)
{
    std::cout << "[StackOverflowBug] Created: " << name << " (Stack grows by " << growthRate << " each turn)" << std::endl;
}

StackOverflowBug::~StackOverflowBug()
{
}

void StackOverflowBug::takeTurn()
{
    stackDepth += stackGrowthRate;

    if (stackDepth > MAX_STACK)
    {
        std::cout << "[StackOverflowBug] " << name << " STACK OVERFLOW! Maximum recursion depth reached!" << std::endl;
        std::cout << "[StackOverflowBug] Stack resets to 1 (program crash and restart)" << std::endl;
        Logger::getInstance()->logEnemyBehavior(name, "Stack Overflow - Reset", MAX_STACK);
        stackDepth = 1;
    }
    else
    {
        std::cout << "[StackOverflowBug] " << name << " recurses deeper! Stack depth: " << stackDepth << "/" << MAX_STACK << std::endl;
        Logger::getInstance()->logEnemyBehavior(name, "Stack Depth Increase", stackDepth);
    }
}

// Calculate attack damage with stack multiplier [Implementation]
int StackOverflowBug::calculateAttackDamage() const
{
    int baseDamage = Enemy::calculateAttackDamage(); // Get damage with strength/weak

    // Damage multiplies based on stack depth (deeper = more damage)
    // Formula: baseDamage * (1 + stackDepth * 0.2)
    // Stack 1 = 1.2x, Stack 5 = 2.0x, Stack 10 = 3.0x
    double multiplier = 1.0 + (stackDepth * 0.2);
    int finalDamage = static_cast<int>(baseDamage * multiplier);

    std::cout << "[StackOverflowBug] Base damage: " << baseDamage
              << " x Stack(" << stackDepth << ") = " << finalDamage << std::endl;

    return finalDamage;
}

// Get current stack depth [Implementation]
int StackOverflowBug::getStackDepth() const { 
    return stackDepth; 
}