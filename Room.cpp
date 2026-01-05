#include "Room.h"
#include "Game.h"
#include <iostream>

// Constructor [Implementation]
Room::Room(int id, RoomType roomType, const std::string &roomName) : roomID(id), type(roomType), name(roomName), childrenHead(static_cast<RoomLink *>(0)), parent(static_cast<Room *>(0)), childCount(0), visited(false), completed(false), locked(false), hovered(false), position(0, 0)
{
    roomColor = getRoomTypeColor(type);
    nodeShape.setRadius(20.0f);
    nodeShape.setOrigin({20.0f, 20.0f});
    nodeShape.setFillColor(roomColor);
    nodeShape.setOutlineColor(Game::COLOR_BLACK);
    nodeShape.setOutlineThickness(3.0f);
    std::cout << "[Room] Created: " << name << " (ID: " << roomID << ")" << std::endl;
}

// Destructor [Implementation]
Room::~Room()
{
    RoomLink *current = childrenHead;
    while (current)
    {
        RoomLink *next = current->next;
        delete current;
        current = next;
    }

    std::cout << "[Room] Destroyed: " << name << " (ID: " << roomID << ")" << std::endl;
}

// Add Child Room [Implementation]
void Room::addChild(Room *child)
{
    if (!child)
        return;

    // Create new link node
    RoomLink *newLink = new RoomLink(child);

    newLink->next = childrenHead;
    childrenHead = newLink;
    childCount++;

    child->setParent(this);
    std::cout << "[Room] " << name << " -> " << child->getName() << " (child added)" << std::endl;
}

// Remove Child Room [Implementation]
void Room::removeChild(Room *child)
{
    if (!childrenHead || !child)
        return;

    // Special case: removing head
    if (childrenHead->room == child)
    {
        RoomLink *temp = childrenHead;
        childrenHead = childrenHead->next;
        delete temp;
        childCount--;
        std::cout << "[Room] Child removed from " << name << std::endl;
        return;
    }

    // Search and remove from middle/end
    RoomLink *current = childrenHead;
    while (current->next)
    {
        if (current->next->room == child)
        {
            RoomLink *temp = current->next;
            current->next = temp->next;
            delete temp;
            childCount--;
            std::cout << "[Room] Child removed from " << name << std::endl;
            return;
        }
        current = current->next;
    }
}

// Set position [Implementation]
void Room::setPosition(float x, float y)
{
    position = sf::Vector2f(x, y);
    nodeShape.setPosition(position);
}

// Update graphics based on state [Implementation]
void Room::updateGraphics()
{
    // Determine color based on state
    sf::Color displayColor = roomColor;

    if (completed)
    {
        displayColor = sf::Color(70, 70, 70);
    }
    else if (visited)
    {
        displayColor = sf::Color(140, 140, 140);
    }
    else if (!isAccessible())
    {
        displayColor = sf::Color(100, 100, 100);
    }
    else if (hovered)
    {
        displayColor.r = static_cast<std::uint8_t>(std::min(255, roomColor.r + 60));
        displayColor.g = static_cast<std::uint8_t>(std::min(255, roomColor.g + 60));
        displayColor.b = static_cast<std::uint8_t>(std::min(255, roomColor.b + 60));
    }

    nodeShape.setFillColor(displayColor);
}

// Check if room is accessible (parent must be visited) [Implementation]
bool Room::isAccessible() const
{
    if (!parent)
        return true;
    return parent->isVisited();
}

// Render connection line to child room [Implementation]
void Room::renderConnectionLine(sf::RenderWindow &window, Room *child)
{
    if (!child)
        return;

    sf::Vector2f startPos = position;
    sf::Vector2f endPos = child->getPosition();

    // Determine line color based on accessibility
    sf::Color lineColor = Game::COLOR_BLACK;
    float thickness = 2.0f;

    if (child->isAccessible() && !child->isVisited())
    {
        lineColor = sf::Color(180, 180, 180);
        thickness = 3.0f;
    }
    else if (child->isVisited())
    {
        lineColor = sf::Color(100, 100, 100);
    }

    sf::Vertex line[] = {sf::Vertex({startPos, lineColor}), sf::Vertex({endPos, lineColor})};
    window.draw(line, 2, sf::PrimitiveType::Lines);
}

// Render the room node and its connections [Implementation]
void Room::render(sf::RenderWindow &window, sf::Font &font)
{
    updateGraphics();
    float radius = 20.0f;
    sf::CircleShape circle(radius);
    circle.setOrigin({radius, radius});
    circle.setPosition(position);
    circle.setFillColor(nodeShape.getFillColor());
    circle.setOutlineColor(Game::COLOR_BLACK);

    // Thicker outline for accessible rooms
    if (isAccessible() && !visited && !completed)
    {
        circle.setOutlineThickness(4.0f);
        circle.setOutlineColor(sf::Color(255, 255, 255));
    }
    else if (hovered)
    {
        circle.setOutlineThickness(5.0f);
        circle.setOutlineColor(sf::Color(100, 200, 255));
    }
    else
    {
        circle.setOutlineThickness(3.0f);
    }

    window.draw(circle);
    std::string symbol = getRoomSymbol(type);
    sf::Text symbolText(font, symbol, 18);
    symbolText.setFillColor(Game::COLOR_BLACK);
    symbolText.setStyle(sf::Text::Bold);

    sf::FloatRect symbolBounds = symbolText.getLocalBounds();
    symbolText.setOrigin({symbolBounds.size.x / 2.0f + symbolBounds.position.x, symbolBounds.size.y / 2.0f + symbolBounds.position.y});
    symbolText.setPosition({position.x, position.y - 2.0f});
    window.draw(symbolText);
}

// Get symbol for room type [Implementation]
std::string Room::getRoomSymbol(RoomType type)
{
    switch (type)
    {
    case RoomType::START:
        return "S";
    case RoomType::COMBAT:
        return "!";
    case RoomType::TREASURE:
        return "$";
    case RoomType::REST:
        return "+";
    case RoomType::SHOP:
        return "#";
    case RoomType::BOSS:
        return "B";
    case RoomType::EXIT:
        return "*";
    default:
        return "?";
    }
}

// Get name for room type [Implementation]
std::string Room::getRoomTypeName(RoomType type)
{
    switch (type)
    {
    case RoomType::START:
        return "Start";
    case RoomType::COMBAT:
        return "Combat";
    case RoomType::TREASURE:
        return "Treasure";
    case RoomType::REST:
        return "Rest";
    case RoomType::SHOP:
        return "Shop";
    case RoomType::BOSS:
        return "Boss";
    case RoomType::EXIT:
        return "Exit";
    default:
        return "Unknown";
    }
}

// Check if point is inside room node [Implementation]
bool Room::contains(sf::Vector2f point) const
{
    float radius = 20.0f;
    float dx = point.x - position.x;
    float dy = point.y - position.y;
    float distanceSquared = dx * dx + dy * dy;

    return distanceSquared <= (radius * radius);
}

// Get color for room type [Implementation]
sf::Color Room::getRoomTypeColor(RoomType type)
{
    switch (type)
    {
    case RoomType::START:
        return sf::Color(220, 220, 220);
    case RoomType::COMBAT:
        return sf::Color(140, 140, 140);
    case RoomType::TREASURE:
        return sf::Color(200, 200, 200);
    case RoomType::REST:
        return sf::Color(180, 180, 180);
    case RoomType::SHOP:
        return sf::Color(160, 160, 160);
    case RoomType::BOSS:
        return sf::Color(100, 100, 100);
    case RoomType::EXIT:
        return sf::Color(240, 240, 240);
    default:
        return sf::Color::White;
    }
}
// Get Children Head [Implementation]
RoomLink *Room::getChildrenHead()
{
    return childrenHead;
}
// Get number of children [Implementation]
int Room::getChildCount() const
{
    return childCount;
}
// Get the parent room [Implementation]
Room *Room::getParent()
{
    return parent;
}
// Set the parent room [Implementation]
void Room::setParent(Room *parentRoom)
{
    parent = parentRoom;
}
// Get the room ID [Implementation]
int Room::getID() const
{
    return roomID;
}
// Get the room type [Implementation]
Room::RoomType Room::getType() const
{
    return type;
}
// Get the room name [Implementation]
std::string Room::getName() const
{
    return name;
}
// Get the room description [Implementation]
std::string Room::getDescription() const
{
    return description;
}
// Get the visited status [Implementation]
bool Room::isVisited() const
{
    return visited;
}
// Get the completed status [Implementation]
bool Room::isCompleted() const
{
    return completed;
}
// Get the locked status [Implementation]
bool Room::isLocked() const
{
    return locked;
}
// Get the position [Implementation]
sf::Vector2f Room::getPosition() const
{
    return position;
}
// Set visited status [Implementation]
void Room::setVisited(bool value)
{
    visited = value;
}
// Set completed status [Implementation]
void Room::setCompleted(bool value)
{
    completed = value;
}
// Set locked status [Implementation]
void Room::setLocked(bool value)
{
    locked = value;
}
// Set description [Implementation]
void Room::setDescription(const std::string &desc)
{
    description = desc;
}
// Set hovered status [Implementation]
void Room::setHovered(bool value)
{
    hovered = value;
}
// Get hovered status [Implementation]
bool Room::isHovered() const
{
    return hovered;
}
