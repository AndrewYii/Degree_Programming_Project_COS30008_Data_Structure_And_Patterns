#pragma once

#include "Screen.h"
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

class SettingsScreen : public Screen {
private:
  int selectedElement;  
    bool mousePressed;
    bool draggingSlider;      
    bool draggingSoundSlider; 
    bool dropdownOpen;
    int selectedResolution;
    float volumePercent; 
    float soundVolumePercent;  
    struct Resolution {
        unsigned int width;
        unsigned int height;
        std::string label;
    };
    std::vector<Resolution> resolutions;
    
public:
    SettingsScreen(Game* gameInstance);
    ~SettingsScreen() override;
    void handleEvents() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window, sf::Font& font) override;
    void onEnter() override;
    void onExit() override;
    
private:
    void applyResolution(unsigned int width, unsigned int height);
    void applyVolume(float volume);
    void applySoundVolume(float volume); 
};
