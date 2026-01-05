#include "Map.h"
#include "AIStoryteller.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <vector>

// Constructor [Implementation]
Map::Map() : root(static_cast<Room *>(0)), currentRoom(static_cast<Room *>(0)), allRoomsHead(static_cast<RoomContainer *>(0)), totalRoomCount(0), nextRoomID(0), currentStage(1), animFrameCounter(0), generating(false), texturesLoaded(false), hoverFloatOffset(0.0f)
{
    std::cout << "[Map] Created" << std::endl;
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
    loadCartridgeTextures();
}

// Destructor [Implementation]
Map::~Map()
{
    clear();
    std::cout << "[Map] Destroyed" << std::endl;
}

// Add room to container list [Implementation]
void Map::addRoomToContainer(Room *room)
{
    RoomContainer *newContainer = new RoomContainer(room);
    newContainer->next = allRoomsHead;
    allRoomsHead = newContainer;
    totalRoomCount++;
}

// Generate map - dungeon depth mode (binary tree)
void Map::generateMap(int , int , float mapWidth, float mapHeight, sf::RenderWindow *window, sf::Font *font)
{
    clear();
    generating = true;
    animFrameCounter = 0;

    std::cout << "[Map] Generating Stage " << currentStage << " map (dungeon depth mode)" << std::endl;

    // Update AIStoryteller with current stage
    AIStoryteller::getInstance()->setCurrentStage(currentStage);

    // Create starting room (root of tree) - just a placeholder, not shown
    Room *startRoom = new Room(nextRoomID++, Room::RoomType::START, "Beginning");
    root = startRoom;
    currentRoom = root;
    root->setVisited(true);
    addRoomToContainer(startRoom);

    // Position at center (not visible in door mode, but kept for data structure)
    float startX = mapWidth * 0.5f;
    float startY = mapHeight * 0.1f;
    root->setPosition(startX, startY);

    int maxDepth;
    if (currentStage == 1)
    {
        maxDepth = 6 + (std::rand() % 3);
    }
    else if (currentStage == 2)
    {
        maxDepth = 7 + (std::rand() % 3);
    }
    else
    {
        maxDepth = 8 + (std::rand() % 3);
    }

    std::cout << "[Map] Stage " << currentStage << " depth: " << maxDepth << std::endl;

    float horizontalSpacing = mapWidth * 0.15f;
    float verticalStep = mapHeight * 0.15f;

    // Initialize counters for room type distribution
    int combatCount = 0;
    int treasureCount = 0;
    int restCount = 0;
    int shopCount = 0;

    // Generate binary tree - each node has exactly 2 children (2 doors)
    generateBranch(root, 1, maxDepth, 2, startX, startY, horizontalSpacing, verticalStep, combatCount, treasureCount, restCount, shopCount, window, font);

    std::cout << "[Map] Generated " << totalRoomCount << " rooms (depth: " << maxDepth << ")" << std::endl;

    generating = false;
}

// Regenerate map for next stage
void Map::regenerateForNextStage(float mapWidth, float mapHeight, sf::RenderWindow *window, sf::Font *font)
{
    currentStage++;
    if (currentStage > 3)
        currentStage = 3; // Max 3 stages

    std::cout << "[Map] Moving to stage " << currentStage << std::endl;

    // Update AIStoryteller with new stage
    AIStoryteller::getInstance()->setCurrentStage(currentStage);

    generateMap(3, 2, mapWidth, mapHeight, window, font);
}

// Recursive branch generation (binary tree)
void Map::generateBranch(Room *parentRoom, int currentDepth, int maxDepth, int branching, float x, float y, float horizontalSpacing, float verticalSpread, int &combatCount, int &treasureCount, int &restCount, int &shopCount, sf::RenderWindow *window, sf::Font *font)
{
    // Show loading screen every few rooms (without animated dots to avoid cramping)
    if (window && font)
    {
        animFrameCounter++;
        if (animFrameCounter % 3 == 0) // Show every 3 rooms
        {
            renderLoadingScreen(*window, *font, "Generating Map...");
        }
    }

    if (currentDepth > maxDepth)
    {
        return;
    }

    // ALWAYS 2 children (binary tree = 2 doors per room)
    int numChildren = 2;

    // Exception: Last level has 1 boss/exit
    if (currentDepth == maxDepth)
    {
        numChildren = 1;
    }

    // Exception: First level has 1 tutorial ONLY on stage 1
    if (currentDepth == 1 && currentStage == 1)
    {
        numChildren = 1;
    }

    // Position for next level (move down vertically for depth)
    float nextY = y + verticalSpread;

    for (int i = 0; i < numChildren; i++)
    {
        // Determine room type with variety
        Room::RoomType roomType;

        if (currentDepth == maxDepth)
        {
            if (currentStage == 3)
            {
                roomType = Room::RoomType::BOSS;
            }
            else
            {
                roomType = Room::RoomType::EXIT;
            }
        }
        else if (currentDepth == 1 && currentStage == 1)
        {
            // First level ONLY on stage 1: tutorial room
            roomType = Room::RoomType::START;
        }
        else
        {
            // Combat-heavy distribution - maximum combat encounters
            std::vector<Room::RoomType> roomPool;

            // Add with adjusted weights - maximum combat focus (70%)
            if (combatCount < 18)
            {
                for (int w = 0; w < 28; w++) // 28 entries = 70% weight
                    roomPool.push_back(Room::RoomType::COMBAT);
            }
            if (treasureCount < 4)
            {
                for (int w = 0; w < 4; w++) // 4 entries = 10% weight
                    roomPool.push_back(Room::RoomType::TREASURE);
            }
            if (restCount < 3)
            {
                for (int w = 0; w < 4; w++) // 4 entries = 10% weight
                    roomPool.push_back(Room::RoomType::REST);
            }
            if (shopCount < 3)
            {
                for (int w = 0; w < 4; w++) // 4 entries = 10% weight
                    roomPool.push_back(Room::RoomType::SHOP);
            }

            if (!roomPool.empty())
            {
                int randomIndex = std::rand() % roomPool.size();
                roomType = roomPool[randomIndex];

                // Update counters
                if (roomType == Room::RoomType::COMBAT)
                    combatCount++;
                else if (roomType == Room::RoomType::TREASURE)
                    treasureCount++;
                else if (roomType == Room::RoomType::REST)
                    restCount++;
                else if (roomType == Room::RoomType::SHOP)
                    shopCount++;
            }
            else
            {
                // Fallback if all limits reached: prefer combat, otherwise cycle
                int fallbackChoice = std::rand() % 4;
                if (fallbackChoice == 0)
                    roomType = Room::RoomType::COMBAT;
                else if (fallbackChoice == 1)
                    roomType = Room::RoomType::TREASURE;
                else if (fallbackChoice == 2)
                    roomType = Room::RoomType::REST;
                else
                    roomType = Room::RoomType::SHOP;
            }
        }

        // Create room
        std::string roomName = Room::getRoomTypeName(roomType);
        if (roomType == Room::RoomType::START)
        {
            roomName = "Tutorial - Begin Your Journey"; // Single tutorial
        }
        Room *newRoom = new Room(nextRoomID++, roomType, roomName);

        // Position for binary tree (left/right positioning)
        float childX;
        if (numChildren == 1)
        {
            // Single child (boss): centered
            childX = x;
        }
        else
        {
            // Two children: spread left and right (2 doors)
            childX = x + (i == 0 ? -horizontalSpacing : horizontalSpacing);
        }

        newRoom->setPosition(childX, nextY);

        // Add to tree structure
        parentRoom->addChild(newRoom);
        addRoomToContainer(newRoom);

        // Recursively generate children (pass counters by reference, and window/font for animation)
        generateBranch(newRoom, currentDepth + 1, maxDepth, branching, childX, nextY, horizontalSpacing * 0.8f, verticalSpread, combatCount, treasureCount, restCount, shopCount, window, font);
    }
}

// Clear map and free memory
void Map::clear()
{
    // Delete all room containers and rooms
    RoomContainer *current = allRoomsHead;
    while (current)
    {
        RoomContainer *next = current->next;
        delete current->room; // Delete the actual room
        delete current;       // Delete the container
        current = next;
    }

    allRoomsHead = static_cast<RoomContainer *>(0);
    root = static_cast<Room *>(0);
    currentRoom = static_cast<Room *>(0);
    totalRoomCount = 0;
    nextRoomID = 0;
    std::cout << "[Map] Cleared" << std::endl;
}

// Move to specified room if accessible
void Map::moveToRoom(Room *room)
{
    if (room && canMoveToRoom(room))
    {
        currentRoom = room;
        room->setVisited(true);
        std::cout << "[Map] Moved to: " << room->getName() << std::endl;
    }
}

// Check if room is accessible from current room
bool Map::canMoveToRoom(Room *room)
{
    if (!room || !currentRoom)
        return false;

    // Can only move to children of current room
    RoomLink *child = currentRoom->getChildrenHead();
    while (child)
    {
        if (child->room == room)
        {
            return true;
        }
        child = child->next;
    }

    return false;
}

// Render map and rooms
void Map::render(sf::RenderWindow &window, sf::Font &font)
{
    // Get window size for responsive positioning
    sf::Vector2u windowSize = window.getSize();
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    // Draw title
    sf::Text titleText(font, "Choose Your Path", 48);
    titleText.setFillColor(sf::Color::White);
    titleText.setPosition({centerX - 100.0f, 30.0f});
    window.draw(titleText);

    // Get available next rooms (children of current room)
    if (!currentRoom)
        return;

    RoomLink *child = currentRoom->getChildrenHead();
    int doorCount = 0;

    // Count doors
    RoomLink *temp = child;
    while (temp)
    {
        doorCount++;
        temp = temp->next;
    }

    // Special case: Single tutorial or boss room (centered, same size and style as regular rooms)
    if (doorCount == 1 && (child->room->getType() == Room::RoomType::START ||
                           child->room->getType() == Room::RoomType::BOSS ||
                           child->room->getType() == Room::RoomType::EXIT))
    {
        float doorWidth = 350.0f;                           // Same as regular rooms
        float doorHeight = 450.0f;                          // Same as regular rooms
        float doorX = centerX - doorWidth / 2.0f + 100.0f;  // Move to the right
        float doorY = centerY - doorHeight / 2.0f - 150.0f; // Same offset as regular rooms

        Room *room = child->room;

        // Determine which texture to use
        std::string textureKey;
        sf::Color fallbackColor;
        if (room->getType() == Room::RoomType::START)
        {
            textureKey = "lightblue";
            fallbackColor = sf::Color(50, 150, 200);
        }
        else if (room->getType() == Room::RoomType::BOSS)
        {
            textureKey = "purple";
            fallbackColor = sf::Color(150, 0, 150);
        }
        else
        {
            textureKey = "cyan";
            fallbackColor = sf::Color(0, 200, 200);
        }

        // Apply floating effect when hovered
        float floatY = room->isHovered() ? hoverFloatOffset : 0.0f;
        float actualDoorY = doorY + floatY;

        // Try to use sprite first
        if (texturesLoaded && cartridgeTextures.find(textureKey) != cartridgeTextures.end())
        {
            // SPRITE RENDERING
            sf::Sprite cartridgeSprite(cartridgeTextures[textureKey]);

            // Scale sprite to fit door size (350x450)
            sf::Vector2u texSize = cartridgeTextures[textureKey].getSize();
            float scaleX = doorWidth / texSize.x;
            float scaleY = doorHeight / texSize.y;
            cartridgeSprite.setScale({scaleX, scaleY});
            cartridgeSprite.setPosition({doorX, actualDoorY});

            // Apply brightness when hovered
            if (room->isHovered())
            {
                cartridgeSprite.setColor(sf::Color(255, 255, 255, 255)); // Full brightness
            }
            else
            {
                cartridgeSprite.setColor(sf::Color(180, 180, 180, 255)); // Slightly darker
            }

            window.draw(cartridgeSprite);
        }
        else
        {
            // FALLBACK: Geometric shapes
            sf::RectangleShape cartridgeBody({doorWidth, doorHeight});
            cartridgeBody.setPosition({doorX, actualDoorY});

            sf::Color cartridgeColor = fallbackColor;

            // Darken when not hovered, brighten when hovered
            if (room->isHovered())
            {
                cartridgeColor.r = static_cast<uint8_t>(std::min(255, cartridgeColor.r + 60));
                cartridgeColor.g = static_cast<uint8_t>(std::min(255, cartridgeColor.g + 60));
                cartridgeColor.b = static_cast<uint8_t>(std::min(255, cartridgeColor.b + 60));
            }
            else
            {
                cartridgeColor.r = static_cast<uint8_t>(cartridgeColor.r * 0.7f);
                cartridgeColor.g = static_cast<uint8_t>(cartridgeColor.g * 0.7f);
                cartridgeColor.b = static_cast<uint8_t>(cartridgeColor.b * 0.7f);
            }

            cartridgeBody.setOutlineColor(sf::Color(30, 30, 30));
            cartridgeBody.setOutlineThickness(4.0f);

            cartridgeBody.setFillColor(cartridgeColor);
            window.draw(cartridgeBody);

            // Cartridge notch at top (pull tab like Game Boy)
            sf::RectangleShape notch({doorWidth * 0.6f, 30.0f});
            notch.setPosition({doorX + doorWidth * 0.2f, actualDoorY - 20.0f});
            notch.setFillColor(sf::Color(static_cast<uint8_t>(cartridgeColor.r * 0.8f), static_cast<uint8_t>(cartridgeColor.g * 0.8f), static_cast<uint8_t>(cartridgeColor.b * 0.8f)));
            notch.setOutlineColor(sf::Color(30, 30, 30));
            notch.setOutlineThickness(2.0f);
            window.draw(notch);

            // Label area (sticker on cartridge)
            sf::RectangleShape labelArea({doorWidth * 0.85f, doorHeight * 0.7f});
            labelArea.setPosition({doorX + doorWidth * 0.075f, actualDoorY + 60.0f});
            labelArea.setFillColor(sf::Color(240, 240, 230)); // Off-white sticker
            labelArea.setOutlineColor(sf::Color(50, 50, 50));
            labelArea.setOutlineThickness(2.0f);
            window.draw(labelArea);
        }

        // Show text ONLY when hovered (with floating effect)
        if (room->isHovered())
        {
            // Game title
            std::string label;
            if (room->getType() == Room::RoomType::START)
                label = "TUTORIAL";
            else if (room->getType() == Room::RoomType::BOSS)
                label = "BOSS BATTLE";
            else
                label = "NEXT STAGE";

            sf::Text roomTitleText(font, label, 42);
            roomTitleText.setFillColor(sf::Color(20, 20, 20));
            roomTitleText.setStyle(sf::Text::Bold);
            sf::FloatRect titleBounds = roomTitleText.getLocalBounds();

            // Tutorial text moved up 20px more (from +50 to +30)
            float titleY = (room->getType() == Room::RoomType::START) ? actualDoorY + 30.0f : actualDoorY + 70.0f;

            roomTitleText.setPosition(sf::Vector2f(doorX + doorWidth / 2.0f - titleBounds.size.x / 2.0f,
                                                   titleY));
            window.draw(roomTitleText);

            // Description
            std::string desc;
            if (room->getType() == Room::RoomType::START)
                desc = "You are Enginner\nLearn the basics\nof coding!";
            else if (room->getType() == Room::RoomType::BOSS)
                desc = "Face the final\nchallenge!";
            else
                desc = "Progress++\nHere we go!";

            sf::Text descText(font, desc, 24);
            descText.setFillColor(sf::Color(60, 60, 60));
            sf::FloatRect descBounds = descText.getLocalBounds();
            descText.setPosition(sf::Vector2f(doorX + doorWidth / 2.0f - descBounds.size.x / 2.0f,
                                              actualDoorY + doorHeight - 210.0f)); // Moved up from 180
            window.draw(descText);

            // "Click to enter" hint
            sf::Text hintText(font, ">> CLICK TO ENTER <<", 20);
            hintText.setFillColor(sf::Color(200, 0, 0));
            hintText.setStyle(sf::Text::Bold);
            sf::FloatRect hintBounds = hintText.getLocalBounds();
            hintText.setPosition(sf::Vector2f(doorX + doorWidth / 2.0f - hintBounds.size.x / 2.0f,
                                              actualDoorY + doorHeight - 90.0f)); // Moved up from 60
            window.draw(hintText);
        }
        else
        {
            // When NOT hovered - show "???"
            sf::Text minimalistText(font, "???", 72);
            minimalistText.setFillColor(sf::Color(100, 100, 100));
            minimalistText.setStyle(sf::Text::Bold);
            sf::FloatRect minBounds = minimalistText.getLocalBounds();
            minimalistText.setPosition(sf::Vector2f(doorX + doorWidth / 2.0f - minBounds.size.x / 2.0f,
                                                    doorY + doorHeight / 2.0f - minBounds.size.y / 2.0f));
            window.draw(minimalistText);
        }

        return; // Don't show regular doors
    }

    // Draw each door as a large button
    int doorIndex = 0;
    float doorWidth = 450.0f; // Square cartridge
    float doorHeight = 450.0f;
    float spacing = 100.0f;

    // Calculate starting X - moved to the right (removed the -100px left offset)
    float totalWidth = doorCount * doorWidth + (doorCount - 1) * spacing;
    float startX = centerX - totalWidth / 2.0f + 50.0f; // Moved 50px to the right

    // Move doors UP even higher (was -80, now -150px higher than center)
    float doorStartY = centerY - doorHeight / 2.0f - 150.0f;

    while (child)
    {
        Room *room = child->room;

        // Calculate door position
        float doorX = startX + doorIndex * (doorWidth + spacing);
        float doorY = doorStartY;
        // Determine texture key and fallback color
        std::string textureKey;
        sf::Color fallbackColor;
        switch (room->getType())
        {
        case Room::RoomType::START:
            textureKey = "lightblue";
            fallbackColor = sf::Color(50, 150, 200);
            break;
        case Room::RoomType::COMBAT:
            textureKey = "red";
            fallbackColor = sf::Color(200, 50, 50);
            break;
        case Room::RoomType::TREASURE:
            textureKey = "gold";
            fallbackColor = sf::Color(255, 215, 0);
            break;
        case Room::RoomType::REST:
            textureKey = "green";
            fallbackColor = sf::Color(50, 200, 50);
            break;
        case Room::RoomType::SHOP:
            textureKey = "blue";
            fallbackColor = sf::Color(100, 100, 255);
            break;
        case Room::RoomType::BOSS:
            textureKey = "purple";
            fallbackColor = sf::Color(150, 0, 150);
            break;
        case Room::RoomType::EXIT:
            textureKey = "cyan";
            fallbackColor = sf::Color(0, 200, 200);
            break;
        default:
            textureKey = "";
            fallbackColor = sf::Color(100, 100, 100);
        }

        // Apply floating effect when hovered
        float floatY = room->isHovered() ? hoverFloatOffset : 0.0f;
        float actualDoorY = doorY + floatY;

        // Try to use sprite first
        if (texturesLoaded && !textureKey.empty() && cartridgeTextures.find(textureKey) != cartridgeTextures.end())
        {
            // SPRITE RENDERING
            sf::Sprite cartridgeSprite(cartridgeTextures[textureKey]);

            // Scale sprite to fit door size (350x450)
            sf::Vector2u texSize = cartridgeTextures[textureKey].getSize();
            float scaleX = doorWidth / texSize.x;
            float scaleY = doorHeight / texSize.y;
            cartridgeSprite.setScale({scaleX, scaleY});
            cartridgeSprite.setPosition({doorX, actualDoorY});

            // Apply brightness when hovered
            if (room->isHovered())
            {
                cartridgeSprite.setColor(sf::Color(255, 255, 255, 255)); // Full brightness
            }
            else
            {
                cartridgeSprite.setColor(sf::Color(180, 180, 180, 255)); // Slightly darker
            }

            window.draw(cartridgeSprite);
        }
        else
        {
            // FALLBACK: Geometric shapes
            sf::Color cartridgeColor = fallbackColor;

            // Darken when not hovered, brighten when hovered
            if (room->isHovered())
            {
                cartridgeColor.r = static_cast<uint8_t>(std::min(255, cartridgeColor.r + 60));
                cartridgeColor.g = static_cast<uint8_t>(std::min(255, cartridgeColor.g + 60));
                cartridgeColor.b = static_cast<uint8_t>(std::min(255, cartridgeColor.b + 60));
            }
            else
            {
                cartridgeColor.r = static_cast<uint8_t>(cartridgeColor.r * 0.7f);
                cartridgeColor.g = static_cast<uint8_t>(cartridgeColor.g * 0.7f);
                cartridgeColor.b = static_cast<uint8_t>(cartridgeColor.b * 0.7f);
            }

            // Main cartridge body
            sf::RectangleShape cartridgeBody({doorWidth, doorHeight});
            cartridgeBody.setPosition({doorX, actualDoorY});
            cartridgeBody.setFillColor(cartridgeColor);

            cartridgeBody.setOutlineColor(sf::Color(30, 30, 30));
            cartridgeBody.setOutlineThickness(4.0f);

            window.draw(cartridgeBody);

            // Cartridge notch at top (pull tab)
            sf::RectangleShape notch({doorWidth * 0.6f, 30.0f});
            notch.setPosition({doorX + doorWidth * 0.2f, actualDoorY - 20.0f});
            notch.setFillColor(sf::Color(static_cast<uint8_t>(cartridgeColor.r * 0.8f), static_cast<uint8_t>(cartridgeColor.g * 0.8f), static_cast<uint8_t>(cartridgeColor.b * 0.8f)));
            notch.setOutlineColor(sf::Color(30, 30, 30));
            notch.setOutlineThickness(2.0f);
            window.draw(notch);

            // Label area (sticker on cartridge)
            sf::RectangleShape labelArea({doorWidth * 0.85f, doorHeight * 0.7f});
            labelArea.setPosition({doorX + doorWidth * 0.075f, actualDoorY + 60.0f});
            labelArea.setFillColor(sf::Color(240, 240, 230)); // Off-white sticker
            labelArea.setOutlineColor(sf::Color(50, 50, 50));
            labelArea.setOutlineThickness(2.0f);
            window.draw(labelArea);
        }

        // Show text ONLY when hovered (with floating effect)
        if (room->isHovered())
        {
            // Room type label
            sf::Text roomLabel(font, room->getName(), 36);
            roomLabel.setFillColor(sf::Color(20, 20, 20));
            roomLabel.setStyle(sf::Text::Bold);
            sf::FloatRect labelBounds = roomLabel.getLocalBounds();
            roomLabel.setPosition(sf::Vector2f(doorX + doorWidth / 2.0f - labelBounds.size.x / 2.0f,
                                               actualDoorY + 70.0f)); // Moved up from 100
            window.draw(roomLabel);

            // Description text - programmer themed
            std::string desc;
            switch (room->getType())
            {
            case Room::RoomType::COMBAT:
                desc = "Fight bugs\n& errors!";
                break;
            case Room::RoomType::TREASURE:
                desc = "Collect\npower-ups!";
                break;
            case Room::RoomType::REST:
                desc = "Coffee break\ntime!";
                break;
            case Room::RoomType::SHOP:
                desc = "Upgrade\nyour IDE!";
                break;
            default:
                desc = "Click\nto enter";
            }

            sf::Text descText(font, desc, 24);
            descText.setFillColor(sf::Color(60, 60, 60));
            sf::FloatRect descBounds = descText.getLocalBounds();
            descText.setPosition(sf::Vector2f(doorX + doorWidth / 2.0f - descBounds.size.x / 2.0f,
                                              actualDoorY + doorHeight - 210.0f)); // Moved up from 180
            window.draw(descText);

            // "Click to enter" hint
            sf::Text hintText(font, ">> SELECT <<", 20);
            hintText.setFillColor(sf::Color(200, 0, 0));
            hintText.setStyle(sf::Text::Bold);
            sf::FloatRect hintBounds = hintText.getLocalBounds();
            hintText.setPosition(sf::Vector2f(doorX + doorWidth / 2.0f - hintBounds.size.x / 2.0f,
                                              actualDoorY + doorHeight - 90.0f)); // Moved up from 60
            window.draw(hintText);
        }
        else
        {
            // When NOT hovered - show "???" (no floating)
            sf::Text minimalistText(font, "???", 72);
            minimalistText.setFillColor(sf::Color(100, 100, 100));
            minimalistText.setStyle(sf::Text::Bold);
            sf::FloatRect minBounds = minimalistText.getLocalBounds();
            minimalistText.setPosition(sf::Vector2f(doorX + doorWidth / 2.0f - minBounds.size.x / 2.0f,
                                                    doorY + doorHeight / 2.0f - minBounds.size.y / 2.0f));
            window.draw(minimalistText);
        }

        child = child->next;
        doorIndex++;
    }
}

// Update hover states based on mouse position
void Map::updateHoverState(sf::Vector2f mousePos, sf::Vector2u windowSize)
{
    // Reset all hover states first
    RoomContainer *current = allRoomsHead;
    while (current)
    {
        current->room->setHovered(false);
        current = current->next;
    }

    // Check if mouse is over any door
    if (!currentRoom)
        return;

    // Calculate door positions (EXACT same logic as render)
    RoomLink *child = currentRoom->getChildrenHead();
    int doorCount = 0;
    RoomLink *temp = child;
    while (temp)
    {
        doorCount++;
        temp = temp->next;
    }

    // Use ACTUAL window dimensions
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    // Special case: Single tutorial/boss/exit room (centered, same size as regular rooms)
    if (doorCount == 1 && (child->room->getType() == Room::RoomType::START ||
                           child->room->getType() == Room::RoomType::BOSS ||
                           child->room->getType() == Room::RoomType::EXIT))
    {
        float doorWidth = 350.0f;  // Same as regular rooms
        float doorHeight = 450.0f; // Same as regular rooms
        float doorX = centerX - doorWidth / 2.0f;
        float doorY = centerY - doorHeight / 2.0f - 150.0f; // Same offset as regular rooms

        if (mousePos.x >= doorX && mousePos.x <= doorX + doorWidth &&
            mousePos.y >= doorY && mousePos.y <= doorY + doorHeight)
        {
            child->room->setHovered(true);
        }
        return;
    }

    int doorIndex = 0;
    float doorWidth = 450.0f; // Square cartridge
    float doorHeight = 450.0f;
    float spacing = 100.0f;

    float totalWidth = doorCount * doorWidth + (doorCount - 1) * spacing;
    float startX = centerX - totalWidth / 2.0f + 50.0f;      // Moved 50px right to match rendering
    float doorStartY = centerY - doorHeight / 2.0f - 150.0f; // Same offset as render

    child = currentRoom->getChildrenHead();
    while (child)
    {
        float doorX = startX + doorIndex * (doorWidth + spacing);
        float doorY = doorStartY;

        // Check if mouse is inside door rectangle
        if (mousePos.x >= doorX && mousePos.x <= doorX + doorWidth &&
            mousePos.y >= doorY && mousePos.y <= doorY + doorHeight)
        {
            child->room->setHovered(true);
            break;
        }

        child = child->next;
        doorIndex++;
    }
}

// Debug: Print map tree structure to console
void Map::printMapTree()
{
    std::cout << "[Map] Tree structure:" << std::endl;
    printRoomTree(root, 0);
}

// Recursive helper to print room tree
void Map::printRoomTree(Room *room, int depth)
{
    if (!room)
        return;

    // Print indentation
    for (int i = 0; i < depth; i++)
    {
        std::cout << "  ";
    }
    std::cout << "- " << room->getName() << " (ID: " << room->getID() << ")" << std::endl;

    // Print children
    RoomLink *current = room->getChildrenHead();
    while (current)
    {
        printRoomTree(current->room, depth + 1);
        current = current->next;
    }
}

// Find which room is at given position (for mouse clicks)
Room *Map::findRoomAt(sf::Vector2f position, sf::Vector2u windowSize)
{
    if (!currentRoom)
        return static_cast<Room *>(0);

    // Calculate door positions (EXACT same as render and updateHoverState)
    RoomLink *child = currentRoom->getChildrenHead();
    int doorCount = 0;
    RoomLink *temp = child;
    while (temp)
    {
        doorCount++;
        temp = temp->next;
    }

    // Use ACTUAL window dimensions
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    // Special case: Single tutorial/boss/exit room (centered, same size as regular rooms)
    if (doorCount == 1 && (child->room->getType() == Room::RoomType::START ||
                           child->room->getType() == Room::RoomType::BOSS ||
                           child->room->getType() == Room::RoomType::EXIT))
    {
        float doorWidth = 450.0f;                           // Square cartridge
        float doorHeight = 450.0f;                          // Square cartridge
        float doorX = centerX - doorWidth / 2.0f + 100.0f;  // Moved 100px right to match rendering
        float doorY = centerY - doorHeight / 2.0f - 150.0f; // Same offset as regular rooms

        if (position.x >= doorX && position.x <= doorX + doorWidth &&
            position.y >= doorY && position.y <= doorY + doorHeight)
        {
            return child->room;
        }
        return static_cast<Room *>(0);
    }

    int doorIndex = 0;
    float doorWidth = 450.0f; // Square cartridge
    float doorHeight = 450.0f;
    float spacing = 100.0f;

    float totalWidth = doorCount * doorWidth + (doorCount - 1) * spacing;
    float startX = centerX - totalWidth / 2.0f + 50.0f;      // Moved 50px right to match rendering
    float doorStartY = centerY - doorHeight / 2.0f - 150.0f; // Same offset as render

    child = currentRoom->getChildrenHead();
    while (child)
    {
        float doorX = startX + doorIndex * (doorWidth + spacing);
        float doorY = doorStartY;

        // Check if position is inside door rectangle
        if (position.x >= doorX && position.x <= doorX + doorWidth &&
            position.y >= doorY && position.y <= doorY + doorHeight)
        {
            return child->room;
        }

        child = child->next;
        doorIndex++;
    }

    return static_cast<Room *>(0);
}

// Render loading screen with animation
void Map::renderLoadingScreen(sf::RenderWindow &window, sf::Font &font, const std::string & /*message*/)
{
    // Process events to keep window responsive
    while (const std::optional event = window.pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            window.close();
        }
    }

    window.clear(sf::Color(20, 20, 30)); // Dark background

    sf::Vector2u windowSize = window.getSize();
    float centerX = windowSize.x / 2.0f;
    float centerY = windowSize.y / 2.0f;

    // Loading text
    sf::Text loadingText(font, "Generating Map...", 48);
    loadingText.setFillColor(sf::Color::White);
    sf::FloatRect textBounds = loadingText.getLocalBounds();
    loadingText.setPosition(sf::Vector2f(centerX - textBounds.size.x / 2.0f, centerY - 100.0f));
    window.draw(loadingText);

    // Rotating circle animation with dots
    float rotation = (animFrameCounter * 0.1f);
    while (rotation > 6.28318f) // Keep in 0 to 2*PI range
        rotation -= 6.28318f;

    // Draw 8 rotating dots in circle
    for (int i = 0; i < 8; i++)
    {
        float angle = (i * 45.0f) * 3.14159f / 180.0f + rotation;
        float radius = 60.0f;
        float dotX = centerX + std::cos(angle) * radius;
        float dotY = centerY + 50.0f + std::sin(angle) * radius;

        sf::CircleShape dot(8.0f);
        uint8_t alpha = static_cast<uint8_t>(255 - (i * 25));
        dot.setFillColor(sf::Color(100, 200, 255, alpha));
        dot.setPosition(sf::Vector2f(dotX - 8.0f, dotY - 8.0f));
        window.draw(dot);
    }

    window.display();
}

// Load cartridge textures from files
void Map::loadCartridgeTextures()
{
    std::cout << "[Map] Loading cartridge textures..." << std::endl;

    struct TextureInfo
    {
        std::string key;
        std::string path;
        sf::IntRect rect;
    };

    // Light blue uses custom rect (119, 96, 262, 309), others use full (0, 0, 500, 500)
    std::vector<TextureInfo> textures = {
        {"lightblue", "asset/room/cartridge_lightblue.png", sf::IntRect({119, 96}, {262, 309})},
        {"red", "asset/room/cartridge_red.png", sf::IntRect({0, 0}, {500, 500})},
        {"gold", "asset/room/cartridge_gold.png", sf::IntRect({0, 0}, {500, 500})},
        {"green", "asset/room/cartridge_green.png", sf::IntRect({0, 0}, {500, 500})},
        {"blue", "asset/room/cartridge_blue.png", sf::IntRect({0, 0}, {500, 500})},
        {"purple", "asset/room/cartridge_purple.png", sf::IntRect({0, 0}, {500, 500})},
        {"cyan", "asset/room/cartridge_cyan.png", sf::IntRect({0, 0}, {500, 500})}};

    texturesLoaded = true;
    for (const auto &texInfo : textures)
    {
        sf::Texture texture;
        if (texture.loadFromFile(texInfo.path, false, texInfo.rect))
        {
            cartridgeTextures[texInfo.key] = texture;
            std::cout << "[Map] Loaded: " << texInfo.path << std::endl;
        }
        else
        {
            std::cout << "[Map] WARNING: Failed to load " << texInfo.path << std::endl;
            texturesLoaded = false;
        }
    }

    if (texturesLoaded)
    {
        std::cout << "[Map] All cartridge textures loaded successfully!" << std::endl;
    }
    else
    {
        std::cout << "[Map] Some textures failed to load - using fallback shapes" << std::endl;
    }
}

// Update map state
void Map::update(float deltaTime)
{
    // Smooth sine wave for floating effect (cycle every 2 seconds)
    static float floatTime = 0.0f;
    floatTime += deltaTime;
    hoverFloatOffset = std::sin(floatTime * 3.14159f) * 15.0f; // Float up/down 15 pixels
}

// Get the total room count
int Map::getTotalRoomCount() const
{
    return totalRoomCount;
}
// Get the current stage
int Map::getCurrentStage() const
{
    return currentStage;
}
// Check if map is generating
bool Map::isGenerating() const
{
    return generating;
}
