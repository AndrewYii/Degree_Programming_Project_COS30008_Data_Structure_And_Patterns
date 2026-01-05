#pragma once

#include "Card.h"

class PowerCard : public Card
{
private:
    int strengthGain;
    int dexterityGain;
    int turnsActive;

public:
    PowerCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int str = 0, int dex = 0, int turns = -1);
    virtual ~PowerCard() override;
    void play(Player *player, Entity *target) override;
    Card *clone() const override;

    int getStrengthGain() const { return strengthGain; }
    int getDexterityGain() const { return dexterityGain; }
    void setStrengthGain(int value) { strengthGain = value; }
    void setDexterityGain(int value) { dexterityGain = value; }
};
