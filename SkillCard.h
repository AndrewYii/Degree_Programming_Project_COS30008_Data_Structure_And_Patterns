#pragma once

#include "Card.h"

class SkillCard : public Card
{
public:
    SkillCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost);
    virtual ~SkillCard() override;
    virtual void play(Player *player, Entity *target) override = 0;
    Card *clone() const override;
};
