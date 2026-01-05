#pragma once

#include "Player.h"

class DataScientist : public Player {
public:
    // Constructor [Header] 
    DataScientist(const std::string& name);
    // Destructor [Header]
    ~DataScientist() override;
    // Update function [Header]
    void update(float deltaTime) override;

private:
    // Frame setup function [Header]
    void setupFrames();
};
