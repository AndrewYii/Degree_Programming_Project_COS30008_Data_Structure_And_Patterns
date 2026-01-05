#pragma once

#include "SkillCard.h"

class BinarySearchCard : public SkillCard
{
private:
    // Critical damage amount
    int critDamage;

public:
    // Constructor [Header]
    BinarySearchCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int damage);
    // Destructor [Header]
    virtual ~BinarySearchCard() override;
    // play method override [Header]
    void play(Player *player, Entity *target) override;
    // clone method override [Header]
    Card *clone() const override;

    int getCritDamage() const { return critDamage; }
    void setCritDamage(int value) { critDamage = value; }
};
