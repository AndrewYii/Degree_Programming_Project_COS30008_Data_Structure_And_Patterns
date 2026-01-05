#pragma once

#include "Player.h"

class FullStackDev : public Player {
public:
    // Constructor [Header]
    FullStackDev(const std::string& name);
    // Destructor [Header]
    ~FullStackDev() override;
    // Update  [Header]
    void update(float deltaTime) override;

private:
    // Setup animation frames  [Header]
    void setupFrames();
};
