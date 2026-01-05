#include "IndexOutOfBoundsBug.h"
#include "Logger.h"
#include <iostream>
#include <cstdlib>

IndexOutOfBoundsBug::IndexOutOfBoundsBug(const std::string &enemyName, int health, int attackDmg)
    : Enemy(enemyName, health, attackDmg), chaosMultiplier(1)
{
    std::cout << "[IndexOutOfBoundsBug] Created: " << name << " (Random targeting)" << std::endl;
}

IndexOutOfBoundsBug::~IndexOutOfBoundsBug()
{
}

void IndexOutOfBoundsBug::takeTurn()
{
    chaosMultiplier = (rand() % 3) + 1;
    std::cout << "[IndexOutOfBoundsBug] " << name << " attacks with CHAOS x" << chaosMultiplier << "!" << std::endl;
    Logger::getInstance()->logEnemyBehavior(name, "Chaos Multiplier", chaosMultiplier);
}

// Calculate attack damage with chaos multiplier [Implementation]
int IndexOutOfBoundsBug::calculateAttackDamage() const
{
    int baseDamage = Enemy::calculateAttackDamage(); // Get damage with strength/weak
    int finalDamage = baseDamage * chaosMultiplier;  // Apply chaos multiplier

    std::cout << "[IndexOutOfBoundsBug] Base damage: " << baseDamage
              << " x Chaos(" << chaosMultiplier << ") = " << finalDamage << std::endl;

    return finalDamage;
}
