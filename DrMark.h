#pragma once

#include "Enemy.h"

class DrMark : public Enemy
{
private:
    // Enrage mechanic
    bool enraged;
    // HP threshold to trigger enrage
    int rageThreshold;

public:
    // Constructor [Header]
    DrMark(const std::string &bossName = "Dr. Mark", int health = 200, int damage = 25);
    // Destructor [Header]
    virtual ~DrMark() override;
    // Boss AI with enrage mechanic [Header]
    void takeTurn() override;
    // Override takeDamage to handle enrage trigger [Header]
    void takeDamage(int damage) override;
    // Get number of attacks per turn (2 when enraged, 1 otherwise)
    int getAttackCount() const;
    // Check if enraged
    bool isEnraged() const;
};
