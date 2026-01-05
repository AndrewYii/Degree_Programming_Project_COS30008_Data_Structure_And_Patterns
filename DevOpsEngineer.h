#pragma once

#include "Player.h"

class DevOpsEngineer : public Player {
public:
    // Constructor [Header]
    DevOpsEngineer(const std::string& name);
    // Destructor [Header]
    ~DevOpsEngineer() override;
    // Override update function [Header]
    void update(float deltaTime) override;
private:
    // Setup animation frames specific to DevOps Engineer [Header]
    void setupFrames();
};
