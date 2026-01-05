#include "Game.h"
#include "Entity.h"
#include "Screen.h"
#include "Player.h"
#include "MainMenuScreen.h"
#include "RoleSelectionScreen.h"
#include "StoryIntroScreen.h"
#include "SettingsScreen.h"
#include "GameplayScreen.h"
#include "CardFactory.h"
#include "AIStoryteller.h"
#include "Logger.h"
#include <iostream>

// Create static instance pointer
Game *Game::instance = static_cast<Game *>(0);

const char *Game::WINDOW_TITLE = "Life of Programmer";
const sf::Color Game::COLOR_OFF_WHITE(232, 232, 208);
const sf::Color Game::COLOR_BLACK(44, 44, 44);

// Constructor [Implementation]
Game::Game() : window(static_cast<sf::RenderWindow *>(0)), currentScreen(static_cast<Screen *>(0)), previousScreen(static_cast<Screen *>(0)), player(nullptr), running(false), deltaTime(0.0f), musicVolume(100.0f), soundVolume(100.0f), windowWidth(1024), windowHeight(768)
{
    // Initialize screen array to null
    for (int i = 0; i < 13; i++)
    {
        screens[i] = static_cast<Screen *>(0);
    }

    std::cout << "========================================" << std::endl;
    std::cout << "  LIFE OF PROGRAMMER" << std::endl;
    std::cout << "  Roguelike Deckbuilder" << std::endl;
    std::cout << "  Data Structures & Design Patterns Demo" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "[Game] Singleton instance created" << std::endl;
}

// Destructor [Implementation]
Game::~Game()
{
    std::cout << "[Game] Singleton destroyed" << std::endl;
    delete player;
}

// Get singleton instance [Implementation]
Game *Game::getInstance()
{
    if (instance == static_cast<Game *>(0))
    {
        instance = new Game();
        std::cout << "[Game] First getInstance() call - creating singleton" << std::endl;
    }
    return instance;
}

// Destroy singleton instance [Implementation]
void Game::destroy()
{
    if (instance != static_cast<Game *>(0))
    {
        delete instance;
        instance = static_cast<Game *>(0);
        std::cout << "[Game] Singleton instance destroyed" << std::endl;
    }
}

// Initialize game [Implementation]
void Game::initialise()
{
    std::cout << "\n[Game] Initialising..." << std::endl;

    // Initialize Logger system first (creates logs folder)
    Logger::getInstance()->initialize("logs");

    // Create SFML window
    window = new sf::RenderWindow(
        sf::VideoMode({windowWidth, windowHeight}),
        WINDOW_TITLE,
        sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(60);

    std::cout << "[Game] Window created: " << windowWidth << "x" << windowHeight << std::endl;

    // Load assets
    if (!loadAssets())
    {
        std::cerr << "[Game] ERROR: Failed to load assets!" << std::endl;
        running = false;
        return;
    }

    // Initialize CardFactory database
    std::cout << "[Game] Initializing card database..." << std::endl;
    CardFactory::getInstance()->initializeCardDatabase();

    // Start background music
    backgroundMusic.setLooping(true);
    backgroundMusic.setVolume(musicVolume);
    backgroundMusic.play();
    std::cout << "[Game] Background music started at " << musicVolume << "% volume" << std::endl;

    // Create all screens
    createScreens();

    // Start with main menu
    changeScreen(ScreenType::MAIN_MENU);

    running = true;
    std::cout << "[Game] Initialisation complete!\n"
              << std::endl;
}

// Load game assets [Implementation]
bool Game::loadAssets()
{
    std::cout << "[Game] Loading assets..." << std::endl;

    if (!font.openFromFile("font/8bit.otf"))
    {
        std::cerr << "[Game] ERROR: Failed to load font/8bit.otf" << std::endl;
        return false;
    }
    std::cout << "[Game] Font loaded" << std::endl;

    if (!backgroundMusic.openFromFile("audio/backgroundmusic.mp3"))
    {
        std::cerr << "[Game] ERROR: Failed to load audio/backgroundmusic.mp3" << std::endl;
        return false;
    }
    std::cout << "[Game] Music loaded" << std::endl;

    return true;
}

// Create all screen objects [Implementation]
void Game::createScreens()
{
    std::cout << "[Game] Creating screens..." << std::endl;

    // Create screens and store in array using enum index
    // Uses POLYMORPHISM - all stored as Screen* pointers
    screens[static_cast<int>(ScreenType::MAIN_MENU)] = new MainMenuScreen(this);
    screens[static_cast<int>(ScreenType::ROLE_SELECTION)] = new RoleSelectionScreen(this);
    screens[static_cast<int>(ScreenType::STORY_INTRO)] = new StoryIntroScreen(this);
    screens[static_cast<int>(ScreenType::SETTINGS)] = new SettingsScreen(this);
    screens[static_cast<int>(ScreenType::GAMEPLAY)] = new GameplayScreen(this);
    screens[static_cast<int>(ScreenType::TUTORIAL)] = new TutorialScreen(this);
    screens[static_cast<int>(ScreenType::COMBAT)] = new CombatScreen(this);
    screens[static_cast<int>(ScreenType::VICTORY)] = new VictoryScreen(this);
    screens[static_cast<int>(ScreenType::DEATH)] = new DeathScreen(this);
    screens[static_cast<int>(ScreenType::TREASURE)] = new TreasureScreen(this);
    screens[static_cast<int>(ScreenType::REST)] = new RestScreen(this);
    screens[static_cast<int>(ScreenType::SHOP)] = new ShopScreen(this);
    screens[static_cast<int>(ScreenType::CREDITS)] = new CreditsScreen(this);

    std::cout << "[Game] All screens created (stored polymorphically as Screen*)" << std::endl;
}

// `Destroy all screen objects [Implementation]
void Game::destroyScreens()
{
    std::cout << "[Game] Destroying screens..." << std::endl;

    // Delete all screens from array (polymorphic cleanup)
    for (int i = 0; i < 13; i++)
    {
        delete screens[i];
        screens[i] = static_cast<Screen *>(0);
    }

    currentScreen = static_cast<Screen *>(0);
    previousScreen = static_cast<Screen *>(0);

    std::cout << "[Game] All screens destroyed" << std::endl;
}

// Change screen [Implementation]
void Game::changeScreen(ScreenType newScreen)
{
    // Exit current screen
    if (currentScreen)
    {
        currentScreen->onExit();
        previousScreen = currentScreen;
    }

    // Get screen from array using polymorphism
    int screenIndex = static_cast<int>(newScreen);
    currentScreen = screens[screenIndex];

    // Log screen change
    const char *screenNames[] = {
        "MAIN_MENU", "ROLE_SELECTION", "SETTINGS", "GAMEPLAY",
        "STORY_INTRO", "TUTORIAL", "COMBAT", "VICTORY",
        "TREASURE", "REST", "SHOP", "DEATH", "CREDITS"};
    std::cout << "[Game] Switching to " << screenNames[screenIndex] << " screen" << std::endl;

    if (currentScreen)
    {
        currentScreen->onEnter();
    }
}

// Go back to previous screen [Implementation]
void Game::goBackToPreviousScreen()
{
    if (!previousScreen)
    {
        std::cout << "[Game] No previous screen, going to MAIN_MENU" << std::endl;
        changeScreen(ScreenType::MAIN_MENU);
        return;
    }

    std::cout << "[Game] Going back to previous screen" << std::endl;

    // Exit current
    if (currentScreen)
    {
        currentScreen->onExit();
    }

    // Swap current and previous
    Screen *temp = currentScreen;
    currentScreen = previousScreen;
    previousScreen = temp;

    // Enter previous screen
    if (currentScreen)
    {
        currentScreen->onEnter();
    }
}

// Set Resolution (immediately applied) [Implementation]
void Game::setResolution(unsigned int width, unsigned int height)
{
    std::cout << "[Game] Changing resolution to " << width << "x" << height << std::endl;

    windowWidth = width;
    windowHeight = height;

    // Recreate window with new size
    delete window;
    window = new sf::RenderWindow(
        sf::VideoMode({windowWidth, windowHeight}),
        WINDOW_TITLE,
        sf::Style::Titlebar | sf::Style::Close);
    window->setFramerateLimit(60);

    std::cout << "[Game] Resolution changed successfully" << std::endl;
}

// Set music volume (immediately applied) [Implementation]
void Game::setVolume(float volume)
{
    musicVolume = volume;
    if (musicVolume < 0.0f)
        musicVolume = 0.0f;
    if (musicVolume > 100.0f)
        musicVolume = 100.0f;

    backgroundMusic.setVolume(musicVolume);
    std::cout << "[Game] Music volume set to: " << musicVolume << std::endl;
}

// Set sound effects volume (immediately applied) [Implementation]
void Game::setSoundVolume(float volume)
{
    soundVolume = volume;
    if (soundVolume < 0.0f)
        soundVolume = 0.0f;
    if (soundVolume > 100.0f)
        soundVolume = 100.0f;

    std::cout << "[Game] Sound effects volume set to: " << soundVolume << std::endl;
}

// Set player reference [Implementation]
void Game::setPlayer(Player *newPlayer)
{
    if (player)
    {
        delete player; // Delete old player
    }
    player = newPlayer;
    std::cout << "[Game] Player set to: " << player->getName() << " (" << player->getRoleName() << ")" << std::endl;
}

// Main game loop [Implementation]
void Game::run()
{
    std::cout << "[Game] Entering main loop..." << std::endl;

    while (running && window->isOpen())
    {
        // Calculate delta time
        deltaTime = clock.restart().asSeconds();

        // Update AI Storyteller every frame (monitors player state)
        if (player)
        {
            AIStoryteller::getInstance()->update(deltaTime, player);
        }

        // Current screen handles everything
        if (currentScreen)
        {
            currentScreen->handleEvents();
            currentScreen->update(deltaTime);
            currentScreen->render(*window, font);
        }

        // Display
        window->display();
    }

    std::cout << "[Game] Exited main loop" << std::endl;
}

// Shutdown game [Implementation]
void Game::shutdown()
{
    std::cout << "\n[Game] Shutting down..." << std::endl;

    // Destroy AI Storyteller
    AIStoryteller::destroy();

    // Destroy all screens
    destroyScreens();

    // Stop music
    backgroundMusic.stop();

    // Close window
    if (window && window->isOpen())
    {
        window->close();
    }

    // Delete window
    delete window;
    window = static_cast<sf::RenderWindow *>(0);

    running = false;

    Logger::destroy(); // Close all log files

    std::cout << "[Game] Shutdown complete" << std::endl;
}

// Get current volume levels [Implementation]
float Game::getVolume() const
{
    return musicVolume;
}
// Get current sound effects volume level [Implementation]
float Game::getSoundVolume() const
{
    return soundVolume;
}
// Quit game [Implementation]
void Game::quit()
{
    running = false;
}
// Get window [Implementation]
sf::RenderWindow *Game::getWindow()
{
    return window;
}
// Get font [Implementation]
sf::Font &Game::getFont()
{
    return font;
}
// Get game state info [Implementation]
bool Game::isRunning() const
{
    return running;
}
// Get delta time [Implementation]
float Game::getDeltaTime() const
{
    return deltaTime;
}
// Get specific screens [Implementation]
Screen *Game::getStoryIntroScreen()
{
    return screens[static_cast<int>(ScreenType::STORY_INTRO)];
}
// Get player reference [Implementation]
Player &Game::getPlayer()
{
    return *player;
}
