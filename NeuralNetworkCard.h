#pragma once

#include "SkillCard.h"

class NeuralNetworkCard : public SkillCard
{
private:
    int baseDamage;
    static int learningIterations; 

public:
    NeuralNetworkCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int damage);
    virtual ~NeuralNetworkCard() override;
    void play(Player *player, Entity *target) override;
    Card *clone() const override;
};
