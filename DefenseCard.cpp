#include "DefenseCard.h"
#include "Player.h"
#include "Logger.h"
#include <iostream>

// Constructor [Implementation]
DefenseCard::DefenseCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int block) : Card(id, cardName, desc, CardType::DEFENSE, rarity, cost), blockAmount(static_cast<int>(block * getRarityMultiplier(rarity)))
{
}

// Destructor [Implementation]
DefenseCard::~DefenseCard()
{
}

// Play method [Implementation]
void DefenseCard::play(Player *player, Entity *)
{
    if (!player)
        return;

    int finalBlock = player->calculateBlock(blockAmount);

    std::cout << "[DefenseCard] " << getName() << " grants " << finalBlock << " block! (base: " << blockAmount << ")" << std::endl;

    Logger::getInstance()->logBlockGained("Player", finalBlock);

    player->gainBlock(finalBlock);
}

// Clone method [Implementation]
Card *DefenseCard::clone() const
{
    return new DefenseCard(*this);
}

// Get block amount [Implementation]
int DefenseCard::getBlockAmount() const
{
    return blockAmount;
}
// Set block amount [Implementation]
void DefenseCard::setBlockAmount(int newBlock)
{
    blockAmount = newBlock;
}
