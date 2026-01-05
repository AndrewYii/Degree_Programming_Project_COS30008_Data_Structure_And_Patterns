#pragma once

#include <string>
#include <SFML/Graphics.hpp>
#include "Entity.h"

// Forward declaration because the parameter will use the Player class while Player Class will include Card.h, it will cause circular dependency
class Player;

class Card : public Entity
{
public:
    // Define the card types
    enum class CardType
    {
        // Deal damage
        ATTACK,
        // Make the block
        DEFENSE,
        // Skill effect cards
        SKILL,
        // Permanent buffs for the turn
        POWER
    };
    // Define card rarity
    enum class CardRarity
    {
        // Common cards
        COMMON,
        // Uncommon cards
        UNCOMMON,
        // Rare cards
        RARE,
        // Epic cards
        EPIC,
        // Legendary cards
        LEGENDARY
    };
    // Define data structure carried by the card
    enum class CardDataType
    {
        // Stack data structure which make the LIFO arrangement for extra bonus
        STACK,
        // Queue data structure which make the FIFO arrangement for extra bonus
        QUEUE,
        // Doubly-linked traversal (forward+backward) for extra bonus
        DLL,
        // Singly-linked sequential execution for extra bonus
        SLL,
        // Branching pattern (parent->children) for extra bonus
        TREE,
        // Hash grouping required for bonus for extra bonus
        HASH_TABLE,
        // Node data structure for building blocks for extra bonus
        NODE
    };
    // Protected for derived classes to inherit/access
protected:
    // For Card Data (Built-in/Game Logic)
    // Card ID
    int cardID;
    // Card Name
    std::string name;
    // Card Description
    std::string description;
    // Card Type : Attack/Defense/Skill/Power
    CardType type;
    // Card Rarity : Common/Uncommon/Rare/Epic/Legendary
    CardRarity rarity;
    // Data Structure Type : Stack/Queue/Tree/HashTable/DLL/SLL/Node
    CardDataType dataType;
    // Energy cost to play the card
    int energyCost;
    // Whether the card exhausts upon use
    bool exhausts;
    // Whether the card is ethereal (removed if not played)
    bool ethereal;
    // Whether card has been upgraded
    bool upgraded;
    // For Card Graphics
    // Card color
    sf::Color cardColor;
    // Card texture and sprite
    sf::Texture *cardTexture;
    // Card icon texture
    sf::Texture iconTexture;
    // Card icon sprite
    sf::Sprite *iconSprite;

public:
    // Next pointer for linked list in deck/hand/active bar
    Card *next;
    // Previous pointer for linked list in deck/hand/active bar
    Card *prev;
    // Constructor  [Header]
    Card(int id, const std::string &cardName, const std::string &desc, CardType cardType, CardRarity cardRarity, int cost);
    // Copy constructor for proper deep copy [Header]
    Card(const Card &other);
    // Pure virtual so that derived card has different bonus effect implementation [Header]
    virtual void play(Player *player, Entity *target) = 0;
    // Clone method for copying cards polymorphically [Header]
    virtual Card *clone() const = 0;
    // Override Entity virtual functions [Header]
    void update(float deltaTime) override;
    // Render function overridden for card rendering [Header]
    void render(sf::RenderWindow &window) override;
    // Take turn function overridden (cards don't take turns, so empty) [Header]
    void takeTurn() override;
    // Render at specific position with font and hover effect [Header]
    virtual void renderAt(sf::RenderWindow &window, sf::Font &font, sf::Vector2f pos, bool isHovered = false);
    // Getter for card id [Header]
    int getID() const;
    // Getter for the card name [Header]
    std::string getName() const;
    // Getter for the card description [Header]
    std::string getDescription() const;
    // Getter for card type [Header]
    CardType getType() const;
    // Getter for card rarity [Header]
    CardRarity getRarity() const;
    // Getter for card data type(for bonus implementation) [Header]
    CardDataType getDataType() const;
    // Getter for energy cost [Header]
    int getEnergyCost() const;
    // Getter for exhausts property [Header]
    bool doesExhaust() const;
    // Getter for ethereal property [Header]
    bool isEthereal() const;
    // Getter to check if the card is upgraded [Header]
    bool isUpgraded() const;
    // Getter for card color [Header]
    sf::Color getCardColor() const;
    // Getter for icon sprite [Header]
    sf::Sprite *getIconSprite() const;
    // Setter for card data type (for bonus implementation) [Header]
    void setDataType(CardDataType newType);
    // Setter for card exhausts property [Header]
    void setExhausts(bool value);
    // Setter for card ethereal property [Header]
    void setEthereal(bool value);
    // Upgrade function to enhance the card [Header]
    virtual void upgrade();
    // Globalise/Standardise the colour change after rarity change [Header]
    static sf::Color getColorByRarity(CardRarity rarity);
    // Globalise/Standardise the rarity name retrieval [Header]
    static std::string getRarityName(CardRarity rarity);
    // Get icon path based on data type [Header]
    std::string getIconPath() const;
    // Load icon texture [Header]
    void loadIcon();
    // Apply rarity-based scaling multiplier to card stats [Header]
    static float getRarityMultiplier(CardRarity rarity);
    // Virtual destructor for polymorphism as derived classes (AttackCard, DefenseCard, etc.) will have their own resources to clean up [Header]
    virtual ~Card();
};
