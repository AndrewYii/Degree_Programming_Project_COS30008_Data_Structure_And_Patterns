#pragma once

#include <SFML/Graphics.hpp>
#include <string>

class Room;

struct RoomLink
{
    Room *room;
    RoomLink *next;

    RoomLink(Room *r) : room(r), next(static_cast<RoomLink *>(0)) {}
};

class Room
{
public:
    // Room types
    enum class RoomType
    {
        START,    
        TUTORIAL, 
        COMBAT,   
        TREASURE, 
        REST,    
        SHOP,     
        BOSS,     
        EXIT      
    };

private:
    int roomID;
    RoomType type;
    std::string name;
    std::string description;
    RoomLink *childrenHead; 
    Room *parent;           
    int childCount;
    bool visited;
    bool completed;
    bool locked;
    bool hovered; 
    sf::Vector2f position;
    sf::CircleShape nodeShape;
    sf::Color roomColor;

public:
    // Constructor [Header]
    Room(int id, RoomType roomType, const std::string &roomName);
    // Destructor [Header]
    ~Room();
    // Add Child Room [Header]
    void addChild(Room *child);
    // Remove Child Room [Header]
    void removeChild(Room *child);
    // Get the head of the children linked list [Header]
    RoomLink *getChildrenHead();
    // Get number of children [Header]
    int getChildCount() const;
    // Get the parent room [Header]
    Room *getParent();
    /// Set the parent room [Header]
    void setParent(Room *parentRoom);
    // Get the room ID [Header]
    int getID() const;
    // Get the room type [Header]
    RoomType getType() const;
    // Get the room name [Header]
    std::string getName() const;
    // Get the room description [Header]
    std::string getDescription() const;
    // Get the visited status [Header]
    bool isVisited() const;
    // Get the completed status [Header]
    bool isCompleted() const;
    // Get the locked status [Header]
    bool isLocked() const;
    // Get the position [Header]
    sf::Vector2f getPosition() const;
    // Set the visited status [Header]
    void setVisited(bool value);
    // Set the completed status [Header]
    void setCompleted(bool value);
    // Set the locked status [Header]
    void setLocked(bool value);
    // Set the description [Header]
    void setDescription(const std::string &desc);
    // Set hovered status [Header]
    void setHovered(bool value) ;
    // Get hovered status [Header]
    bool isHovered() const;
    // Set position [Header]
    void setPosition(float x, float y);
    // Update graphics based on state [Header]
    void updateGraphics();
    // Render the room node and its connections [Header]
    void render(sf::RenderWindow &window, sf::Font &font);
    // Render connection line to child room [Header]
    void renderConnectionLine(sf::RenderWindow &window, Room *child);
    // Check if point is inside room node [Header] 
    bool contains(sf::Vector2f point) const;
    // Check if room is accessible (parent must be visited) [Header]
    bool isAccessible() const; 
    // Get color for room type [Header]
    static sf::Color getRoomTypeColor(RoomType type);
    // Get name for room type [Header]
    static std::string getRoomTypeName(RoomType type);
    // Get symbol for room type [Header]
    static std::string getRoomSymbol(RoomType type);
};
