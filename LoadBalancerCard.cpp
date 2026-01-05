#include "LoadBalancerCard.h"
#include "Player.h"
#include <iostream>

LoadBalancerCard::LoadBalancerCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int damage) : SkillCard(id, cardName, desc, rarity, cost), totalDamage(damage)
{
    dataType = CardDataType::DLL;
    loadIcon();
}

LoadBalancerCard::~LoadBalancerCard()
{
}

void LoadBalancerCard::play(Player *player, Entity *target)
{
    if (!player || !target)
        return;

    int finalDamage = player->calculateDamage(totalDamage);
    std::cout << "[LoadBalancerCard] " << getName() << " distributing " << finalDamage << " damage!" << std::endl;
    target->takeDamage(finalDamage);
}

Card *LoadBalancerCard::clone() const
{
    return new LoadBalancerCard(*this);
}
