#include "CreditsScreen.h"
#include "Game.h"

// Constructor [Implementation]
CreditsScreen::CreditsScreen(Game *gameInstance) : Screen(gameInstance), scrollY(0.f), scrollSpeed(50.f), scrollComplete(false)
{
    initializeCredits();
}
// Destructor [Implementation]
CreditsScreen::~CreditsScreen()
{
}
// Initialize credit lines [Implementation]
void CreditsScreen::initializeCredits()
{
    creditLines.clear();
    creditLines.push_back(CreditLine("", 1, sf::Color::White, false, 100.f));
    creditLines.push_back(CreditLine("PROGRAMMING PROJECT", 60, sf::Color(255, 215, 0), true, 80.f));
    creditLines.push_back(CreditLine("GAME DEVELOPED BY", 36, sf::Color(200, 200, 200), true, 20.f));
    creditLines.push_back(CreditLine("Andrew Yii", 48, sf::Color::White, true, 100.f));
    creditLines.push_back(CreditLine("PROGRAMMER", 32, sf::Color(100, 200, 255), true, 20.f));
    creditLines.push_back(CreditLine("Andrew Yii", 40, sf::Color::White, false, 100.f));
    creditLines.push_back(CreditLine("ART & DESIGN", 32, sf::Color(255, 100, 200), true, 20.f));
    creditLines.push_back(CreditLine("Andrew Yii", 40, sf::Color::White, false, 100.f));
    creditLines.push_back(CreditLine("MUSIC", 32, sf::Color(150, 255, 150), true, 20.f));
    creditLines.push_back(CreditLine("Andrew Yii", 40, sf::Color::White, false, 100.f));
    creditLines.push_back(CreditLine("SPECIAL THANKS", 36, sf::Color(255, 200, 100), true, 30.f));
    creditLines.push_back(CreditLine("Dr. Mark - My Lecturer", 32, sf::Color::White, false, 20.f));
    creditLines.push_back(CreditLine("For guidance and support", 24, sf::Color(180, 180, 180), false, 40.f));
    creditLines.push_back(CreditLine("SFML Development Team", 32, sf::Color::White, false, 20.f));
    creditLines.push_back(CreditLine("For the amazing game framework", 24, sf::Color(180, 180, 180), false, 40.f));
    creditLines.push_back(CreditLine("My Family and Friends", 32, sf::Color::White, false, 20.f));
    creditLines.push_back(CreditLine("For endless support and encouragement", 24, sf::Color(180, 180, 180), false, 100.f));
    creditLines.push_back(CreditLine("TESTERS", 32, sf::Color(200, 150, 255), true, 20.f));
    creditLines.push_back(CreditLine("Andrew Yii", 36, sf::Color::White, false, 15.f));
    creditLines.push_back(CreditLine("Dr. Mark", 36, sf::Color::White, false, 15.f));
    creditLines.push_back(CreditLine("My Friends", 36, sf::Color::White, false, 100.f));
    creditLines.push_back(CreditLine("BUILT WITH", 28, sf::Color(150, 150, 150), true, 20.f));
    creditLines.push_back(CreditLine("C++ & SFML", 32, sf::Color::White, false, 15.f));
    creditLines.push_back(CreditLine("Visual Studio 2022", 32, sf::Color::White, false, 100.f));
    creditLines.push_back(CreditLine("THANKS FOR PLAYING!", 52, sf::Color(255, 215, 0), true, 40.f));
    creditLines.push_back(CreditLine("Press any key to return to main menu", 28, sf::Color(180, 180, 180), false, 150.f));
    creditLines.push_back(CreditLine("", 1, sf::Color::White, false, 200.f));
}

void CreditsScreen::onEnter()
{
    scrollY = static_cast<float>(game->getWindow()->getSize().y);
    scrollComplete = false;
}

void CreditsScreen::onExit()
{
}

// Event handling [Implementation]
void CreditsScreen::handleEvents()
{
    while (const std::optional<sf::Event> event = game->getWindow()->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            game->quit();
        }

        // Press any key to return to main menu (after scrolling starts)
        if (event->is<sf::Event::KeyPressed>())
        {
            game->changeScreen(Game::ScreenType::MAIN_MENU);
        }

        // Click anywhere to return
        if (event->is<sf::Event::MouseButtonPressed>())
        {
            game->changeScreen(Game::ScreenType::MAIN_MENU);
        }
    }
}

// Update logic [Implementation]
void CreditsScreen::update(float deltaTime)
{
    if (!scrollComplete)
    {
        // Scroll upward
        scrollY -= scrollSpeed * deltaTime;

        // Check if we've scrolled past all content
        float totalHeight = 0.f;
        for (const auto &line : creditLines)
        {
            totalHeight += line.fontSize + line.spacing;
        }

        // If top of credits goes above screen, mark as complete
        if (scrollY + totalHeight < 0)
        {
            scrollComplete = true;
        }
    }
}

// Rendering [Implementation]
void CreditsScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    window.clear(sf::Color::Black);

    float currentY = scrollY;
    float screenWidth = static_cast<float>(window.getSize().x);

    for (const auto &line : creditLines)
    {
        // Only render if on screen
        if (currentY > -100 && currentY < window.getSize().y + 100)
        {
            if (!line.text.empty())
            {
                sf::Text text(font, line.text, line.fontSize);
                text.setFillColor(line.color);
                if (line.isBold)
                {
                    text.setStyle(sf::Text::Bold);
                }

                // Center horizontally
                sf::FloatRect bounds = text.getLocalBounds();
                text.setPosition(sf::Vector2f((screenWidth - bounds.size.x) / 2.f, currentY));

                window.draw(text);
            }
        }

        currentY += line.fontSize + line.spacing;
    }
}
