#include "SkillCard.h"
#include "Player.h"
#include <iostream>

SkillCard::SkillCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost) : Card(id, cardName, desc, CardType::SKILL, rarity, cost)
{
    dataType = CardDataType::NODE;
    loadIcon();
}

SkillCard::~SkillCard()
{
}

Card *SkillCard::clone() const
{
    return nullptr;
}
