#pragma once

#include "Enemy.h"

class SyntaxErrorBug : public Enemy
{
private:
    int failureChance;     // Chance (%) that attack fails completely
    int debuffChance;      // Chance (%) to apply debuff instead of damage
    bool lastAttackFailed; // Track if last attack failed

public:
    // Constructor [Header]
    SyntaxErrorBug(const std::string &enemyName, int health, int attackDmg, int failChance = 100, int debuffChance = 100);
    // Destructor [Header]
    virtual ~SyntaxErrorBug() override;
    // Take turn method - unpredictable behavior [Header]
    void takeTurn() override;
    // Override calculateAttackDamage - may return 0 if syntax fails
    int calculateAttackDamage() const override;
    // Check if last attack failed [Header]
    bool didLastAttackFail() const;
    // Get pointer to player to apply debuffs (will be set during combat)
    void applyRandomDebuff(Entity *target);
};
