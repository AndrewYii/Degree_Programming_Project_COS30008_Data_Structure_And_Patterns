#include "NullPointerBug.h"
#include "Logger.h"
#include <iostream>
#include <cstdlib>
#include <ctime>

NullPointerBug::NullPointerBug(const std::string &enemyName, int health, int attackDmg, int skipPercent) : Enemy(enemyName, health, attackDmg), skipChance(skipPercent)
{
    std::cout << "[NullPointerBug] Created: " << name << " (Skip Chance: " << skipChance << "%)" << std::endl;
}

NullPointerBug::~NullPointerBug()
{
}

void NullPointerBug::takeTurn()
{
    int roll = rand() % 100;
    if (roll < skipChance)
    {
        std::cout << "[NullPointerBug] " << name << " encountered a NULL POINTER! Turn skipped!" << std::endl;
        Logger::getInstance()->logEnemyBehavior(name, "Null Pointer - Turn Skipped", 0);
        return;
    }
    std::cout << "[NullPointerBug] " << name << " attacks!" << std::endl;
    Logger::getInstance()->logEnemyBehavior(name, "Attack Successful", attackDamage);
}
