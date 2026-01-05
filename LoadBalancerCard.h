#pragma once

#include "SkillCard.h"

class LoadBalancerCard : public SkillCard
{
private:
    int totalDamage;

public:
    LoadBalancerCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int damage);
    virtual ~LoadBalancerCard() override;
    void play(Player *player, Entity *target) override;
    Card *clone() const override;
};
