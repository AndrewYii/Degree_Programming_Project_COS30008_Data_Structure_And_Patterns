#include "NeuralNetworkCard.h"
#include "Player.h"
#include <iostream>

int NeuralNetworkCard::learningIterations = 0;

// Constructor [Implementation]
NeuralNetworkCard::NeuralNetworkCard(int id, const std::string &cardName, const std::string &desc, CardRarity rarity, int cost, int damage)
    : SkillCard(id, cardName, desc, rarity, cost), baseDamage(damage)
{
    dataType = CardDataType::NODE;
    loadIcon();
}
// Destructor [Implementation]
NeuralNetworkCard::~NeuralNetworkCard()
{
}

// Override play method [Implementation]
void NeuralNetworkCard::play(Player *player, Entity *target)
{
    if (!player || !target)
        return;

    learningIterations++;
    int adaptiveDamage = player->calculateDamage(baseDamage + (learningIterations * 2));
    std::cout << "[NeuralNetworkCard] " << name << " adapts! Deals " << adaptiveDamage << " damage (Learning: " << learningIterations << ")" << std::endl;
    target->takeDamage(adaptiveDamage);
}

// Clone method [Implementation]
Card *NeuralNetworkCard::clone() const
{
    return new NeuralNetworkCard(*this);
}
