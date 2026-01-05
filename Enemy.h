#pragma once

#include "Entity.h"
#include <SFML/Graphics.hpp>

class Enemy : public Entity
{
protected:
    // Attack damage this enemy deals on its turn
    int attackDamage;

private:
    // Is the enemy currently in attacking state
    bool attacking;

public:
    // Constructor [Header]
    Enemy(std::string name, int health, int damage);
    // Destructor [Header]
    ~Enemy();
    // Update methods [Header]
    void update(float deltaTime) override;
    // Render method [Header]
    void render(sf::RenderWindow &window) override;
    // Take turn method [Header]
    void takeTurn() override;
    // Set attack damage [Header]
    void setAttackDamage(int damage);
    int getAttackDamage() const;
    virtual int calculateAttackDamage() const;
};
