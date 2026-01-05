#pragma once

#include "Player.h"

class FrontendDev : public Player
{
public:
    // Constructor [Header]
    FrontendDev(const std::string &name);
    // Destructor [Header]
    ~FrontendDev() override;
    // Override update function
    void update(float deltaTime) override;

private:
    // Setup animation frames
    void setupFrames();
};
