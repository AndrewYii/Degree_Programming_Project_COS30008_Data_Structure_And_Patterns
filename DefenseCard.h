#pragma once

#include "Card.h"

class DefenseCard : public Card
{
private:
    // Amount of block this card provides
    int blockAmount;

public:
    // Constructor [Header]
    DefenseCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int block);
    // Destructor [Header]
    virtual ~DefenseCard() override;
    // Override play method [Header]
    void play(Player *player, Entity *target) override;
    // Override clone method [Header]
    Card *clone() const override;
    // Get block amount [Header]
    int getBlockAmount() const;
    // Set block amount [Header]
    void setBlockAmount(int newBlock);
};
