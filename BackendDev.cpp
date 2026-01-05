#include "BackendDev.h"
#include <iostream>

// Constructor [Implementation]
BackendDev::BackendDev(const std::string &name) : Player(name, "BACKEND DEV", 100, 3)
{

    std::cout << "[BackendDev] Created: " << name << std::endl;
    if (loadSprite("asset/character/BackendDeveloper.png"))
    {
        std::cout << "[BackendDev] Sprite loaded successfully" << std::endl;
        setupFrames();
    }
    else
    {
        std::cout << "[BackendDev] Warning: Could not load sprite" << std::endl;
    }
    setPosition(100.0f, 400.0f);
}

// Destructor [Implementation]
BackendDev::~BackendDev()
{
    std::cout << "[BackendDev] Destroyed" << std::endl;
}

void BackendDev::update(float)
{
}

void BackendDev::setupFrames()
{
    setFrameRect(0, 0, 0, 94, 186);
    setFrameRect(1, 148, 0, 85, 186);
    setFrameRect(2, 255, 0, 104, 186);
    setFrameRect(3, 396, 0, 80, 186);
}
