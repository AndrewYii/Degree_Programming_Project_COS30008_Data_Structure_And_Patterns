#pragma once

#include "SkillCard.h"

class GarbageCollectionCard : public SkillCard
{
public:
    GarbageCollectionCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost);
    virtual ~GarbageCollectionCard() override;
    void play(Player *player, Entity *target) override;
    Card *clone() const override;
};
