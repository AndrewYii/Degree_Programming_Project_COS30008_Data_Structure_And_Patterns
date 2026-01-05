#include "Card.h"
#include "Player.h"
#include "Entity.h"
#include <iostream>

// Card Constructor using both initialiser and assignment approach, set some default value,card texture/sprite set to <DataType>(0) to avoid the dangling pointer issue, some dummy data has been added. Moreover, the hp is set to 1 as cards don't have health but Entity base class requires it. [Implementation]
Card::Card(int id, const std::string &cardName, const std::string &desc, CardType cardType, CardRarity cardRarity, int cost) : Entity(cardName, 1), cardID(id), name(cardName), description(desc), type(cardType), rarity(cardRarity), dataType(CardDataType::SLL), energyCost(cost), exhausts(false), ethereal(false), upgraded(false), cardTexture(static_cast<sf::Texture *>(0)), iconSprite(static_cast<sf::Sprite *>(0)), next(static_cast<Card *>(0)), prev(static_cast<Card *>(0))
{
    // Set color based on type
    switch (type)
    {
    case CardType::ATTACK:
        // Red
        cardColor = sf::Color(220, 50, 50);
        break;
    case CardType::DEFENSE:
        // Blue
        cardColor = sf::Color(50, 150, 220);
        break;
    case CardType::SKILL:
        // Green
        cardColor = sf::Color(100, 220, 100);
        break;
    case CardType::POWER:
        // Gold
        cardColor = sf::Color(220, 180, 50);
        break;
    }
}

// Copy constructor for proper deep copy [Implementation]
Card::Card(const Card &other) : Entity(other.name, 1), cardID(other.cardID), name(other.name), description(other.description), type(other.type), rarity(other.rarity), dataType(other.dataType), energyCost(other.energyCost), exhausts(other.exhausts), ethereal(other.ethereal), upgraded(other.upgraded), cardColor(other.cardColor), cardTexture(static_cast<sf::Texture *>(0)), iconSprite(static_cast<sf::Sprite *>(0)), next(static_cast<Card *>(0)), prev(static_cast<Card *>(0))
{
    // Deep copy: Create new icon sprite if the original has one
    if (other.iconSprite && other.iconTexture.getNativeHandle())
    {
        // Copy the texture
        iconTexture = other.iconTexture;
        // Create a new sprite with the copied texture
        iconSprite = new sf::Sprite(iconTexture);
    }
}

void Card::update(float) {}

void Card::render(sf::RenderWindow &window)
{
    sf::Font font;
    renderAt(window, font, position, false);
}
// Entity overrides but Cards don't take turns [Implementation]
void Card::takeTurn() {}
// Render the card at specified position, with hover effect [Implementation]
void Card::renderAt(sf::RenderWindow &window, sf::Font &font, sf::Vector2f pos, bool isHovered)
{
    // Card background
    sf::RectangleShape cardBg({120, 180});
    cardBg.setPosition(pos);
    cardBg.setFillColor(isHovered ? sf::Color(255, 255, 255) : sf::Color(240, 240, 240));
    cardBg.setOutlineColor(cardColor);
    cardBg.setOutlineThickness(isHovered ? 5.0f : 3.0f);
    window.draw(cardBg);
    // Card name
    sf::Text nameText(font, name, 14);
    nameText.setPosition({pos.x + 5, pos.y + 5});
    nameText.setFillColor(sf::Color::Black);
    window.draw(nameText);
    // Energy cost
    sf::CircleShape energyCircle(15);
    energyCircle.setPosition({pos.x + 10, pos.y + 30});
    energyCircle.setFillColor(sf::Color(100, 200, 255));
    energyCircle.setOutlineColor(sf::Color::Black);
    energyCircle.setOutlineThickness(2);
    window.draw(energyCircle);
    sf::Text costText(font, std::to_string(energyCost), 18);
    costText.setPosition({pos.x + 17, pos.y + 33});
    costText.setFillColor(sf::Color::Black);
    window.draw(costText);
    // Description (wrapped)
    sf::Text descText(font, description, 11);
    descText.setPosition({pos.x + 5, pos.y + 120});
    descText.setFillColor(sf::Color(50, 50, 50));
    window.draw(descText);
}
// Getter for card ID [Implementation]
int Card::getID() const
{
    return cardID;
}
// Getter for card name [Implementation]
std::string Card::getName() const
{
    return name;
}
// Getter for card description [Implementation]
std::string Card::getDescription() const
{
    return description;
}
// Getter for card type [Implementation]
Card::CardType Card::getType() const
{
    return type;
}
// Getter for card rarity [Implementation]
Card::CardRarity Card::getRarity() const
{
    return rarity;
}
// Getter for card data structure type [Implementation]
Card::CardDataType Card::getDataType() const
{
    return dataType;
}
// Getter for energy cost [Implementation]
int Card::getEnergyCost() const
{
    return energyCost;
}
// Getter for exhausts property [Implementation]
bool Card::doesExhaust() const
{
    return exhausts;
}
// Getter for ethereal property [Implementation]
bool Card::isEthereal() const
{
    return ethereal;
}
// Getter for upgraded property [Implementation]
bool Card::isUpgraded() const
{
    return upgraded;
}
// Getter for card color [Implementation]
sf::Color Card::getCardColor() const
{
    return cardColor;
}
// Getter for icon sprite [Implementation]
sf::Sprite *Card::getIconSprite() const
{
    return iconSprite;
}
// Setter for data type [Implementation]
void Card::setDataType(CardDataType newType)
{
    dataType = newType;
    // Auto-load icon when dataType is set
    loadIcon();
}
// Setter for exhausts property [Implementation]
void Card::setExhausts(bool value)
{
    exhausts = value;
}
// Setter for ethereal property [Implementation]
void Card::setEthereal(bool value)
{
    ethereal = value;
}
// Globalise/Standardise card color by rarity [Implementation]
sf::Color Card::getColorByRarity(CardRarity rarity)
{
    switch (rarity)
    {
    case CardRarity::COMMON:
        return sf::Color(200, 200, 200);
    case CardRarity::UNCOMMON:
        return sf::Color(100, 255, 100);
    case CardRarity::RARE:
        return sf::Color(100, 150, 255);
    case CardRarity::EPIC:
        return sf::Color(200, 100, 255);
    default:
        return sf::Color::White;
    }
}
// Get rarity name as string [Implementation]
std::string Card::getRarityName(CardRarity rarity)
{
    switch (rarity)
    {
    case CardRarity::COMMON:
        return "Common";
    case CardRarity::UNCOMMON:
        return "Uncommon";
    case CardRarity::RARE:
        return "Rare";
    case CardRarity::EPIC:
        return "Epic";
    default:
        return "Unknown";
    }
}
// Get icon file path based on data type [Implementation]
std::string Card::getIconPath() const
{
    switch (dataType)
    {
    case CardDataType::STACK:
        return "asset/icon/stack.png";
    case CardDataType::QUEUE:
        return "asset/icon/queue.png";
    case CardDataType::TREE:
        return "asset/icon/tree.png";
    case CardDataType::HASH_TABLE:
        return "asset/icon/hash.png";
    case CardDataType::DLL:
        return "asset/icon/dll.png";
    case CardDataType::NODE:
        return "asset/icon/node.png";
    case CardDataType::SLL:
        return "asset/icon/sll.png";
    default:
        return "asset/icon/none.png";
    }
}
// Load icon texture and create sprite [Implementation]
void Card::loadIcon()
{
    std::string iconPath = getIconPath();

    if (iconSprite)
    {
        delete iconSprite;
        iconSprite = (sf::Sprite *)0;
    }

    if (!iconTexture.loadFromFile(iconPath))
    {
        std::cerr << "[Card] Failed to load icon: " << iconPath << std::endl;
        iconSprite = (sf::Sprite *)0;
        return;
    }

    // Create sprite with texture
    iconSprite = new sf::Sprite(iconTexture);
    std::cout << "[Card] Loaded icon for " << name << ": " << iconPath << std::endl;
}
// Upgrade the card (base implementation) [Implementation]
void Card::upgrade()
{
    if (upgraded)
    {
        std::cout << "[Card] " << name << " is already upgraded!" << std::endl;
        return;
    }

    upgraded = true;
    name = name + "+";
    std::cout << "[Card] Upgraded: " << name << std::endl;
}

// Get rarity multiplier for stat scaling [Implementation]
float Card::getRarityMultiplier(CardRarity rarity)
{
    switch (rarity)
    {
    case CardRarity::COMMON:
        return 1.0f;
    case CardRarity::UNCOMMON:
        return 1.5f;
    case CardRarity::RARE:
        return 2.5f;
    case CardRarity::EPIC:
        return 3.5f;
    case CardRarity::LEGENDARY:
        return 5.0f;
    default:
        return 1.0f;
    }
}

// Virtual destructor for polymorphism as derived classes (AttackCard, DefenseCard, etc.) will have their own resources to clean up [Implementation]
Card::~Card()
{
    // Clean up icon sprite
    if (iconSprite)
    {
        delete iconSprite;
        iconSprite = (sf::Sprite *)0;
    }
}