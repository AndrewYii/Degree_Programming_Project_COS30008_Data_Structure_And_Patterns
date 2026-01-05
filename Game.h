#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Screen.h"
#include "Player.h"
#include "FrontendDev.h"
#include "BackendDev.h"
#include "FullStackDev.h"
#include "DevOpsEngineer.h"
#include "DataScientist.h"
#include "MainMenuScreen.h"
#include "RoleSelectionScreen.h"
#include "StoryIntroScreen.h"
#include "SettingsScreen.h"
#include "GameplayScreen.h"
#include "CombatScreen.h"
#include "VictoryScreen.h"
#include "DeathScreen.h"
#include "TreasureScreen.h"
#include "RestScreen.h"
#include "ShopScreen.h"
#include "TutorialScreen.h"
#include "CreditsScreen.h"
#include "Entity.h"

class Game
{
public:
    // Screen types enum
    enum class ScreenType
    {
        MAIN_MENU,
        ROLE_SELECTION,
        SETTINGS,
        GAMEPLAY,
        STORY_INTRO,
        TUTORIAL,
        COMBAT,
        VICTORY,
        TREASURE,
        REST,
        SHOP,
        DEATH,
        CREDITS
    };
    static const sf::Color COLOR_OFF_WHITE;
    static const sf::Color COLOR_BLACK;

private:
    Game();
    Game(const Game &) = delete;
    Game &operator=(const Game &) = delete;
    // Static instance pointer
    static Game *instance;
    // SFML components
    sf::RenderWindow *window;
    sf::Font font;
    sf::Music backgroundMusic;
    sf::Clock clock;
    // Screen management
    Screen *currentScreen;
    Screen *previousScreen;
    // Array to store all screen instances
    Screen *screens[13];
    Player *player;
    bool running;
    float deltaTime;
    float musicVolume;
    float soundVolume;
    unsigned int windowWidth;
    unsigned int windowHeight;
    static const char *WINDOW_TITLE;

public:
    // Singleton access methods [Header]
    static Game *getInstance();
    // Destroy singleton instance [Header]
    static void destroy();
    // Destructor [Header]
    ~Game();
    // Initialise game [Header]
    void initialise();
    // Main game loop [Header]
    void run();
    // Shutdown game [Header]
    void shutdown();
    // Change screen [Header]
    void changeScreen(ScreenType newScreen);
    // Go back to previous screen [Header]
    void goBackToPreviousScreen();
    // Set Resolution and apply immediately [Header]
    void setResolution(unsigned int width, unsigned int height);
    // Set music volume and apply immediately [Header]
    void setVolume(float volume);
    // Set sound effects volume and apply immediately [Header]
    void setSoundVolume(float volume);
    // Get current volume levels [Header]
    float getVolume() const;
    // Get current sound effects volume level [Header]
    float getSoundVolume() const;
    // Quit game [Header]
    void quit();
    // Get window [Header]
    sf::RenderWindow *getWindow();
    // Get font [Header]
    sf::Font &getFont();
    // Get game state info [Header]
    bool isRunning() const;
    // Get delta time [Header]
    float getDeltaTime() const;
    // Get specific screens [Header]
    Screen *getStoryIntroScreen();
    // Get player reference [Header]
    Player &getPlayer();
    // Set player reference [Header]
    void setPlayer(Player *newPlayer);

private:
    // Load game assets [Header]
    bool loadAssets();
    // Create all screen objects [Header]
    void createScreens();
    // Destroy all screen objects [Header]
    void destroyScreens();
};
