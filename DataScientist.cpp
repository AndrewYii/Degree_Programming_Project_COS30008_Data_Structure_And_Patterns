#include "DataScientist.h"
#include <iostream>

// Constructor [Implementation]
DataScientist::DataScientist(const std::string &name) : Player(name, "DATA SCIENTIST", 75, 4)
{
    std::cout << "[DataScientist] Created: " << name << std::endl;

    // Load Data Scientist sprite
    if (loadSprite("asset/character/Datascientist.png"))
    {
        std::cout << "[DataScientist] Sprite loaded successfully" << std::endl;
        setupFrames();
    }
    else
    {
        std::cout << "[DataScientist] Warning: Could not load sprite" << std::endl;
    }

    setPosition(100.0f, 400.0f);
}

// Destructor [Implementation]
DataScientist::~DataScientist()
{
    std::cout << "[DataScientist] Destroyed" << std::endl;
}

void DataScientist::update(float)
{
}

void DataScientist::setupFrames()
{
    setFrameRect(0, 30, 0, 104, 164);  // Frame 0
    setFrameRect(1, 132, 0, 107, 164); // Frame 1
    setFrameRect(2, 240, 0, 119, 164); // Frame 2
    setFrameRect(3, 374, 0, 115, 164); // Frame 3
}
