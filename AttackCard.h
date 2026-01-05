#pragma once

#include "Card.h"


class AttackCard : public Card
{
private:
    // Base damage of the attack
    int baseDamage;
    // Damage multiplier (for buffs/debuffs)
    int damageMultiplier; 
public:
    // Constructor [Header]
    AttackCard(int id, const std::string &cardName, const std::string &desc,CardRarity rarity, int cost, int damage);
    // Destructor [Header]
    virtual ~AttackCard() override;
    // Override play method [Header]
    void play(Player *player, Entity *target) override;
    // Clone method [Header]
    Card *clone() const override;
    // Get Damage [Header]
    int getBaseDamage() const;
    // Set Damage [Header]
    void setBaseDamage(int damage);
};
