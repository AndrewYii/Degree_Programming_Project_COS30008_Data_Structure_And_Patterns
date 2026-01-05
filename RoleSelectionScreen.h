#pragma once

#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <string>

class RoleSelectionScreen : public Screen {
public:

    struct ProgrammerRole {
      std::string name;
      std::string descriptionLine1;
      std::string descriptionLine2;
      std::string humorQuote;
      std::string specialBonus;
      int startingHealth;
      int startingEnergy;
      std::string spriteSheetPath;
    };
    struct SpriteFrame {
      int x, y, width, height;
    };
    struct RoleSpriteSheet {
      sf::Texture texture;
      SpriteFrame frames[4];
      bool loaded;
      RoleSpriteSheet() : loaded(false) {}
    };
    struct RoleNode {
      ProgrammerRole data;
      RoleNode* prev;
      RoleNode* next;
      RoleNode(const ProgrammerRole& role) : data(role), prev(nullptr), next(nullptr) {}
    };

private:
    RoleNode* head;
    RoleNode* currentMiddle;
    int totalRoles;
    int selectedRoleIndex;
    int hoveredElement;  
    bool mousePressed;
    RoleSpriteSheet spriteSheets[5];
    int currentFrame[3];
    float animationTimer[3];
    float frameTime;
    std::string playerName;
    bool inputActive;
    float cursorBlinkTimer;
    bool cursorVisible;
    
public:
    // Constructor [Header]
    RoleSelectionScreen(Game* gameInstance);
    // Destructor [Header]
    ~RoleSelectionScreen() override;
    // Handle events [Header]
    void handleEvents() override;
    // Update logic [Header]
    void update(float deltaTime) override;
    // Render screen [Header]
    void render(sf::RenderWindow& window, sf::Font& font) override;
    // Navigate to this screen [Header]
    void onEnter() override;
    // Exit from this screen [Header]
    void onExit() override;
    // Get player name [Header]
    std::string getPlayerName() const;
    
private: 
    // Build the circular doubly linked list of roles [Header]
    void buildCircularList();
    // Destroy the circular doubly linked list [Header]
    void destroyCircularList();
    // Rotate the list to the left (previous role) [Header]
    void rotateLeft();
    // Rotate the list to the right (next role) [Header]
    void rotateRight();
    // Load sprite sheets for roles [Header]
    void loadSpriteSheets();
    // Get role index from node [Header]
    int getRoleIndex(RoleNode* node);
    // Draw a role card [Header]
    void drawRoleCard(sf::RenderWindow& window, sf::Font& font,const ProgrammerRole& role, int roleIndex,float cardX, float cardY, float cardWidth, float cardHeight,bool isMiddle, bool isHovered, int cardPosition);
    // Draw gear icon [Header]
    void drawGearIcon(sf::RenderWindow& window, float x, float y, float radius, sf::Color color);
    // Draw arrow [Header]
    void drawArrow(sf::RenderWindow& window, float x, float y, float size, bool pointsLeft, sf::Color color);
};
