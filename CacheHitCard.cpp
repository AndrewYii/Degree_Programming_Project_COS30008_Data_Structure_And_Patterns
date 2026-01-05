#include "CacheHitCard.h"
#include "Player.h"
#include <iostream>

CacheHitCard::CacheHitCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int draw) : SkillCard(id, cardName, desc, rarity, cost), drawAmount(draw)
{
    dataType = CardDataType::HASH_TABLE;
    loadIcon();
}

CacheHitCard::~CacheHitCard()
{
}

void CacheHitCard::play(Player *player, Entity *)
{
    if (!player)
        return;

    std::cout << "[CacheHitCard] " << getName() << " activated!" << std::endl;

    if (drawAmount > 0)
    {
        player->addBonusDrawNextTurn(drawAmount);
        std::cout << "  Will draw +" << drawAmount << " extra cards next turn!" << std::endl;
    }
}

Card *CacheHitCard::clone() const
{
    return new CacheHitCard(*this);
}
