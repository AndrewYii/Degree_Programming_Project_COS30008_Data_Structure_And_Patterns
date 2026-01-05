#include "RestScreen.h"
#include "Game.h"
#include "Player.h"
#include <iostream>
#include <algorithm>

// Constructor [Implementation]
RestScreen::RestScreen(Game *g) : Screen(g), game(g), playerRef(nullptr), hoveredButton(-1), actionTaken(false), viewingDeck(false), hoveredCardIndex(-1), selectedCardIndex(-1), showUpgradeConfirm(false), confirmAlpha(0.0f), deckScrollOffset(0.0f), fadeAlpha(0.0f)
{
}

// Destructor [Implementation]
RestScreen::~RestScreen()
{
}

// Navigate to this screen [Implementation]
void RestScreen::onEnter()
{
    std::cout << "[RestScreen] Entered - rest area" << std::endl;
    playerRef = &game->getPlayer();
    fadeAlpha = 0.0f;
    actionTaken = false;
    hoveredButton = -1;
    viewingDeck = false;
    hoveredCardIndex = -1;
    selectedCardIndex = -1;
    showUpgradeConfirm = false;
    confirmAlpha = 0.0f;
    deckScrollOffset = 0.0f;
}

// Exit from this screen [Implementation]
void RestScreen::onExit()
{
    std::cout << "[RestScreen] Exiting" << std::endl;
}

void RestScreen::healPlayer()
{
    int maxHP = playerRef->getMaxHP();
    int healAmount = static_cast<int>(maxHP * 0.3f);
    playerRef->heal(healAmount);
    std::cout << "[Rest] Player healed for " << healAmount << " HP" << std::endl;
    actionTaken = true;
}
// Open upgrade card selection [Implementation]
void RestScreen::openUpgradeSelection()
{
    std::cout << "[Rest] Opening upgrade card selection" << std::endl;
    viewingDeck = true;
    hoveredCardIndex = -1;
    selectedCardIndex = -1;
    deckScrollOffset = 0.0f;

    // Ensure all card icons are loaded
    std::vector<Card *> &deck = playerRef->getMasterDeck();
    for (Card *c : deck)
    {
        if (c && c->getIconSprite() == nullptr)
        {
            c->loadIcon();
        }
    }
}

// Upgrade selected card [Implementation]
void RestScreen::upgradeCard(int cardIndex)
{
    std::vector<Card *> &deck = playerRef->getMasterDeck();
    if (cardIndex >= 0 && cardIndex < static_cast<int>(deck.size()))
    {
        Card *card = deck[cardIndex];
        card->upgrade();
        std::cout << "[Rest] Upgraded card: " << card->getName() << std::endl;
        actionTaken = true;
        viewingDeck = false;
        showUpgradeConfirm = false;
    }
}

// Handle events [Implementation]
void RestScreen::handleEvents()
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

                // If showing upgrade confirm popup
                if (showUpgradeConfirm)
                {
                    float popupWidth = 500.0f;
                    float popupHeight = 600.0f;
                    float popupX = (windowSize.x - popupWidth) / 2.0f;
                    float popupY = (windowSize.y - popupHeight) / 2.0f;

                    // Check confirm button
                    float buttonW = 300.0f;
                    float buttonH = 50.0f;
                    float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
                    float buttonY = popupY + popupHeight - 70.0f;
                    sf::FloatRect confirmBounds({buttonX, buttonY}, {buttonW, buttonH});

                    if (confirmBounds.contains(mousePos))
                    {
                        std::cout << "[Rest] Confirm upgrade" << std::endl;
                        upgradeCard(selectedCardIndex);
                    }
                    else
                    {
                        // Click outside to close
                        sf::FloatRect popupBounds({popupX, popupY}, {popupWidth, popupHeight});
                        if (!popupBounds.contains(mousePos))
                        {
                            std::cout << "[Rest] Clicked outside - closing confirm" << std::endl;
                            showUpgradeConfirm = false;
                            confirmAlpha = 0.0f;
                        }
                    }
                }
                // If viewing deck for upgrade
                else if (viewingDeck)
                {
                    std::vector<Card *> &deck = playerRef->getMasterDeck();

                    // Check card clicks
                    float cardWidth = 140.0f;
                    float cardHeight = 200.0f;
                    float spacing = 15.0f;
                    int cardsPerRow = 7;
                    float totalWidth = cardsPerRow * cardWidth + (cardsPerRow - 1) * spacing;
                    float startX = (windowSize.x - totalWidth) / 2.0f;
                    float startY = 150.0f;

                    for (int i = 0; i < static_cast<int>(deck.size()); i++)
                    {
                        int row = i / cardsPerRow;
                        int col = i % cardsPerRow;
                        float cardX = startX + col * (cardWidth + spacing);
                        float cardY = startY + row * (cardHeight + spacing) - deckScrollOffset;
                        sf::FloatRect cardBounds({cardX, cardY}, {cardWidth, cardHeight});

                        if (cardBounds.contains(mousePos))
                        {
                            std::cout << "[Rest] Card " << i << " clicked - showing upgrade confirm" << std::endl;
                            selectedCardIndex = i;
                            showUpgradeConfirm = true;
                            confirmAlpha = 0.0f;
                            break;
                        }
                    }

                    // Check close button
                    float closeW = 150.0f;
                    float closeH = 40.0f;
                    float closeX = windowSize.x - closeW - 30;
                    float closeY = 30;
                    sf::FloatRect closeBounds({closeX, closeY}, {closeW, closeH});

                    if (closeBounds.contains(mousePos))
                    {
                        std::cout << "[Rest] Closing deck view" << std::endl;
                        viewingDeck = false;
                    }
                }
                else if (!actionTaken)
                {
                    float buttonW = 350.0f;
                    float buttonH = 80.0f;
                    float buttonSpacing = 30.0f;
                    float startY = windowSize.y * 0.5f;

                    // Heal button
                    float healX = (windowSize.x - buttonW) / 2.0f;
                    float healY = startY;
                    sf::FloatRect healBounds({healX, healY}, {buttonW, buttonH});

                    // Upgrade button
                    float upgradeX = (windowSize.x - buttonW) / 2.0f;
                    float upgradeY = startY + buttonH + buttonSpacing;
                    sf::FloatRect upgradeBounds({upgradeX, upgradeY}, {buttonW, buttonH});

                    if (healBounds.contains(mousePos))
                    {
                        healPlayer();
                    }
                    else if (upgradeBounds.contains(mousePos))
                    {
                        openUpgradeSelection();
                    }
                }
                else
                {
                    // After action, click anywhere to continue
                    std::cout << "[Rest] Continue - returning to map" << std::endl;
                    game->changeScreen(Game::ScreenType::GAMEPLAY);
                }
            }
        }

        // Mouse wheel scrolling for deck
        if (const auto *wheel = event->getIf<sf::Event::MouseWheelScrolled>())
        {
            if (viewingDeck && !showUpgradeConfirm)
            {
                sf::Vector2u windowSize = window->getSize();
                float cardHeight = 200.0f;
                float spacing = 15.0f;
                int cardsPerRow = 7;
                std::vector<Card *> &deck = playerRef->getMasterDeck();
                int rows = static_cast<int>((deck.size() + cardsPerRow - 1) / cardsPerRow);
                float startY = 150.0f;
                float totalHeight = rows * (cardHeight + spacing) + startY;
                float visibleHeight = static_cast<float>(windowSize.y) - 200.0f;

                float maxScroll = std::max(0.0f, totalHeight - visibleHeight);
                deckScrollOffset -= wheel->delta * 60.0f;
                if (deckScrollOffset < 0.0f)
                    deckScrollOffset = 0.0f;
                if (deckScrollOffset > maxScroll)
                    deckScrollOffset = maxScroll;
            }
        }
    }
}

// Update logic [Implementation]
void RestScreen::update(float deltaTime)
{
    // Fade in
    if (fadeAlpha < 255.0f)
    {
        fadeAlpha += deltaTime * 300.0f;
        if (fadeAlpha > 255.0f)
            fadeAlpha = 255.0f;
    }

    // Fade in confirm popup
    if (showUpgradeConfirm && confirmAlpha < 255.0f)
    {
        confirmAlpha += deltaTime * 500.0f;
        if (confirmAlpha > 255.0f)
            confirmAlpha = 255.0f;
    }

    // Update hover states
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*game->getWindow());
    sf::Vector2f mousePos = game->getWindow()->mapPixelToCoords(mousePixelPos);
    sf::Vector2u windowSize = game->getWindow()->getSize();

    hoveredButton = -1;
    hoveredCardIndex = -1;

    if (viewingDeck && !showUpgradeConfirm)
    {
        // Hover detection for cards in deck
        std::vector<Card *> &deck = playerRef->getMasterDeck();
        float cardWidth = 140.0f;
        float cardHeight = 200.0f;
        float spacing = 15.0f;
        int cardsPerRow = 7;
        float totalWidth = cardsPerRow * cardWidth + (cardsPerRow - 1) * spacing;
        float startX = (windowSize.x - totalWidth) / 2.0f;
        float startY = 150.0f;

        for (int i = 0; i < static_cast<int>(deck.size()); i++)
        {
            int row = i / cardsPerRow;
            int col = i % cardsPerRow;
            float cardX = startX + col * (cardWidth + spacing);
            float cardY = startY + row * (cardHeight + spacing) - deckScrollOffset;
            sf::FloatRect cardBounds({cardX, cardY}, {cardWidth, cardHeight});

            if (cardBounds.contains(mousePos))
            {
                hoveredCardIndex = i;
                break;
            }
        }
    }
    else if (!actionTaken && !viewingDeck)
    {
        float buttonW = 350.0f;
        float buttonH = 80.0f;
        float buttonSpacing = 30.0f;
        float startY = windowSize.y * 0.5f;

        // Heal button
        float healX = (windowSize.x - buttonW) / 2.0f;
        float healY = startY;
        sf::FloatRect healBounds({healX, healY}, {buttonW, buttonH});

        // Upgrade button
        float upgradeX = (windowSize.x - buttonW) / 2.0f;
        float upgradeY = startY + buttonH + buttonSpacing;
        sf::FloatRect upgradeBounds({upgradeX, upgradeY}, {buttonW, buttonH});

        if (healBounds.contains(mousePos))
            hoveredButton = 0;
        else if (upgradeBounds.contains(mousePos))
            hoveredButton = 1;
    }
}

// Render screen [Implementation]
void RestScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    // Background (calm blue theme)
    window.clear(sf::Color(150, 200, 230));

    // Title
    sf::Text titleText(font, "REST AREA", 72);
    titleText.setFillColor(sf::Color(40, 80, 120, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 80});
    window.draw(titleText);

    // Subtitle
    std::string subtitleStr = actionTaken ? "Rest Complete!" : "Choose an action:";
    sf::Text subtitleText(font, subtitleStr, 32);
    subtitleText.setFillColor(sf::Color(60, 100, 140, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect subtitleBounds = subtitleText.getLocalBounds();
    subtitleText.setOrigin({subtitleBounds.size.x / 2.0f, 0});
    subtitleText.setPosition({windowSize.x / 2.0f, 180});
    window.draw(subtitleText);

    if (!actionTaken)
    {
        // Draw buttons
        float buttonW = 350.0f;
        float buttonH = 80.0f;
        float buttonSpacing = 30.0f;
        float startY = windowSize.y * 0.5f;

        // Heal button
        float healX = (windowSize.x - buttonW) / 2.0f;
        float healY = startY;
        sf::RectangleShape healButton({buttonW, buttonH});
        healButton.setPosition({healX, healY});
        healButton.setFillColor(hoveredButton == 0 ? sf::Color(80, 180, 80, static_cast<std::uint8_t>(fadeAlpha))
                                                   : sf::Color(100, 200, 100, static_cast<std::uint8_t>(fadeAlpha)));
        window.draw(healButton);

        int healAmount = static_cast<int>(playerRef->getMaxHP() * 0.3f);
        sf::Text healText(font, "HEAL +" + std::to_string(healAmount) + " HP", 28);
        healText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
        healText.setStyle(sf::Text::Bold);
        sf::FloatRect healTextBounds = healText.getLocalBounds();
        healText.setOrigin({healTextBounds.size.x / 2.0f, healTextBounds.size.y / 2.0f});
        healText.setPosition({healX + buttonW / 2.0f, healY + buttonH / 2.0f - 5});
        window.draw(healText);
        sf::Text healDescText(font, "Restore 30% of max HP", 18);
        healDescText.setFillColor(sf::Color(220, 255, 220, static_cast<std::uint8_t>(fadeAlpha)));
        sf::FloatRect healDescBounds = healDescText.getLocalBounds();
        healDescText.setOrigin({healDescBounds.size.x / 2.0f, 0});
        healDescText.setPosition({healX + buttonW / 2.0f, healY + buttonH / 2.0f + 15});
        window.draw(healDescText);

        // Upgrade button
        float upgradeX = (windowSize.x - buttonW) / 2.0f;
        float upgradeY = startY + buttonH + buttonSpacing;
        sf::RectangleShape upgradeButton({buttonW, buttonH});
        upgradeButton.setPosition({upgradeX, upgradeY});
        upgradeButton.setFillColor(hoveredButton == 1 ? sf::Color(150, 100, 200, static_cast<std::uint8_t>(fadeAlpha))
                                                      : sf::Color(180, 120, 220, static_cast<std::uint8_t>(fadeAlpha)));
        window.draw(upgradeButton);

        sf::Text upgradeText(font, "UPGRADE CARD", 28);
        upgradeText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
        upgradeText.setStyle(sf::Text::Bold);
        sf::FloatRect upgradeTextBounds = upgradeText.getLocalBounds();
        upgradeText.setOrigin({upgradeTextBounds.size.x / 2.0f, upgradeTextBounds.size.y / 2.0f});
        upgradeText.setPosition({upgradeX + buttonW / 2.0f, upgradeY + buttonH / 2.0f - 5});
        window.draw(upgradeText);

        sf::Text upgradeDescText(font, "Enhance a card from your deck", 18);
        upgradeDescText.setFillColor(sf::Color(230, 220, 255, static_cast<std::uint8_t>(fadeAlpha)));
        sf::FloatRect upgradeDescBounds = upgradeDescText.getLocalBounds();
        upgradeDescText.setOrigin({upgradeDescBounds.size.x / 2.0f, 0});
        upgradeDescText.setPosition({upgradeX + buttonW / 2.0f, upgradeY + buttonH / 2.0f + 15});
        window.draw(upgradeDescText);
    }
    else
    {
        // Show completion message
        sf::Text completeText(font, "Click anywhere to continue", 28);
        completeText.setFillColor(sf::Color(60, 100, 140, static_cast<std::uint8_t>(fadeAlpha)));
        sf::FloatRect completeBounds = completeText.getLocalBounds();
        completeText.setOrigin({completeBounds.size.x / 2.0f, 0});
        completeText.setPosition({windowSize.x / 2.0f, windowSize.y * 0.7f});
        window.draw(completeText);
    }

    // Draw upgrade overlay if active
    if (viewingDeck)
    {
        drawUpgradeOverlay(window, font);
    }

    // Draw upgrade confirm popup if active
    if (showUpgradeConfirm)
    {
        drawUpgradeConfirmPopup(window, font);
    }
}

// Draw upgrade overlay [Implementation]
void RestScreen::drawUpgradeOverlay(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();
    std::vector<Card *> &deck = playerRef->getMasterDeck();

    // Dark overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    // Title
    sf::Text titleText(font, "SELECT CARD TO UPGRADE", 48);
    titleText.setFillColor(sf::Color::White);
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 50});
    window.draw(titleText);

    // Help text
    sf::Text helpText(font, "Click a card to preview upgrade", 22);
    helpText.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect helpBounds = helpText.getLocalBounds();
    helpText.setOrigin({helpBounds.size.x / 2.0f, 0});
    helpText.setPosition({windowSize.x / 2.0f, 110});
    window.draw(helpText);

    // Draw cards
    float cardWidth = 140.0f;
    float cardHeight = 200.0f;
    float spacing = 15.0f;
    int cardsPerRow = 7;
    float totalWidth = cardsPerRow * cardWidth + (cardsPerRow - 1) * spacing;
    float startX = (windowSize.x - totalWidth) / 2.0f;
    float startY = 150.0f;

    for (int i = 0; i < static_cast<int>(deck.size()); i++)
    {
        Card *card = deck[i];
        int row = i / cardsPerRow;
        int col = i % cardsPerRow;
        float cardX = startX + col * (cardWidth + spacing);
        float cardY = startY + row * (cardHeight + spacing) - deckScrollOffset;
        bool isHovered = (hoveredCardIndex == i);

        // Card background
        sf::RectangleShape cardBg({cardWidth, cardHeight});
        cardBg.setPosition({cardX, cardY});
        cardBg.setFillColor(sf::Color(240, 240, 230));
        cardBg.setOutlineThickness(isHovered ? 4.0f : 2.0f);
        cardBg.setOutlineColor(isHovered ? sf::Color(180, 120, 220) : sf::Color(100, 100, 100));
        window.draw(cardBg);

        // Hover glow
        if (isHovered)
        {
            sf::RectangleShape glow({cardWidth + 8, cardHeight + 8});
            glow.setPosition({cardX - 4, cardY - 4});
            glow.setFillColor(sf::Color::Transparent);
            glow.setOutlineThickness(4.0f);
            glow.setOutlineColor(sf::Color(200, 150, 255, 180));
            window.draw(glow);
        }

        // Card name
        sf::Text nameText(font, card->getName(), 16);
        nameText.setFillColor(sf::Color::Black);
        nameText.setStyle(sf::Text::Bold);
        sf::FloatRect nameBounds = nameText.getLocalBounds();
        nameText.setOrigin({nameBounds.size.x / 2.0f, 0});
        nameText.setPosition({cardX + cardWidth / 2.0f, cardY + 10});
        window.draw(nameText);

        sf::Sprite *iconSprite = card->getIconSprite();
        if (iconSprite)
        {
            const sf::Texture &iconTexture = iconSprite->getTexture();
            sf::Vector2u iconSize = iconTexture.getSize();
            float iconScale = 80.0f / std::max(1u, iconSize.x);
            sf::Sprite tempSprite = *iconSprite;
            tempSprite.setScale({iconScale, iconScale});
            tempSprite.setPosition({cardX + (cardWidth - 80) / 2.0f, cardY + 40});
            window.draw(tempSprite);
        }

        if (card->isUpgraded())
        {
            sf::CircleShape upgradeCircle(12.0f, 5);
            upgradeCircle.setPosition({cardX + cardWidth - 30, cardY + 8});
            upgradeCircle.setFillColor(sf::Color(255, 215, 0));
            upgradeCircle.setOutlineColor(sf::Color(255, 165, 0));
            upgradeCircle.setOutlineThickness(2.0f);
            window.draw(upgradeCircle);

            sf::Text upgradeText(font, "+1", 16);
            upgradeText.setFillColor(sf::Color::Black);
            upgradeText.setStyle(sf::Text::Bold);
            sf::FloatRect upgradeBounds = upgradeText.getLocalBounds();
            upgradeText.setOrigin({upgradeBounds.size.x / 2.0f, upgradeBounds.size.y / 2.0f});
            upgradeText.setPosition({cardX + cardWidth - 18, cardY + 20});
            window.draw(upgradeText);
        }
    }

    float closeW = 150.0f;
    float closeH = 40.0f;
    float closeX = windowSize.x - closeW - 30;
    float closeY = 30;

    sf::RectangleShape closeButton({closeW, closeH});
    closeButton.setPosition({closeX, closeY});
    closeButton.setFillColor(sf::Color(180, 80, 80));
    window.draw(closeButton);

    sf::Text closeText(font, "CLOSE", 20);
    closeText.setFillColor(sf::Color::White);
    closeText.setStyle(sf::Text::Bold);
    sf::FloatRect closeBounds = closeText.getLocalBounds();
    closeText.setOrigin({closeBounds.size.x / 2.0f, closeBounds.size.y / 2.0f});
    closeText.setPosition({closeX + closeW / 2.0f, closeY + closeH / 2.0f});
    window.draw(closeText);
}

// Draw upgrade confirm overlay [Implementation]
void RestScreen::drawUpgradeConfirmPopup(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();
    std::vector<Card *> &deck = playerRef->getMasterDeck();
    Card *card = deck[selectedCardIndex];

    // Darker overlay on top
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(confirmAlpha * 0.7f)));
    window.draw(overlay);

    // Popup panel
    float popupWidth = 500.0f;
    float popupHeight = 600.0f;
    float popupX = (windowSize.x - popupWidth) / 2.0f;
    float popupY = (windowSize.y - popupHeight) / 2.0f;

    sf::RectangleShape popup({popupWidth, popupHeight});
    popup.setPosition({popupX, popupY});
    popup.setFillColor(sf::Color(240, 235, 255, static_cast<std::uint8_t>(confirmAlpha)));
    popup.setOutlineColor(sf::Color(180, 120, 220, static_cast<std::uint8_t>(confirmAlpha)));
    popup.setOutlineThickness(5.0f);
    window.draw(popup);

    // Card name
    sf::Text nameText(font, card->getName(), 32);
    nameText.setFillColor(sf::Color(20, 20, 20, static_cast<std::uint8_t>(confirmAlpha)));
    nameText.setStyle(sf::Text::Bold);
    sf::FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setOrigin({nameBounds.size.x / 2.0f, 0});
    nameText.setPosition({popupX + popupWidth / 2.0f, popupY + 30});
    window.draw(nameText);

    // Card icon (data type sprite)
    sf::Sprite *iconSprite = card->getIconSprite();
    if (iconSprite)
    {
        const sf::Texture &iconTexture = iconSprite->getTexture();
        sf::Vector2u iconSize = iconTexture.getSize();
        float iconScale = 160.0f / std::max(1u, iconSize.x);
        sf::Sprite tempSprite = *iconSprite;
        tempSprite.setScale({iconScale, iconScale});
        tempSprite.setPosition({popupX + (popupWidth - 160) / 2.0f, popupY + 90});
        sf::Color iconColor = tempSprite.getColor();
        iconColor.a = static_cast<std::uint8_t>(confirmAlpha);
        tempSprite.setColor(iconColor);
        window.draw(tempSprite);
    }

    // Upgrade arrow and text
    std::string upgradeStr = card->isUpgraded() ? "ALREADY UPGRADED" : "UPGRADE CARD?";
    sf::Text upgradeText(font, upgradeStr, 24);
    upgradeText.setFillColor(card->isUpgraded() ? sf::Color(180, 180, 0, static_cast<std::uint8_t>(confirmAlpha))
                                                : sf::Color(100, 200, 100, static_cast<std::uint8_t>(confirmAlpha)));
    upgradeText.setStyle(sf::Text::Bold);
    sf::FloatRect upgradeBounds = upgradeText.getLocalBounds();
    upgradeText.setOrigin({upgradeBounds.size.x / 2.0f, 0});
    upgradeText.setPosition({popupX + popupWidth / 2.0f, popupY + 80});
    window.draw(upgradeText);

    sf::Text descText(font, card->getDescription(), 18);
    descText.setFillColor(sf::Color(40, 40, 40, static_cast<std::uint8_t>(confirmAlpha)));
    descText.setPosition({popupX + 30, popupY + 280});
    window.draw(descText);

    sf::CircleShape costCircle(25.0f);
    costCircle.setPosition({popupX + 30, popupY + 350});
    costCircle.setFillColor(sf::Color(100, 150, 255, static_cast<std::uint8_t>(confirmAlpha)));
    costCircle.setOutlineColor(sf::Color(20, 20, 20, static_cast<std::uint8_t>(confirmAlpha)));
    costCircle.setOutlineThickness(3.0f);
    window.draw(costCircle);

    sf::Text costText(font, std::to_string(card->getEnergyCost()), 28);
    costText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(confirmAlpha)));
    costText.setStyle(sf::Text::Bold);
    sf::FloatRect costBounds = costText.getLocalBounds();
    costText.setOrigin({costBounds.size.x / 2.0f, costBounds.size.y / 2.0f});
    costText.setPosition({popupX + 30 + 25, popupY + 350 + 25});
    window.draw(costText);

    sf::Text costLabel(font, "ENERGY COST", 16);
    costLabel.setFillColor(sf::Color(60, 60, 60, static_cast<std::uint8_t>(confirmAlpha)));
    costLabel.setPosition({popupX + 85, popupY + 360});
    window.draw(costLabel);

    if (!card->isUpgraded())
    {
        sf::Text infoText(font, "Upgrading will enhance this card's effects.\nMost cards get improved damage, defense,\nor additional effects.", 18);
        infoText.setFillColor(sf::Color(80, 80, 80, static_cast<std::uint8_t>(confirmAlpha)));
        infoText.setPosition({popupX + 30, popupY + 420});
        window.draw(infoText);
    }

    float buttonW = 300.0f;
    float buttonH = 50.0f;
    float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
    float buttonY = popupY + popupHeight - 70.0f;

    if (!card->isUpgraded())
    {
        sf::RectangleShape confirmButton({buttonW, buttonH});
        confirmButton.setPosition({buttonX, buttonY});
        confirmButton.setFillColor(sf::Color(150, 100, 220, static_cast<std::uint8_t>(confirmAlpha)));
        confirmButton.setOutlineColor(sf::Color(100, 50, 180, static_cast<std::uint8_t>(confirmAlpha)));
        confirmButton.setOutlineThickness(3.0f);
        window.draw(confirmButton);

        sf::Text confirmText(font, "UPGRADE THIS CARD", 24);
        confirmText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(confirmAlpha)));
        confirmText.setStyle(sf::Text::Bold);
        sf::FloatRect confirmBounds = confirmText.getLocalBounds();
        confirmText.setOrigin({confirmBounds.size.x / 2.0f, confirmBounds.size.y / 2.0f});
        confirmText.setPosition({buttonX + buttonW / 2.0f, buttonY + buttonH / 2.0f});
        window.draw(confirmText);
    }
    else
    {
        sf::Text alreadyText(font, "Card already upgraded!\nChoose a different card.", 20);
        alreadyText.setFillColor(sf::Color(150, 100, 0, static_cast<std::uint8_t>(confirmAlpha)));
        sf::FloatRect alreadyBounds = alreadyText.getLocalBounds();
        alreadyText.setOrigin({alreadyBounds.size.x / 2.0f, 0});
        alreadyText.setPosition({popupX + popupWidth / 2.0f, buttonY});
        window.draw(alreadyText);
    }
}
