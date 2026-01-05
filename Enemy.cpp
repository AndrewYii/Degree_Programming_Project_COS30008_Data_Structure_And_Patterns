#include "Enemy.h"
#include <iostream>

// Constructor [Implementation]
Enemy::Enemy(std::string name, int health, int damage) : Entity(name, health), attackDamage(damage), attacking(false)
{
    std::cout << "[Enemy] " << name << " created with " << health << " HP" << std::endl;
}
// Destructor [Implementation]
Enemy::~Enemy()
{
    std::cout << "[Enemy] " << name << " destroyed" << std::endl;
}

void Enemy::update(float)
{
}

void Enemy::render(sf::RenderWindow &)
{
}
// Take turn method [Implementation
void Enemy::takeTurn()
{
    std::cout << "[Enemy] " << name << " attacks for " << attackDamage << " damage!" << std::endl;
}

// Set attack damage [Implementation]
void Enemy::setAttackDamage(int damage)
{
    attackDamage = damage;
}
// Get attack damage [Implementation]
int Enemy::getAttackDamage() const
{
    return attackDamage;
}

int Enemy::calculateAttackDamage() const
{
    int finalDamage = attackDamage + strength;

    if (weakStacks > 0)
    {
        finalDamage = static_cast<int>(finalDamage * 0.75f);
    }

    return (finalDamage < 0) ? 0 : finalDamage;
}