#include "TreasureScreen.h"
#include "Game.h"
#include "Player.h"
#include "CardFactory.h"
#include "AIStoryteller.h"
#include <iostream>
#include <sstream>

// Constructor
TreasureScreen::TreasureScreen(Game *g) : Screen(g), game(g), playerRef(nullptr), selectedCardIndex(-1), rewardClaimed(false), hoveredContinue(false), hoveredCardIndex(-1), showCardDetail(false), selectedCardForDetail(nullptr), cardDetailAlpha(0.0f), hoveredConfirmButton(false), fadeAlpha(0.0f)
{
}

// Destructor
TreasureScreen::~TreasureScreen()
{
    cardOptions.clear();
}

// Navigate to this screen
void TreasureScreen::onEnter()
{
    std::cout << "[TreasureScreen] Entered - generating treasure rewards..." << std::endl;
    playerRef = &game->getPlayer();
    fadeAlpha = 0.0f;
    rewardClaimed = false;
    selectedCardIndex = -1;
    generateRewards();
}

// Exit from this screen
void TreasureScreen::onExit()
{
    std::cout << "[TreasureScreen] Exiting" << std::endl;
    cardOptions.clear();
}

// Generate treasure card options
void TreasureScreen::generateRewards()
{
    cardOptions.clear();
    CardFactory *factory = CardFactory::getInstance();
    Card *uncommon = factory->getRandomCard(Card::CardRarity::UNCOMMON);
    Card *rare1 = factory->getRandomCard(Card::CardRarity::RARE);
    Card *rare2 = factory->getRandomCard(Card::CardRarity::RARE);

    if (uncommon)
        cardOptions.push_back(uncommon);
    if (rare1)
        cardOptions.push_back(rare1);
    if (rare2)
        cardOptions.push_back(rare2);

    std::cout << "[Treasure] Generated " << cardOptions.size() << " treasure card options" << std::endl;
}

// Claim the selected reward
void TreasureScreen::claimReward(int cardIndex)
{
    if (cardIndex >= 0 && cardIndex < static_cast<int>(cardOptions.size()))
    {
        Card *chosenCard = cardOptions[cardIndex];
        if (chosenCard && playerRef)
        {
            playerRef->addCardToDeck(chosenCard->clone());

            bool isRare = (chosenCard->getRarity() == Card::CardRarity::RARE || chosenCard->getRarity() == Card::CardRarity::EPIC);
            AIStoryteller::getInstance()->onCardAcquired(isRare);

            std::cout << "[Treasure] Player found: " << chosenCard->getName() << std::endl;
            rewardClaimed = true;
        }
    }
}

// Handle user input events
void TreasureScreen::handleEvents()
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
            sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
            sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);
            sf::Vector2u windowSize = window->getSize();

            if (mousePress->button == sf::Mouse::Button::Left)
            {
                if (showCardDetail && selectedCardForDetail)
                {
                    float buttonW = 250.0f;
                    float buttonH = 60.0f;
                    float popupWidth = 500;
                    float popupHeight = 600;
                    float popupX = (windowSize.x - popupWidth) / 2.0f;
                    float popupY = (windowSize.y - popupHeight) / 2.0f;
                    float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
                    float buttonY = popupY + popupHeight - 90;
                    sf::FloatRect confirmBounds({buttonX, buttonY}, {buttonW, buttonH});

                    if (confirmBounds.contains(mousePos))
                    {
                        // Find index of selected card and claim it
                        for (int i = 0; i < static_cast<int>(cardOptions.size()); i++)
                        {
                            if (cardOptions[i] == selectedCardForDetail)
                            {
                                claimReward(i);
                                showCardDetail = false;
                                selectedCardForDetail = nullptr;
                                cardDetailAlpha = 0.0f;
                                break;
                            }
                        }
                        return;
                    }

                    float popupLeft = popupX;
                    float popupRight = popupX + popupWidth;
                    float popupTop = popupY;
                    float popupBottom = popupY + popupHeight;

                    if (mousePos.x < popupLeft || mousePos.x > popupRight ||
                        mousePos.y < popupTop || mousePos.y > popupBottom)
                    {
                        showCardDetail = false;
                        selectedCardForDetail = nullptr;
                        cardDetailAlpha = 0.0f;
                    }
                    return;
                }

                if (!rewardClaimed && !showCardDetail)
                {
                    float cardWidth = 250.0f;
                    float cardHeight = 350.0f;
                    float spacing = 40.0f;
                    float totalWidth = cardOptions.size() * cardWidth + (cardOptions.size() - 1) * spacing;
                    float startX = (windowSize.x - totalWidth) / 2.0f;
                    float cardY = windowSize.y * 0.40f;

                    for (int i = 0; i < static_cast<int>(cardOptions.size()); i++)
                    {
                        float cardX = startX + i * (cardWidth + spacing);
                        sf::FloatRect cardBounds({cardX, cardY}, {cardWidth, cardHeight});

                        if (cardBounds.contains(mousePos))
                        {
                            showCardDetail = true;
                            selectedCardForDetail = cardOptions[i];
                            cardDetailAlpha = 0.0f;
                            std::cout << "[Treasure] Previewing card: " << selectedCardForDetail->getName() << std::endl;
                            break;
                        }
                    }
                }
                if (rewardClaimed)
                {
                    float buttonW = 300.0f;
                    float buttonH = 60.0f;
                    float buttonX = (windowSize.x - buttonW) / 2.0f;
                    float buttonY = windowSize.y * 0.85f;
                    sf::FloatRect buttonBounds({buttonX, buttonY}, {buttonW, buttonH});

                    if (buttonBounds.contains(mousePos))
                    {
                        std::cout << "[Treasure] Continue - returning to map" << std::endl;
                        game->changeScreen(Game::ScreenType::GAMEPLAY);
                    }
                }
            }
            else if (mousePress->button == sf::Mouse::Button::Right && !rewardClaimed && !showCardDetail)
            {
                float cardWidth = 250.0f;
                float cardHeight = 350.0f;
                float spacing = 40.0f;
                float totalWidth = cardOptions.size() * cardWidth + (cardOptions.size() - 1) * spacing;
                float startX = (windowSize.x - totalWidth) / 2.0f;
                float cardY = windowSize.y * 0.40f;

                for (int i = 0; i < static_cast<int>(cardOptions.size()); i++)
                {
                    float cardX = startX + i * (cardWidth + spacing);
                    sf::FloatRect cardBounds({cardX, cardY}, {cardWidth, cardHeight});

                    if (cardBounds.contains(mousePos))
                    {
                        showCardDetail = true;
                        selectedCardForDetail = cardOptions[i];
                        cardDetailAlpha = 0.0f;
                        std::cout << "[Treasure] Viewing details: " << selectedCardForDetail->getName() << std::endl;
                        break;
                    }
                }
            }
        }
    }
}

// Update logic
void TreasureScreen::update(float deltaTime)
{
    // Fade in
    if (fadeAlpha < 255.0f)
    {
        fadeAlpha += deltaTime * 400.0f;
        if (fadeAlpha > 255.0f)
            fadeAlpha = 255.0f;
    }

    if (showCardDetail && cardDetailAlpha < 255.0f)
    {
        cardDetailAlpha += deltaTime * 600.0f;
        if (cardDetailAlpha > 255.0f)
            cardDetailAlpha = 255.0f;
    }

    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*game->getWindow());
    sf::Vector2f mousePos = game->getWindow()->mapPixelToCoords(mousePixelPos);
    sf::Vector2u windowSize = game->getWindow()->getSize();

    hoveredCardIndex = -1;
    hoveredContinue = false;
    hoveredConfirmButton = false;

    if (showCardDetail)
    {
        float buttonW = 250.0f;
        float buttonH = 60.0f;
        float popupWidth = 500;
        float popupHeight = 600;
        float popupX = (windowSize.x - popupWidth) / 2.0f;
        float popupY = (windowSize.y - popupHeight) / 2.0f;
        float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
        float buttonY = popupY + popupHeight - 90;
        sf::FloatRect confirmBounds({buttonX, buttonY}, {buttonW, buttonH});

        if (confirmBounds.contains(mousePos))
        {
            hoveredConfirmButton = true;
        }
    }

    if (!rewardClaimed && !showCardDetail)
    {
        float cardWidth = 250.0f;
        float cardHeight = 350.0f;
        float spacing = 40.0f;
        float totalWidth = cardOptions.size() * cardWidth + (cardOptions.size() - 1) * spacing;
        float startX = (windowSize.x - totalWidth) / 2.0f;
        float cardY = windowSize.y * 0.40f;

        for (int i = 0; i < static_cast<int>(cardOptions.size()); i++)
        {
            float cardX = startX + i * (cardWidth + spacing);
            sf::FloatRect cardBounds({cardX, cardY}, {cardWidth, cardHeight});

            if (cardBounds.contains(mousePos))
            {
                hoveredCardIndex = i;
                break;
            }
        }
    }

    if (rewardClaimed)
    {
        float buttonW = 300.0f;
        float buttonH = 60.0f;
        float buttonX = (windowSize.x - buttonW) / 2.0f;
        float buttonY = windowSize.y * 0.85f;
        sf::FloatRect buttonBounds({buttonX, buttonY}, {buttonW, buttonH});

        if (buttonBounds.contains(mousePos))
        {
            hoveredContinue = true;
        }
    }
}

// Render the screen
void TreasureScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();
    window.clear(sf::Color(255, 220, 150));

    // Title
    sf::Text titleText(font, "TREASURE FOUND!", 72);
    titleText.setFillColor(sf::Color(180, 120, 0, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 50});
    window.draw(titleText);

    // Subtitle
    sf::Text subtitleText(font, rewardClaimed ? "Card Added to Deck!" : "Choose ONE card to add to your deck", 28);
    subtitleText.setFillColor(sf::Color(100, 70, 0, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect subtitleBounds = subtitleText.getLocalBounds();
    subtitleText.setOrigin({subtitleBounds.size.x / 2.0f, 0});
    subtitleText.setPosition({windowSize.x / 2.0f, 150});
    window.draw(subtitleText);

    // Help text
    if (!rewardClaimed)
    {
        sf::Text helpText(font, "Click card to preview details", 20);
        helpText.setFillColor(sf::Color(100, 70, 0, static_cast<std::uint8_t>(fadeAlpha * 0.8f)));
        sf::FloatRect helpBounds = helpText.getLocalBounds();
        helpText.setOrigin({helpBounds.size.x / 2.0f, 0});
        helpText.setPosition({windowSize.x / 2.0f, 195});
        window.draw(helpText);
    }

    float cardWidth = 250.0f;
    float cardHeight = 350.0f;
    float spacing = 40.0f;
    float totalWidth = cardOptions.size() * cardWidth + (cardOptions.size() - 1) * spacing;
    float startX = (windowSize.x - totalWidth) / 2.0f;
    float cardY = windowSize.y * 0.40f;

    for (int i = 0; i < cardOptions.size(); i++)
    {
        Card *card = cardOptions[i];
        float cardX = startX + i * (cardWidth + spacing);
        bool isHovered = (hoveredCardIndex == i && !rewardClaimed);
        bool isSelected = (showCardDetail && selectedCardForDetail == card);

        sf::RectangleShape cardBg({cardWidth, cardHeight});
        cardBg.setPosition({cardX, cardY});
        cardBg.setFillColor(sf::Color(250, 250, 240, static_cast<std::uint8_t>(fadeAlpha)));
        cardBg.setOutlineThickness(isSelected ? 6.0f : (isHovered ? 5.0f : 3.0f));
        cardBg.setOutlineColor(isSelected ? sf::Color(255, 215, 0, static_cast<std::uint8_t>(fadeAlpha)) : sf::Color(180, 120, 0, static_cast<std::uint8_t>(fadeAlpha)));
        window.draw(cardBg);

        // Selected glow effect (YELLOW)
        if (isSelected)
        {
            sf::RectangleShape glow({cardWidth + 12, cardHeight + 12});
            glow.setPosition({cardX - 6, cardY - 6});
            glow.setFillColor(sf::Color::Transparent);
            glow.setOutlineThickness(8.0f);
            glow.setOutlineColor(sf::Color(255, 215, 0, static_cast<std::uint8_t>(fadeAlpha * 0.8f)));
            window.draw(glow);
        }
        // Hover glow effect
        else if (isHovered)
        {
            sf::RectangleShape glow({cardWidth + 12, cardHeight + 12});
            glow.setPosition({cardX - 6, cardY - 6});
            glow.setFillColor(sf::Color::Transparent);
            glow.setOutlineThickness(6.0f);
            glow.setOutlineColor(sf::Color(255, 200, 0, static_cast<std::uint8_t>(fadeAlpha * 0.7f)));
            window.draw(glow);
        }

        sf::Text questionMark(font, "?", 180);
        questionMark.setFillColor(sf::Color(180, 120, 0, static_cast<std::uint8_t>(fadeAlpha)));
        questionMark.setStyle(sf::Text::Bold);
        sf::FloatRect questionBounds = questionMark.getLocalBounds();
        questionMark.setOrigin({questionBounds.size.x / 2.0f, questionBounds.size.y / 2.0f});
        questionMark.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight / 2.0f});
        window.draw(questionMark);

        sf::Text mysteryText(font, "MYSTERY CARD", 22);
        mysteryText.setFillColor(sf::Color(100, 70, 0, static_cast<std::uint8_t>(fadeAlpha)));
        mysteryText.setStyle(sf::Text::Bold);
        sf::FloatRect mysteryBounds = mysteryText.getLocalBounds();
        mysteryText.setOrigin({mysteryBounds.size.x / 2.0f, 0});
        mysteryText.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight - 50});
        window.draw(mysteryText);

        if (isHovered)
        {
            sf::Text hintText(font, "Click to reveal", 18);
            hintText.setFillColor(sf::Color(255, 200, 0, static_cast<std::uint8_t>(fadeAlpha)));
            sf::FloatRect hintBounds = hintText.getLocalBounds();
            hintText.setOrigin({hintBounds.size.x / 2.0f, 0});
            hintText.setPosition({cardX + cardWidth / 2.0f, cardY + 20});
            window.draw(hintText);
        }
    }

    if (rewardClaimed)
    {
        float buttonW = 300.0f;
        float buttonH = 60.0f;
        float buttonX = (windowSize.x - buttonW) / 2.0f;
        float buttonY = windowSize.y * 0.85f;

        sf::RectangleShape button({buttonW, buttonH});
        button.setPosition({buttonX, buttonY});
        button.setFillColor(hoveredContinue ? sf::Color(150, 100, 0, static_cast<std::uint8_t>(fadeAlpha))
                                            : sf::Color(180, 120, 0, static_cast<std::uint8_t>(fadeAlpha)));
        window.draw(button);

        sf::Text buttonText(font, "Continue", 32);
        buttonText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
        sf::FloatRect buttonTextBounds = buttonText.getLocalBounds();
        buttonText.setOrigin({buttonTextBounds.size.x / 2.0f, buttonTextBounds.size.y / 2.0f});
        buttonText.setPosition({buttonX + buttonW / 2.0f, buttonY + buttonH / 2.0f});
        window.draw(buttonText);
    }

    // Card detail popup (drawn on top)
    if (showCardDetail && selectedCardForDetail)
    {
        drawCardDetailPopup(window, font);
    }
}

// Draw card detail popup
void TreasureScreen::drawCardDetailPopup(sf::RenderWindow &window, sf::Font &font)
{
    if (!showCardDetail || !selectedCardForDetail)
        return;

    sf::Vector2u windowSize = window.getSize();

    // Semi-transparent background overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha * 0.7f)));
    window.draw(overlay);

    // Popup panel (center)
    float popupWidth = 500;
    float popupHeight = 600;
    float popupX = (windowSize.x - popupWidth) / 2.0f;
    float popupY = (windowSize.y - popupHeight) / 2.0f;

    sf::RectangleShape popup({popupWidth, popupHeight});
    popup.setPosition({popupX, popupY});
    popup.setFillColor(sf::Color(240, 240, 230, static_cast<std::uint8_t>(cardDetailAlpha)));
    popup.setOutlineColor(sf::Color(255, 215, 0, static_cast<std::uint8_t>(cardDetailAlpha))); // Yellow border
    popup.setOutlineThickness(6.0f);
    window.draw(popup);

    // Card name
    sf::Text nameText(font, selectedCardForDetail->getName(), 32);
    nameText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
    nameText.setStyle(sf::Text::Bold);
    sf::FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setOrigin({nameBounds.size.x / 2.0f, 0});
    nameText.setPosition({popupX + popupWidth / 2.0f, popupY + 30});
    window.draw(nameText);

    // Card icon (large)
    sf::Sprite *iconSprite = selectedCardForDetail->getIconSprite();
    if (iconSprite)
    {
        const sf::Texture &iconTexture = iconSprite->getTexture();
        sf::Vector2u iconSize = iconTexture.getSize();

        float iconScale = 200.0f / iconSize.x;
        iconSprite->setScale({iconScale, iconScale});
        iconSprite->setPosition({popupX + (popupWidth - 200) / 2.0f, popupY + 90});

        // Set alpha
        sf::Color iconColor = iconSprite->getColor();
        iconColor.a = static_cast<std::uint8_t>(cardDetailAlpha);
        iconSprite->setColor(iconColor);

        window.draw(*iconSprite);

        // Reset color
        iconColor.a = 255;
        iconSprite->setColor(iconColor);
    }

    float yPos = popupY + 310;

    // Cost
    sf::Text costText(font, "Cost: " + std::to_string(selectedCardForDetail->getEnergyCost()) + " Energy", 24);
    costText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
    costText.setPosition({popupX + 50, yPos});
    window.draw(costText);
    yPos += 40;

    // Type
    std::string typeStr = "";
    switch (selectedCardForDetail->getType())
    {
    case Card::CardType::ATTACK:
        typeStr = "Attack";
        break;
    case Card::CardType::DEFENSE:
        typeStr = "Defense";
        break;
    case Card::CardType::SKILL:
        typeStr = "Skill";
        break;
    case Card::CardType::POWER:
        typeStr = "Power";
        break;
    }

    sf::Text typeText(font, "Type: " + typeStr, 24);
    typeText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
    typeText.setPosition({popupX + 50, yPos});
    window.draw(typeText);
    yPos += 40;

    // Rarity
    std::string rarityStr = "";
    sf::Color rarityColor = sf::Color::Black;
    switch (selectedCardForDetail->getRarity())
    {
    case Card::CardRarity::COMMON:
        rarityStr = "Common";
        rarityColor = sf::Color(120, 120, 120);
        break;
    case Card::CardRarity::UNCOMMON:
        rarityStr = "Uncommon";
        rarityColor = sf::Color(0, 180, 0);
        break;
    case Card::CardRarity::RARE:
        rarityStr = "Rare";
        rarityColor = sf::Color(0, 100, 255);
        break;
    case Card::CardRarity::EPIC:
        rarityStr = "Epic";
        rarityColor = sf::Color(160, 0, 200);
        break;
    case Card::CardRarity::LEGENDARY:
        rarityStr = "LEGENDARY";
        rarityColor = sf::Color(255, 165, 0);
        break;
    }

    sf::Text rarityText(font, "Rarity: " + rarityStr, 24);
    rarityColor.a = static_cast<std::uint8_t>(cardDetailAlpha);
    rarityText.setFillColor(rarityColor);
    rarityText.setStyle(sf::Text::Bold);
    rarityText.setPosition({popupX + 50, yPos});
    window.draw(rarityText);
    yPos += 60;

    // Description title
    sf::Text descTitle(font, "Description:", 20);
    descTitle.setFillColor(sf::Color(80, 80, 80, static_cast<std::uint8_t>(cardDetailAlpha)));
    descTitle.setStyle(sf::Text::Bold);
    descTitle.setPosition({popupX + 50, yPos});
    window.draw(descTitle);
    yPos += 35;

    // Description text
    sf::Text descText(font, selectedCardForDetail->getDescription(), 18);
    descText.setFillColor(sf::Color(60, 60, 60, static_cast<std::uint8_t>(cardDetailAlpha)));
    descText.setPosition({popupX + 50, yPos});
    window.draw(descText);

    // Confirm button
    float buttonW = 250.0f;
    float buttonH = 60.0f;
    float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
    float buttonY = popupY + popupHeight - 90;

    sf::RectangleShape confirmButton({buttonW, buttonH});
    confirmButton.setPosition({buttonX, buttonY});

    if (hoveredConfirmButton)
    {
        confirmButton.setFillColor(sf::Color(0, 200, 0, static_cast<std::uint8_t>(cardDetailAlpha))); // Bright green
    }
    else
    {
        confirmButton.setFillColor(sf::Color(0, 150, 0, static_cast<std::uint8_t>(cardDetailAlpha))); // Green
    }
    confirmButton.setOutlineColor(sf::Color(0, 100, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
    confirmButton.setOutlineThickness(3.0f);
    window.draw(confirmButton);

    sf::Text confirmText(font, "CONFIRM SELECTION", 24);
    confirmText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(cardDetailAlpha)));
    confirmText.setStyle(sf::Text::Bold);
    sf::FloatRect confirmBounds = confirmText.getLocalBounds();
    confirmText.setOrigin({confirmBounds.size.x / 2.0f, confirmBounds.size.y / 2.0f});
    confirmText.setPosition({buttonX + buttonW / 2.0f, buttonY + buttonH / 2.0f});
    window.draw(confirmText);

    // Close instruction
    sf::Text closeText(font, "Click outside to close", 16);
    closeText.setFillColor(sf::Color(120, 120, 120, static_cast<std::uint8_t>(cardDetailAlpha)));
    sf::FloatRect closeBounds = closeText.getLocalBounds();
    closeText.setOrigin({closeBounds.size.x / 2.0f, 0});
    closeText.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 25});
    window.draw(closeText);
}
