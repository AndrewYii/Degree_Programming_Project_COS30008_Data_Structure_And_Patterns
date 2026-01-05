#include "MemoryLeakBug.h"
#include "Logger.h"
#include <iostream>

MemoryLeakBug::MemoryLeakBug(const std::string &enemyName, int health, int attackDmg, int damageIncrease) : Enemy(enemyName, health, attackDmg), damageIncreasePerTurn(damageIncrease), turnsElapsed(0)
{
    std::cout << "[MemoryLeakBug] Created: " << name << " (Damage escalates by +" << damageIncrease << "/turn)" << std::endl;
}

MemoryLeakBug::~MemoryLeakBug()
{
}

void MemoryLeakBug::takeTurn()
{
    turnsElapsed++;
    int bonusDamage = damageIncreasePerTurn * turnsElapsed;
    std::cout << "[MemoryLeakBug] " << name << " attacks! (Base: " << attackDamage << " + Leaked: +" << bonusDamage << " = " << (attackDamage + bonusDamage) << ")" << std::endl;

    Logger::getInstance()->logEnemyBehavior(name, "Damage Escalation", bonusDamage);

    attackDamage += damageIncreasePerTurn;
}
