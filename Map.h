#pragma once

#include "Room.h"
#include <SFML/Graphics.hpp>
#include <queue>
#include <map>

struct RoomContainer
{
  Room *room;
  RoomContainer *next;

  RoomContainer(Room *r) : room(r), next(static_cast<RoomContainer *>(0)) {}
};

class Map
{
private:
  Room *root;
  Room *currentRoom;
  RoomContainer *allRoomsHead;
  int totalRoomCount;
  int nextRoomID;
  int currentStage;
  int animFrameCounter;
  bool generating;
  std::map<std::string, sf::Texture> cartridgeTextures;
  bool texturesLoaded;
  float hoverFloatOffset;

public:
  // Constructor
  Map();
  // Destructor
  ~Map();

  // Generation
  void generateMap(int depth, int branching, float mapWidth, float mapHeight, sf::RenderWindow *window = nullptr, sf::Font *font = nullptr);
  void regenerateForNextStage(float mapWidth, float mapHeight, sf::RenderWindow *window = nullptr, sf::Font *font = nullptr); // Generate new map for next stage
  void clear();

  // Navigation
  Room *getCurrentRoom() { return currentRoom; }
  void moveToRoom(Room *room);
  bool canMoveToRoom(Room *room);

  // Tree traversal
  void printMapTree();

  // Rendering
  void render(sf::RenderWindow &window, sf::Font &font);
  void renderLoadingScreen(sf::RenderWindow &window, sf::Font &font, const std::string &message);
  void update(float deltaTime); // Update floating animation

  // Room lookup
  Room *findRoomAt(sf::Vector2f position, sf::Vector2u windowSize);
  void updateHoverState(sf::Vector2f mousePos, sf::Vector2u windowSize);

  // Getters
  int getTotalRoomCount() const;
  int getCurrentStage() const;
  bool isGenerating() const;

private:
  // Helper functions
  void generateBranch(Room *parentRoom, int currentDepth, int maxDepth, int branching, float x, float y, float horizontalSpacing, float verticalSpread, int &combatCount, int &treasureCount, int &restCount, int &shopCount, sf::RenderWindow *window, sf::Font *font);
  void printRoomTree(Room *room, int depth);
  void addRoomToContainer(Room *room);
  void loadCartridgeTextures();
};
