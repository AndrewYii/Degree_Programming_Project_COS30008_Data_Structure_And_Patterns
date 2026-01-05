#include "DevOpsEngineer.h"
#include <iostream>

// Constructor [Implementation]
DevOpsEngineer::DevOpsEngineer(const std::string &name) : Player(name, "DEVOPS ENGINEER", 85, 3)
{

    std::cout << "[DevOpsEngineer] Created: " << name << std::endl;

    // Load DevOps Engineer sprite
    if (loadSprite("asset/character/DevOsDeveloper.png"))
    {
        std::cout << "[DevOpsEngineer] Sprite loaded successfully" << std::endl;
        setupFrames();
    }
    else
    {
        std::cout << "[DevOpsEngineer] Warning: Could not load sprite" << std::endl;
    }

    setPosition(100.0f, 400.0f);
}
// Destructor [Implementation]
DevOpsEngineer::~DevOpsEngineer()
{
    std::cout << "[DevOpsEngineer] Destroyed" << std::endl;
}

void DevOpsEngineer::update(float)
{
}

void DevOpsEngineer::setupFrames()
{
    setFrameRect(0, 0, 0, 120, 192);   // Frame 0
    setFrameRect(1, 140, 0, 90, 192);  // Frame 1
    setFrameRect(2, 258, 0, 106, 192); // Frame 2
    setFrameRect(3, 368, 0, 108, 192); // Frame 3
}
