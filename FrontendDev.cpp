#include "FrontendDev.h"
#include <iostream>

// Constructor [Implementation]
FrontendDev::FrontendDev(const std::string &name) : Player(name, "FRONTEND DEV", 80, 3)
{

    std::cout << "[FrontendDev] Created: " << name << std::endl;
    if (loadSprite("asset/character/Frontenddeveloper.png"))
    {
        std::cout << "[FrontendDev] Sprite loaded successfully" << std::endl;
        setupFrames(); // Setup frontend-specific frame coordinates
    }
    else
    {
        std::cout << "[FrontendDev] Warning: Could not load sprite" << std::endl;
    }
    setPosition(100.0f, 400.0f);
}

// Destructor [Implementation]
FrontendDev::~FrontendDev()
{
    std::cout << "[FrontendDev] Destroyed" << std::endl;
}

void FrontendDev::update(float)
{
}

void FrontendDev::setupFrames()
{
    setFrameRect(0, 0, 0, 122, 185);   // Frame 0: x=0, y=0, w=122, h=185
    setFrameRect(1, 132, 0, 118, 185); // Frame 1: x=132, y=0, w=118, h=185
    setFrameRect(2, 258, 0, 110, 185); // Frame 2: x=258, y=0, w=110, h=185
    setFrameRect(3, 370, 0, 114, 185); // Frame 3: x=370, y=0, w=114, h=185
}
