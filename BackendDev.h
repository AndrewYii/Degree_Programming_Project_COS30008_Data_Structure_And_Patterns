#pragma once

#include "Player.h"

class BackendDev : public Player {
public:
    // Constructor
    BackendDev(const std::string& name);
    // Destructor
    ~BackendDev() override;
    // Override update function
    void update(float deltaTime) override;
private:
    // Setup animation frames
    void setupFrames();
};
