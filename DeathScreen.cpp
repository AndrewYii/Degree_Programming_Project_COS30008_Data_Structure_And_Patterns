#include "DeathScreen.h"
#include "Game.h"
#include <iostream>

// Constructor [Implementation]
DeathScreen::DeathScreen(Game *g) : Screen(g), game(g), hoveredButton(-1), fadeAlpha(0.0f)
{
}
// Destructor [Implementation]
DeathScreen::~DeathScreen()
{
}
// Navigate to this screen [Implementation]
void DeathScreen::onEnter()
{
    std::cout << "[DeathScreen] Player died - Game Over" << std::endl;
    fadeAlpha = 0.0f;
}
// Exit from this screen [Implementation]
void DeathScreen::onExit()
{
    std::cout << "[DeathScreen] Exiting" << std::endl;
}
// Handle events [Implementation]
void DeathScreen::handleEvents()
{
    sf::RenderWindow *window = game->getWindow();

    while (const std::optional event = window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            game->quit();
        }

        if (const auto *mousePress = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePress->button == sf::Mouse::Button::Left)
            {
                sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
                sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);
                sf::Vector2u windowSize = window->getSize();

                float buttonW = 300.0f;
                float buttonH = 60.0f;
                float buttonSpacing = 20.0f;
                float startY = windowSize.y * 0.6f;

                // Restart button
                float restartX = (windowSize.x - buttonW) / 2.0f;
                float restartY = startY;
                sf::FloatRect restartBounds({restartX, restartY}, {buttonW, buttonH});

                // Menu button
                float menuX = (windowSize.x - buttonW) / 2.0f;
                float menuY = startY + buttonH + buttonSpacing;
                sf::FloatRect menuBounds({menuX, menuY}, {buttonW, buttonH});

                if (restartBounds.contains(mousePos))
                {
                    std::cout << "[Death] Restarting game..." << std::endl;
                    // Reset player HP and go back to gameplay
                    game->getPlayer().heal(game->getPlayer().getMaxHP());
                    game->changeScreen(Game::ScreenType::GAMEPLAY);
                }
                else if (menuBounds.contains(mousePos))
                {
                    std::cout << "[Death] Returning to main menu..." << std::endl;
                    game->changeScreen(Game::ScreenType::MAIN_MENU);
                }
            }
        }
    }
}
// Update logic [Implementation]
void DeathScreen::update(float deltaTime)
{
    // Fade in
    if (fadeAlpha < 255.0f)
    {
        fadeAlpha += deltaTime * 300.0f;
        if (fadeAlpha > 255.0f)
            fadeAlpha = 255.0f;
    }

    // Update hover states
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*game->getWindow());
    sf::Vector2f mousePos = game->getWindow()->mapPixelToCoords(mousePixelPos);
    sf::Vector2u windowSize = game->getWindow()->getSize();

    float buttonW = 300.0f;
    float buttonH = 60.0f;
    float buttonSpacing = 20.0f;
    float startY = windowSize.y * 0.6f;

    hoveredButton = -1;

    // Restart button
    float restartX = (windowSize.x - buttonW) / 2.0f;
    float restartY = startY;
    sf::FloatRect restartBounds({restartX, restartY}, {buttonW, buttonH});

    // Menu button
    float menuX = (windowSize.x - buttonW) / 2.0f;
    float menuY = startY + buttonH + buttonSpacing;
    sf::FloatRect menuBounds({menuX, menuY}, {buttonW, buttonH});

    if (restartBounds.contains(mousePos))
        hoveredButton = 0;
    else if (menuBounds.contains(mousePos))
        hoveredButton = 1;
}
// Render screen [Implementation]
void DeathScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    // Dark background
    window.clear(sf::Color(40, 40, 40));

    // Title
    sf::Text titleText(font, "GAME OVER", 72);
    titleText.setFillColor(sf::Color(200, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition({(windowSize.x - titleBounds.size.x) / 2, windowSize.y * 0.25f});
    window.draw(titleText);

    // Subtitle
    sf::Text subtitle(font, "You have been defeated...", 28);
    subtitle.setFillColor(sf::Color(200, 200, 200, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect subtitleBounds = subtitle.getLocalBounds();
    subtitle.setPosition({(windowSize.x - subtitleBounds.size.x) / 2, windowSize.y * 0.40f});
    window.draw(subtitle);

    // Buttons
    float buttonW = 300.0f;
    float buttonH = 60.0f;
    float buttonSpacing = 20.0f;
    float startY = windowSize.y * 0.6f;

    // Restart button
    float restartX = (windowSize.x - buttonW) / 2.0f;
    float restartY = startY;

    sf::RectangleShape restartBtn({buttonW, buttonH});
    restartBtn.setPosition({restartX, restartY});
    restartBtn.setFillColor(hoveredButton == 0 ? sf::Color(100, 100, 150, static_cast<std::uint8_t>(fadeAlpha))
                                               : sf::Color(80, 80, 130, static_cast<std::uint8_t>(fadeAlpha)));
    restartBtn.setOutlineColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
    restartBtn.setOutlineThickness(3.0f);
    window.draw(restartBtn);

    sf::Text restartText(font, "Restart", 32);
    restartText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
    restartText.setStyle(sf::Text::Bold);
    sf::FloatRect restartTextBounds = restartText.getLocalBounds();
    restartText.setPosition({restartX + (buttonW - restartTextBounds.size.x) / 2,
                             restartY + (buttonH - restartTextBounds.size.y) / 2 - 5});
    window.draw(restartText);

    // Menu button
    float menuX = (windowSize.x - buttonW) / 2.0f;
    float menuY = startY + buttonH + buttonSpacing;

    sf::RectangleShape menuBtn({buttonW, buttonH});
    menuBtn.setPosition({menuX, menuY});
    menuBtn.setFillColor(hoveredButton == 1 ? sf::Color(100, 100, 100, static_cast<std::uint8_t>(fadeAlpha))
                                            : sf::Color(80, 80, 80, static_cast<std::uint8_t>(fadeAlpha)));
    menuBtn.setOutlineColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
    menuBtn.setOutlineThickness(3.0f);
    window.draw(menuBtn);

    sf::Text menuText(font, "Main Menu", 32);
    menuText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
    menuText.setStyle(sf::Text::Bold);
    sf::FloatRect menuTextBounds = menuText.getLocalBounds();
    menuText.setPosition({menuX + (buttonW - menuTextBounds.size.x) / 2,
                          menuY + (buttonH - menuTextBounds.size.y) / 2 - 5});
    window.draw(menuText);
}
