#pragma once

#include "Enemy.h"

class MemoryLeakBug : public Enemy
{
private:
    int damageIncreasePerTurn;
    int turnsElapsed;

public:
    MemoryLeakBug(const std::string &enemyName, int health, int attackDmg, int damageIncrease = 2);
    virtual ~MemoryLeakBug() override;
    void takeTurn() override; 
};
