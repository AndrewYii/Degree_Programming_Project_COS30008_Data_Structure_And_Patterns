#include "AttackCard.h"
#include "Player.h"
#include "Logger.h"
#include <iostream>

// Constructor [Implementation]
AttackCard::AttackCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int damage) : Card(id, cardName, desc, CardType::ATTACK, rarity, cost), baseDamage(static_cast<int>(damage * getRarityMultiplier(rarity))), damageMultiplier(1)
{
}

// Destructor [Implementation]
AttackCard::~AttackCard()
{
}

// Override play method [Implementation]
void AttackCard::play(Player *player, Entity *target)
{
    if (!player || !target)
        return;

    int finalDamage = player->calculateDamage(baseDamage + player->getStrength());

    if (player->getWeakStacks() > 0)
    {
        finalDamage = static_cast<int>(finalDamage * 0.75f);
    }

    std::cout << "[AttackCard] " << getName() << " deals " << finalDamage << " damage! (base: " << baseDamage << ")" << std::endl;

    Logger::getInstance()->logDamageDealt("Player", finalDamage, target->getName(), false);

    target->takeDamage(finalDamage);
}

// Get Damage [Implementation]
int AttackCard::getBaseDamage() const
{
    return baseDamage;
}

// Set Damage [Implementation]
void AttackCard::setBaseDamage(int damage)
{
    baseDamage = damage;
}

// Clone method [Implementation]
Card *AttackCard::clone() const
{
    return new AttackCard(*this);
}
