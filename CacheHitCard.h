#pragma once

#include "SkillCard.h"

class CacheHitCard : public SkillCard
{
private:
    int drawAmount;

public:
    CacheHitCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int draw);
    virtual ~CacheHitCard() override;
    void play(Player *player, Entity *target) override;
    Card *clone() const override;
};
