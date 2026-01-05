#include "BinarySearchCard.h"
#include "Player.h"
#include <iostream>

// Constructor [Implementation]
BinarySearchCard::BinarySearchCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int damage)
    : SkillCard(id, cardName, desc, rarity, cost), critDamage(static_cast<int>(damage * getRarityMultiplier(rarity)))
{
    dataType = CardDataType::TREE;
    loadIcon();
}

// Destructor [Implementation]
BinarySearchCard::~BinarySearchCard()
{
}

// play method override [Implementation]
void BinarySearchCard::play(Player *player, Entity *target)
{
    if (!player || !target)
        return;

    int finalDamage = player->calculateDamage(critDamage * 2);

    std::cout << "[BinarySearchCard] " << name << " finds weakness! CRITICAL " << finalDamage << " damage!" << std::endl;
    target->takeDamage(finalDamage);
}

// clone method override [Implementation]
Card *BinarySearchCard::clone() const
{
    return new BinarySearchCard(*this);
}
