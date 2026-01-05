#pragma warning(push)
#pragma warning(disable : 4100)

#include "FullStackDev.h"
#include <iostream>

// Constructor [Implementation]
FullStackDev::FullStackDev(const std::string &name) : Player(name, "FULL STACK DEV", 90, 4)
{

    std::cout << "[FullStackDev] Created: " << name << std::endl;
    if (loadSprite("asset/character/Fullstackdeveloper.png"))
    {
        std::cout << "[FullStackDev] Sprite loaded successfully" << std::endl;
        setupFrames();
    }
    else
    {
        std::cout << "[FullStackDev] Warning: Could not load sprite" << std::endl;
    }

    setPosition(100.0f, 400.0f);
}

// Destructor [Implementation]
FullStackDev::~FullStackDev()
{
    std::cout << "[FullStackDev] Destroyed" << std::endl;
}

// Update  [Implementation]
void FullStackDev::update(float deltaTime)
{
}

// Setup animation frames  [Implementation]
void FullStackDev::setupFrames()
{

    setFrameRect(0, 0, 0, 115, 175);   // Frame 0
    setFrameRect(1, 130, 0, 115, 175); // Frame 1
    setFrameRect(2, 260, 0, 115, 175); // Frame 2
    setFrameRect(3, 380, 0, 115, 175); // Frame 3
}
