#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Entity
{
protected:
    std::string name;
    int hp;
    int maxHp;
    sf::Vector2f position;
    int block;   
    int strength;  
    int dexterity; 
    int energy;    
    int maxEnergy;
    int poisonStacks;     
    int weakStacks;       
    int vulnerableStacks; 
    int frailStacks;     

public:
    // Next entity in turn order [Header]
    Entity *nextInQueue;
    // Next entity in overall list [Header]
    Entity *next;        
    // Constructor [Header]
    Entity(std::string entityName, int health);
    // Destructor [Header]
    virtual ~Entity();
    // Update  [Header]
    virtual void update(float deltaTime) = 0;
    // Render  [Header]          
    virtual void render(sf::RenderWindow &window) = 0;
    // Take turn  [Header] 
    virtual void takeTurn() = 0; 
    // Take damage  [Header]                      
    virtual void takeDamage(int amount);
    // Gain block  [Header]
    virtual void gainBlock(int amount);
    // Heal  [Header]
    void heal(int amount);
    // Apply poison  [Header]
    void applyPoison();
    // Reduce status effects  [Header]
    void reduceStatusEffects();
    // Reset block  [Header]
    void resetBlock();
    // Get name  [Header]
    std::string getName() const;
    // Get HP  [Header]
    int getHP() const;
    // Get max HP  [Header]
    int getMaxHP() const;
    // Is alive  [Header]
    bool isAlive() const;
    // Get position  [Header]
    sf::Vector2f getPosition() const;
    // Get block  [Header]
    int getBlock() const;
    // Get strength  [Header]
    int getStrength() const;
    // Get dexterity  [Header]
    int getDexterity() const;
    // Get energy  [Header]
    int getEnergy() const;
    // Get max energy  [Header]
    int getMaxEnergy() const;
    // Get poison stacks  [Header]
    int getPoisonStacks() const;
    // Get weak stacks  [Header]
    int getWeakStacks() const;
    // Get vulnerable stacks  [Header]
    int getVulnerableStacks() const;
    // Get frail stacks  [Header]
    int getFrailStacks() const;
    // Set position  [Header]
    void setPosition(float x, float y);
    // Set position  [Header]
    void setPosition(sf::Vector2f pos);
    // Set HP  [Header]
    void setHP(int newHP);
    // Set max HP  [Header]
    void setMaxHP(int newMaxHP);
    // Set block  [Header]
    void setBlock(int amount);
    // Set strength  [Header]
    void setStrength(int amount);
    // Set dexterity  [Header]
    void setDexterity(int amount);
    // Set energy  [Header]
    void setEnergy(int amount);
    // Set max energy  [Header]
    void setMaxEnergy(int amount);
    // Add poison stacks  [Header]
    void addPoison(int stacks);
    // Add weak stacks  [Header]
    void addWeak(int stacks);
    // Add vulnerable stacks  [Header]
    void addVulnerable(int stacks);
    // Add frail stacks  [Header]
    void addFrail(int stacks);
};
