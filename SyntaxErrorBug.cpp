#include "SyntaxErrorBug.h"
#include "Logger.h"
#include <iostream>
#include <cstdlib>

SyntaxErrorBug::SyntaxErrorBug(const std::string &enemyName, int health, int attackDmg, int failChance, int debuffChance)
    : Enemy(enemyName, health, attackDmg), failureChance(failChance), debuffChance(debuffChance), lastAttackFailed(false)
{
    std::cout << "[SyntaxErrorBug] Created: " << name << " (Fail: " << failChance << "%, Debuff: " << debuffChance << "%)" << std::endl;
}

SyntaxErrorBug::~SyntaxErrorBug()
{
}

void SyntaxErrorBug::takeTurn()
{
    int roll = rand() % 100;

    if (roll < failureChance)
    {
        lastAttackFailed = true;
        std::cout << "[SyntaxErrorBug] " << name << " SYNTAX ERROR! Attack failed to compile!" << std::endl;
        Logger::getInstance()->logEnemyBehavior(name, "Syntax Error - Attack Failed", 0);
    }
    else
    {
        lastAttackFailed = false;
        std::cout << "[SyntaxErrorBug] " << name << " successfully executes attack!" << std::endl;
        Logger::getInstance()->logEnemyBehavior(name, "Attack Successful", attackDamage);
    }
}

// Calculate attack damage [Implementation]
int SyntaxErrorBug::calculateAttackDamage() const
{
    // If last attack failed due to syntax error, deal no damage
    if (lastAttackFailed)
    {
        std::cout << "[SyntaxErrorBug] Syntax prevented damage execution (0 damage)" << std::endl;
        return 0;
    }

    // Otherwise calculate normal damage
    int baseDamage = Enemy::calculateAttackDamage();

    std::cout << "[SyntaxErrorBug] Damage: " << baseDamage << std::endl;

    return baseDamage;
}

// Check if last attack failed [Implementation]
bool SyntaxErrorBug::didLastAttackFail() const { 
    return lastAttackFailed; 
}

void SyntaxErrorBug::applyRandomDebuff(Entity *target)
{
    if (!target || lastAttackFailed)
        return;

    int roll = rand() % 100;

    if (roll < debuffChance)
    {
        int debuffType = rand() % 3;

        switch (debuffType)
        {
        case 0:
            target->addWeak(2);
            std::cout << "[SyntaxErrorBug] Broken code applies WEAK (2 stacks) to " << target->getName() << "!" << std::endl;
            Logger::getInstance()->logEnemyBehavior(name, "Applied WEAK", 2);
            break;
        case 1:
            target->addVulnerable(2);
            std::cout << "[SyntaxErrorBug] Broken code applies VULNERABLE (2 stacks) to " << target->getName() << "!" << std::endl;
            Logger::getInstance()->logEnemyBehavior(name, "Applied VULNERABLE", 2);
            break;
        case 2:
            target->addPoison(3);
            std::cout << "[SyntaxErrorBug] Broken code applies POISON (3 stacks) to " << target->getName() << "!" << std::endl;
            Logger::getInstance()->logEnemyBehavior(name, "Applied POISON", 3);
            break;
        }
    }
}
