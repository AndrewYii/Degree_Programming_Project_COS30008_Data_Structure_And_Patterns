#include "DrMark.h"
#include "Logger.h"
#include <iostream>

DrMark::DrMark(const std::string &bossName, int health, int damage) : Enemy(bossName, health, damage), enraged(false), rageThreshold(static_cast<int>(health * 0.3f))
{
    std::cout << "[DrMark] BOSS SPAWNED: " << name << " (HP: " << maxHp << ", ATK: " << attackDamage << ", Rage at: " << rageThreshold << ")" << std::endl;
}

DrMark::~DrMark()
{
}

void DrMark::takeTurn()
{
    if (enraged)
    {
        std::cout << "[DrMark] " << name << " is ENRAGED! DOUBLE ATTACK!" << std::endl;
        Logger::getInstance()->logEnemyBehavior(name, "Enraged Double Attack", attackDamage * 2);
    }
    else
    {
        std::cout << "[DrMark] " << name << " lectures you aggressively!" << std::endl;
    }
}

void DrMark::takeDamage(int damage)
{
    Enemy::takeDamage(damage);

    if (!enraged && hp <= rageThreshold)
    {
        enraged = true;
        attackDamage = static_cast<int>(attackDamage * 1.5f);
        std::cout << "[DrMark] " << name << " ENRAGES! \"You dare challenge me?!\"" << std::endl;
        std::cout << "[DrMark] Attack power increased to " << attackDamage << "!" << std::endl;

        Logger::getInstance()->logEnemyBehavior(name, "Entered Rage Mode", attackDamage);
    }
}

int DrMark::getAttackCount() const
{
    return enraged ? 2 : 1;
}

bool DrMark::isEnraged() const {
    return enraged; 
}

