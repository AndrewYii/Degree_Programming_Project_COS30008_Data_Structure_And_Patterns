#pragma once

#include "Enemy.h"

class IndexOutOfBoundsBug : public Enemy
{
private:
    int chaosMultiplier; // Random damage multiplier (1-3x)

public:
    // Constructor [Header]
    IndexOutOfBoundsBug(const std::string &enemyName, int health, int attackDmg);
    // Destructor [Header]
    virtual ~IndexOutOfBoundsBug() override;
    // Take turn method [Header]
    void takeTurn() override;
    // Override calculateAttackDamage to apply chaos multiplier
    int calculateAttackDamage() const override;
};
