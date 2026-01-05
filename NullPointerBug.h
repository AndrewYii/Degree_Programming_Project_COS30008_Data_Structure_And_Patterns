#pragma once

#include "Enemy.h"

class NullPointerBug : public Enemy
{
private:
    int skipChance;

public:
    NullPointerBug(const std::string &enemyName, int health, int attackDmg, int skipPercent = 100);
    virtual ~NullPointerBug() override;
    void takeTurn() override;
};
