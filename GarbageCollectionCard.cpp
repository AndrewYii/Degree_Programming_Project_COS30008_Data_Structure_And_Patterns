#include "GarbageCollectionCard.h"
#include "Player.h"
#include <iostream>

GarbageCollectionCard::GarbageCollectionCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost) : SkillCard(id, cardName, desc, rarity, cost)
{
    dataType = CardDataType::QUEUE;
    loadIcon();
}

GarbageCollectionCard::~GarbageCollectionCard()
{
}

void GarbageCollectionCard::play(Player *player, Entity *)
{
    if (!player)
        return;

    std::cout << "[GarbageCollectionCard] " << getName() << " cleaning up debuffs!" << std::endl;

    int totalRemoved = player->getPoisonStacks() + player->getWeakStacks() + player->getVulnerableStacks() + player->getFrailStacks();

    player->addPoison(-player->getPoisonStacks());
    player->addWeak(-player->getWeakStacks());
    player->addVulnerable(-player->getVulnerableStacks());
    player->addFrail(-player->getFrailStacks());

    std::cout << "  Removed " << totalRemoved << " debuff stacks!" << std::endl;
}

Card *GarbageCollectionCard::clone() const
{
    return new GarbageCollectionCard(*this);
}
