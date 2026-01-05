#pragma once

#include "Enemy.h"

class BugMonster : public Enemy
{
private:
    int mutationStage;
    bool hasReplicated;

public:
    BugMonster(const std::string &enemyName, int health, int attackDmg);
    virtual ~BugMonster() override;
    void takeTurn() override;
    int calculateAttackDamage() const override;
    bool canReplicate() const;
};
