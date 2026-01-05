#include "BugMonster.h"
#include "Logger.h"
#include <iostream>
#include <cstdlib>

BugMonster::BugMonster(const std::string &enemyName, int health, int attackDmg)
    : Enemy(enemyName, health, attackDmg), mutationStage(0), hasReplicated(false)
{
    std::cout << "[BugMonster] Created: " << name << " (Adaptive mutation bug)" << std::endl;
}

BugMonster::~BugMonster()
{
}

void BugMonster::takeTurn()
{
    int currentHealthPercent = (hp * 100) / maxHp;

    if (currentHealthPercent <= 30 && !hasReplicated)
    {
        hasReplicated = true;
        std::cout << "[BugMonster] " << name << " attempts to REPLICATE (spawn another bug)!" << std::endl;
        Logger::getInstance()->logEnemyBehavior(name, "Replication Triggered", currentHealthPercent);
    }

    // Mutation: 100% chance to mutate each turn (up to stage 3)
    if (mutationStage < 3 && rand() % 100 < 100)
    {
        mutationStage++;
        std::cout << "[BugMonster] " << name << " MUTATED to stage " << mutationStage << "!" << std::endl;
        Logger::getInstance()->logEnemyBehavior(name, "Mutation Stage", mutationStage);
    }

    std::cout << "[BugMonster] " << name << " attacks with mutation stage " << mutationStage << "!" << std::endl;
}

int BugMonster::calculateAttackDamage() const
{
    int baseDamage = Enemy::calculateAttackDamage();

    int mutationBonus = mutationStage * 2;
    int finalDamage = baseDamage + mutationBonus;

    std::cout << "[BugMonster] Base: " << baseDamage
              << " + Mutation(" << mutationStage << "):" << mutationBonus
              << " = " << finalDamage << std::endl;

    return finalDamage;
}

bool BugMonster::canReplicate() const
{
    return hasReplicated;
}
