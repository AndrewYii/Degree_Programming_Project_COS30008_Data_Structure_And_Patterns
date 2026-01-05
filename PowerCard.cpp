#include "PowerCard.h"
#include "Player.h"
#include <iostream>

// Constructor [Implementation]
PowerCard::PowerCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int str, int dex, int turns) : Card(id, cardName, desc, CardType::POWER, rarity, cost), strengthGain(static_cast<int>(str * getRarityMultiplier(rarity))), dexterityGain(static_cast<int>(dex * getRarityMultiplier(rarity))), turnsActive(turns)
{
}

// Destructor [Implementation]
PowerCard::~PowerCard()
{
}

// Play method [Implementation]
void PowerCard::play(Player *player, Entity *)
{
    if (!player)
        return;

    std::cout << "[Card] " << getName() << " empowers you!" << std::endl;

    if (strengthGain > 0)
    {
        player->setStrength(player->getStrength() + strengthGain);
        std::cout << "  Strength +" << strengthGain << "!" << std::endl;
    }

    if (dexterityGain > 0)
    {
        player->setDexterity(player->getDexterity() + dexterityGain);
        std::cout << "  Dexterity +" << dexterityGain << "!" << std::endl;
    }
}

// Clone method [Implementation]
Card *PowerCard::clone() const
{
    return new PowerCard(*this);
}
