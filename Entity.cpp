#include "Entity.h"
#include <iostream>

// Constructor [Implementation]
Entity::Entity(std::string entityName, int health) : name(entityName), hp(health), maxHp(health), position(0.0f, 0.0f), nextInQueue(static_cast<Entity *>(0)), next(static_cast<Entity *>(0)), block(0), strength(0), dexterity(0), energy(3), maxEnergy(3), poisonStacks(0), weakStacks(0), vulnerableStacks(0), frailStacks(0)
{
  std::cout << "[Entity] Created: " << name << " (HP: " << hp << ")" << std::endl;
}

// Destructor [Implementation]
Entity::~Entity()
{
  std::cout << "[Entity] Destroyed: " << name << std::endl;
}

// Take damage (with block and vulnerability) [Implementation]
void Entity::takeDamage(int amount)
{
  // Apply vulnerability (50% more damage)
  if (vulnerableStacks > 0)
  {
    amount = static_cast<int>(amount * 1.5f);
  }

  // Block absorbs damage first
  if (block > 0)
  {
    if (amount <= block)
    {
      block -= amount;
      std::cout << "[Entity] " << name << " blocked " << amount
                << " damage. Block remaining: " << block << std::endl;
      return;
    }
    else
    {
      amount -= block;
      std::cout << "[Entity] " << name << "'s block (" << block
                << ") broken! " << amount << " damage penetrates!" << std::endl;
      block = 0;
    }
  }

  hp -= amount;
  if (hp < 0)
    hp = 0;

  std::cout << "[Entity] " << name << " took " << amount
            << " damage. HP: " << hp << "/" << maxHp << std::endl;
}

// Gain block (with dexterity and frail) [Implementation]
void Entity::gainBlock(int amount)
{
  // Apply dexterity bonus
  amount += dexterity;

  // Apply frail (25% less block)
  if (frailStacks > 0)
  {
    amount = static_cast<int>(amount * 0.75f);
  }

  block += amount;
  std::cout << "[Entity] " << name << " gained " << amount
            << " block. Total block: " << block << std::endl;
}

// Heal [Implementation]
void Entity::heal(int amount)
{
  hp += amount;
  if (hp > maxHp)
    hp = maxHp;

  std::cout << "[Entity] " << name << " healed " << amount
            << " HP. HP: " << hp << "/" << maxHp << std::endl;
}

// Apply poison damage
void Entity::applyPoison()
{
  if (poisonStacks > 0)
  {
    hp -= poisonStacks;
    if (hp < 0)
      hp = 0;
    std::cout << "[Entity] " << name << " takes " << poisonStacks
              << " poison damage! HP: " << hp << "/" << maxHp << std::endl;
    poisonStacks--; // Poison reduces by 1 each turn
  }
}

// Reduce status effects
void Entity::reduceStatusEffects()
{
  if (weakStacks > 0)
  {
    weakStacks--;
    std::cout << "[Entity] " << name << "'s Weak reduced to " << weakStacks << std::endl;
  }
  if (vulnerableStacks > 0)
  {
    vulnerableStacks--;
    std::cout << "[Entity] " << name << "'s Vulnerable reduced to " << vulnerableStacks << std::endl;
  }
  if (frailStacks > 0)
  {
    frailStacks--;
    std::cout << "[Entity] " << name << "'s Frail reduced to " << frailStacks << std::endl;
  }
}

// Reset block at turn start
void Entity::resetBlock()
{
  if (block > 0)
  {
    std::cout << "[Entity] " << name << "'s block (" << block << ") resets" << std::endl;
    block = 0;
  }
}

// Set Position [Implementation]
void Entity::setPosition(float x, float y)
{
  position.x = x;
  position.y = y;
}

// Set Position [Implementation]
void Entity::setPosition(sf::Vector2f pos)
{
  position = pos;
}

// Set HP [Implementation]
void Entity::setHP(int newHP)
{
  hp = newHP;
  if (hp < 0)
    hp = 0;
  if (hp > maxHp)
    hp = maxHp;
}
// Set Max HP [Implementation]
void Entity::setMaxHP(int newMaxHP)
{
  maxHp = newMaxHP;
  if (hp > maxHp)
    hp = maxHp;
}
// Get Name [Implementation]
std::string Entity::getName() const
{
  return name;
}
// Get HP [Implementation]
int Entity::getHP() const
{
  return hp;
}
// Get Max HP [Implementation
int Entity::getMaxHP() const
{
  return maxHp;
}
// Is Alive [Implementation]
bool Entity::isAlive() const
{
  return hp > 0;
}
// Get Position [Implementation]
sf::Vector2f Entity::getPosition() const
{
  return position;
}
// Get Block [Implementation]
int Entity::getBlock() const
{
  return block;
}
// Get Strength [Implementation]
int Entity::getStrength() const
{
  return strength;
}
// Get Dexterity [Implementation]
int Entity::getDexterity() const
{
  return dexterity;
}
// Get Energy [Implementation]
int Entity::getEnergy() const
{
  return energy;
}
// Get Max Energy [Implementation]
int Entity::getMaxEnergy() const
{
  return maxEnergy;
}
// Get Poison Stacks [Implementation]
int Entity::getPoisonStacks() const
{
  return poisonStacks;
}
// Get Weak Stacks [Implementation]
int Entity::getWeakStacks() const
{
  return weakStacks;
}
// Get Vulnerable Stacks [Implementation]
int Entity::getVulnerableStacks() const
{
  return vulnerableStacks;
}
// Get Frail Stacks [Implementation]
int Entity::getFrailStacks() const
{
  return frailStacks;
}
// Set Block [Implementation]
void Entity::setBlock(int amount)
{
  block = amount;
}
// Set Strength [Implementation]
void Entity::setStrength(int amount)
{
  strength = amount;
}
// Set Dexterity [Implementation]
void Entity::setDexterity(int amount)
{
  dexterity = amount;
}
// Set Energy [Implementation]
void Entity::setEnergy(int amount)
{
  energy = amount;
}
// Set Max Energy [Implementation]
void Entity::setMaxEnergy(int amount)
{
  maxEnergy = amount;
}
// Add Poison Stacks [Implementation]
void Entity::addPoison(int stacks)
{
  poisonStacks += stacks;
}
// Add Weak Stacks [Implementation]
void Entity::addWeak(int stacks)
{
  weakStacks += stacks;
}
// Add Vulnerable Stacks [Implementation]
void Entity::addVulnerable(int stacks)
{
  vulnerableStacks += stacks;
}
// Add Frail Stacks [Implementation]
void Entity::addFrail(int stacks)
{
  frailStacks += stacks;
}