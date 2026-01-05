#include "MainMenuScreen.h"
#include "Game.h"
#include <iostream>
#include <cmath>

// Constructor
MainMenuScreen::MainMenuScreen(Game *gameInstance) : Screen(gameInstance), selectedButton(0), mousePressed(false)
{
    std::cout << "[MainMenuScreen] Created" << std::endl;
}

// Destructor
MainMenuScreen::~MainMenuScreen()
{
    std::cout << "[MainMenuScreen] Destroyed" << std::endl;
}

// Navigate to this screen
void MainMenuScreen::onEnter()
{
    std::cout << "[MainMenuScreen] Entered" << std::endl;
    selectedButton = 0;
    mousePressed = false;
}

// Leave this screen
void MainMenuScreen::onExit()
{
    std::cout << "[MainMenuScreen] Exited" << std::endl;
}

// Handle events
void MainMenuScreen::handleEvents()
{
    sf::RenderWindow *window = game->getWindow();

    while (const std::optional event = window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            std::cout << "[MainMenu] Close event" << std::endl;
            game->quit();
        }

        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Escape)
            {
                std::cout << "[MainMenu] ESC pressed - exiting" << std::endl;
                game->quit();
            }
        }

        if (const auto *mousePress = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePress->button == sf::Mouse::Button::Left)
            {
                mousePressed = true;
            }
        }

        if (const auto *mouseRelease = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseRelease->button == sf::Mouse::Button::Left)
            {
                mousePressed = false;
            }
        }
    }
}

void MainMenuScreen::update(float)
{
    sf::RenderWindow *window = game->getWindow();
    sf::Vector2u windowSize = window->getSize();
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
    sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);

    // Responsive button dimensions
    float buttonWidth = windowSize.x * 0.29f;
    float buttonHeight = windowSize.y * 0.09f;
    float buttonX = windowSize.x / 2.0f - buttonWidth / 2.0f;
    float bottomMargin = windowSize.y * 0.23f;
    float exitButtonY = windowSize.y - bottomMargin - buttonHeight;
    float buttonGap = windowSize.y * 0.04f;
    float startButtonY = exitButtonY - buttonHeight - buttonGap;

    sf::FloatRect startBounds({buttonX, startButtonY}, {buttonWidth, buttonHeight});
    sf::FloatRect exitBounds({buttonX, exitButtonY}, {buttonWidth, buttonHeight});

    float settingsSize = windowSize.y * 0.078f;
    float settingsMargin = windowSize.x * 0.02f;
    float settingsX = windowSize.x - settingsMargin - settingsSize;
    float settingsY = settingsMargin;
    sf::FloatRect settingsBounds({settingsX, settingsY}, {settingsSize, settingsSize});

    selectedButton = -1;

    // Check interactions
    if (startBounds.contains(mousePos))
    {
        selectedButton = 0;
        if (mousePressed)
        {
            std::cout << "[MainMenu] START clicked" << std::endl;
            game->changeScreen(Game::ScreenType::ROLE_SELECTION);
            mousePressed = false;
        }
    }
    else if (exitBounds.contains(mousePos))
    {
        selectedButton = 1;
        if (mousePressed)
        {
            std::cout << "[MainMenu] EXIT clicked" << std::endl;
            game->quit();
            mousePressed = false;
        }
    }
    else if (settingsBounds.contains(mousePos))
    {
        selectedButton = 2;
        if (mousePressed)
        {
            std::cout << "[MainMenu] SETTINGS clicked" << std::endl;
            game->changeScreen(Game::ScreenType::SETTINGS);
            mousePressed = false;
        }
    }
}

// Render the main menu
void MainMenuScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    window.clear(sf::Color::White);

    // Get window size for responsive rendering
    sf::Vector2u windowSize = window.getSize();

    // Use Game's color constants
    const sf::Color &COLOR_BLACK = Game::COLOR_BLACK;

    float settingsSize = windowSize.y * 0.078f;
    float settingsMargin = windowSize.x * 0.02f;
    float settingsX = windowSize.x - settingsMargin - settingsSize;
    float settingsY = settingsMargin;

    sf::RectangleShape settingsButton({settingsSize, settingsSize});
    settingsButton.setPosition({settingsX, settingsY});

    if (selectedButton == 2)
    {
        settingsButton.setFillColor(COLOR_BLACK);
        settingsButton.setOutlineColor(COLOR_BLACK);
    }
    else
    {
        settingsButton.setFillColor(sf::Color::White);
        settingsButton.setOutlineColor(COLOR_BLACK);
    }
    settingsButton.setOutlineThickness(3.0f);
    window.draw(settingsButton);

    sf::Color gearColor = (selectedButton == 2) ? sf::Color::White : COLOR_BLACK;
    drawGearIcon(window, settingsX + settingsSize / 2, settingsY + settingsSize / 2, settingsSize * 0.35f, gearColor);

    float titleSize = windowSize.y * 0.130f;
    sf::Text titleText(font, "LIFE OF", static_cast<unsigned int>(titleSize));
    titleText.setFillColor(COLOR_BLACK);
    titleText.setStyle(sf::Text::Bold);

    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
    titleText.setPosition({windowSize.x / 2.0f, windowSize.y * 0.26f});
    window.draw(titleText);

    sf::Text titleText2(font, "PROGRAMMER", static_cast<unsigned int>(titleSize));
    titleText2.setFillColor(COLOR_BLACK);
    titleText2.setStyle(sf::Text::Bold);

    sf::FloatRect title2Bounds = titleText2.getLocalBounds();
    titleText2.setOrigin({title2Bounds.size.x / 2.0f, title2Bounds.size.y / 2.0f});
    titleText2.setPosition({windowSize.x / 2.0f, windowSize.y * 0.37f});
    window.draw(titleText2);

    float buttonWidth = windowSize.x * 0.29f;
    float buttonHeight = windowSize.y * 0.09f;
    float buttonX = windowSize.x / 2.0f - buttonWidth / 2.0f;
    float bottomMargin = windowSize.y * 0.23f;
    float exitButtonY = windowSize.y - bottomMargin - buttonHeight;
    float buttonGap = windowSize.y * 0.04f;
    float startButtonY = exitButtonY - buttonHeight - buttonGap;

    float buttonTextSize = windowSize.y * 0.065f;

    sf::RectangleShape startButton({buttonWidth, buttonHeight});
    startButton.setPosition({buttonX, startButtonY});
    startButton.setFillColor(selectedButton == 0 ? COLOR_BLACK : sf::Color::White);
    startButton.setOutlineColor(COLOR_BLACK);
    startButton.setOutlineThickness(4.0f);
    window.draw(startButton);

    sf::Text startText(font, "START", static_cast<unsigned int>(buttonTextSize));
    startText.setFillColor(selectedButton == 0 ? sf::Color::White : COLOR_BLACK);
    startText.setStyle(sf::Text::Bold);

    sf::FloatRect startTextBounds = startText.getLocalBounds();
    startText.setOrigin({startTextBounds.position.x + startTextBounds.size.x / 2.0f, startTextBounds.position.y + startTextBounds.size.y / 2.0f});
    startText.setPosition({buttonX + buttonWidth / 2.0f, startButtonY + buttonHeight / 2.0f});
    window.draw(startText);

    // EXIT BUTTON
    sf::RectangleShape exitButton({buttonWidth, buttonHeight});
    exitButton.setPosition({buttonX, exitButtonY});
    exitButton.setFillColor(selectedButton == 1 ? COLOR_BLACK : sf::Color::White);
    exitButton.setOutlineColor(COLOR_BLACK);
    exitButton.setOutlineThickness(4.0f);
    window.draw(exitButton);

    sf::Text exitText(font, "EXIT", static_cast<unsigned int>(buttonTextSize));
    exitText.setFillColor(selectedButton == 1 ? sf::Color::White : COLOR_BLACK);
    exitText.setStyle(sf::Text::Bold);

    sf::FloatRect exitTextBounds = exitText.getLocalBounds();
    exitText.setOrigin({exitTextBounds.position.x + exitTextBounds.size.x / 2.0f, exitTextBounds.position.y + exitTextBounds.size.y / 2.0f});
    exitText.setPosition({buttonX + buttonWidth / 2.0f, exitButtonY + buttonHeight / 2.0f});
    window.draw(exitText);

    // FOOTER - Responsive (BIGGER for laptop)
    float footerSize = windowSize.y * 0.026f; // Increased from 0.021f
    sf::Text controlsText(font, "Use Mouse to Select and Click", static_cast<unsigned int>(footerSize));
    controlsText.setFillColor(COLOR_BLACK);

    sf::FloatRect controlsBounds = controlsText.getLocalBounds();
    controlsText.setOrigin({controlsBounds.size.x / 2.0f, controlsBounds.size.y / 2.0f});
    controlsText.setPosition({windowSize.x / 2.0f, windowSize.y * 0.95f}); // 95% from top
    window.draw(controlsText);
}

// Helper function to check if mouse is over a button
bool MainMenuScreen::isMouseOverButton(sf::RenderWindow &window, sf::FloatRect buttonBounds)
{
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
    sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);
    return buttonBounds.contains(mousePos);
}

// Draw an improved gear icon with proper teeth
void MainMenuScreen::drawGearIcon(sf::RenderWindow &window, float x, float y, float radius, sf::Color color)
{
    const int teeth = 12;
    const float innerRadius = radius * 0.5f;
    const float outerRadius = radius;

    sf::CircleShape outerCircle(outerRadius, teeth * 4);
    outerCircle.setOrigin({outerRadius, outerRadius});
    outerCircle.setPosition({x, y});
    outerCircle.setFillColor(color);
    outerCircle.setOutlineColor(color);
    outerCircle.setOutlineThickness(1.0f);
    window.draw(outerCircle);

    for (int i = 0; i < teeth; i++)
    {
        float angle = i * (360.0f / teeth) * 3.14159f / 180.0f;
        float toothWidth = radius * 0.25f;
        float toothHeight = radius * 0.35f;

        sf::RectangleShape tooth({toothWidth, toothHeight});
        tooth.setOrigin({toothWidth / 2, 0});
        tooth.setPosition({x + std::cos(angle) * outerRadius * 0.85f,
                           y + std::sin(angle) * outerRadius * 0.85f});
        tooth.setRotation(sf::degrees(angle * 180.0f / 3.14159f + 90.0f));
        tooth.setFillColor(color);
        window.draw(tooth);
    }

    // Draw inner circle (hole in the middle)
    sf::CircleShape innerCircle(innerRadius);
    innerCircle.setOrigin({innerRadius, innerRadius});
    innerCircle.setPosition({x, y});
    sf::Color holeColor = (color == sf::Color::White) ? Game::COLOR_BLACK : sf::Color::White;
    innerCircle.setFillColor(holeColor);
    window.draw(innerCircle);

    sf::CircleShape centerCircle(innerRadius * 0.4f);
    centerCircle.setOrigin({innerRadius * 0.4f, innerRadius * 0.4f});
    centerCircle.setPosition({x, y});
    centerCircle.setFillColor(color);
    window.draw(centerCircle);
}
