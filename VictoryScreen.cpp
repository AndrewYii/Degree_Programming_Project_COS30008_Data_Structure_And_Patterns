#include "VictoryScreen.h"
#include "Game.h"
#include "Player.h"
#include "CardFactory.h"
#include "AIStoryteller.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <sstream>

// Constructor
VictoryScreen::VictoryScreen(Game *g) : Screen(g), game(g), playerRef(nullptr), bitcoinReward(0), selectedCardIndex(-1), rewardsClaimed(false), hoveredContinue(false), hoveredCardIndex(-1), showCardDetail(false), selectedCardForDetail(-1), cardDetailAlpha(0.0f), hoveredConfirmButton(false), fadeAlpha(0.0f)
{
}

// Destructor
VictoryScreen::~VictoryScreen()
{
    cardOptions.clear();
}

// Navigate to this screen
void VictoryScreen::onEnter()
{
    std::cout << "[VictoryScreen] Entered - generating rewards..." << std::endl;
    playerRef = &game->getPlayer();
    fadeAlpha = 0.0f;
    rewardsClaimed = false;
    selectedCardIndex = -1;
    generateRewards();
}

// Exit this screen
void VictoryScreen::onExit()
{
    std::cout << "[VictoryScreen] Exiting" << std::endl;
    // Clean up unclaimed cards
    for (Card *card : cardOptions)
    {
        delete card;
    }
    cardOptions.clear();
}

void VictoryScreen::generateRewards()
{
    AIStoryteller *ai = AIStoryteller::getInstance();

    // Bitcoin reward (30-60 BTC) adjusted by AI
    int baseBitcoin = 30 + (rand() % 31);
    bitcoinReward = ai->getAdjustedBitcoinReward(baseBitcoin);
    playerRef->addGold(bitcoinReward);

    std::cout << "[Victory] Bitcoin reward: " << bitcoinReward << " BTC (base: " << baseBitcoin
              << ", multiplier: " << ai->getLootMultiplier() << ")" << std::endl;

    // Generate card options - AI can modify count and rarity
    cardOptions.clear();
    CardFactory *factory = CardFactory::getInstance();

    // Base: Get 1 common, 1 uncommon, 1 rare
    Card *common = factory->getRandomCard(Card::CardRarity::COMMON);
    Card *uncommon = factory->getRandomCard(Card::CardRarity::UNCOMMON);
    Card *rare = factory->getRandomCard(Card::CardRarity::RARE);

    // AI can upgrade cards if player struggling
    if (ai->shouldUpgradeReward() && uncommon)
    {
        uncommon->upgrade();
        std::cout << "[Victory] AI upgraded uncommon card!" << std::endl;
    }

    if (common)
    {
        common->loadIcon(); // Load icon sprite
        cardOptions.push_back(common);
    }
    if (uncommon)
    {
        uncommon->loadIcon(); // Load icon sprite
        cardOptions.push_back(uncommon);
    }
    if (rare)
    {
        rare->loadIcon(); // Load icon sprite
        cardOptions.push_back(rare);
    }

    // AI can grant bonus card if player doing poorly
    if (ai->shouldGrantBonusCard())
    {
        Card *bonusCard = factory->getRandomCard(Card::CardRarity::RARE);
        if (bonusCard)
        {
            bonusCard->loadIcon(); // Load icon sprite
            cardOptions.push_back(bonusCard);
            std::cout << "[Victory] AI granted BONUS card!" << std::endl;
        }
    }

    std::cout << "[Victory] Generated " << cardOptions.size() << " card options (AI influenced)" << std::endl;
}

// Claim the selected reward
void VictoryScreen::claimReward(int cardIndex)
{
    if (cardIndex >= 0 && cardIndex < cardOptions.size())
    {
        Card *chosenCard = cardOptions[cardIndex];
        playerRef->addCardToDeck(chosenCard->clone());

        bool isRare = (chosenCard->getRarity() == Card::CardRarity::RARE || chosenCard->getRarity() == Card::CardRarity::EPIC);
        AIStoryteller::getInstance()->onCardAcquired(isRare);

        std::cout << "[Victory] Player chose: " << chosenCard->getName() << std::endl;
        rewardsClaimed = true;
    }
}

// Event handling
void VictoryScreen::handleEvents()
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

                // If card detail popup is open, check confirm button
                if (showCardDetail)
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
                        std::cout << "[Victory] Confirm button clicked - claiming reward" << std::endl;
                        claimReward(selectedCardForDetail);
                        showCardDetail = false;
                        cardDetailAlpha = 0.0f;
                    }
                    else
                    {
                        // Check if clicked outside popup to close
                        sf::FloatRect popupBounds({popupX, popupY}, {popupWidth, popupHeight});
                        if (!popupBounds.contains(mousePos))
                        {
                            std::cout << "[Victory] Clicked outside card detail - closing" << std::endl;
                            showCardDetail = false;
                            cardDetailAlpha = 0.0f;
                        }
                    }
                }
                // Check card selection to show preview
                else if (!rewardsClaimed)
                {
                    float cardWidth = 180.0f;
                    float cardHeight = 250.0f;
                    float spacing = 30.0f;
                    float totalWidth = cardOptions.size() * cardWidth + (cardOptions.size() - 1) * spacing;
                    float startX = (windowSize.x - totalWidth) / 2.0f;
                    float cardY = windowSize.y * 0.45f;

                    for (int i = 0; i < cardOptions.size(); i++)
                    {
                        float cardX = startX + i * (cardWidth + spacing);
                        sf::FloatRect cardBounds({cardX, cardY}, {cardWidth, cardHeight});

                        if (cardBounds.contains(mousePos))
                        {
                            std::cout << "[Victory] Card " << i << " clicked - showing detail" << std::endl;
                            selectedCardForDetail = i;
                            showCardDetail = true;
                            cardDetailAlpha = 0.0f;
                            break;
                        }
                    }
                }
                // Check continue button (only after reward claimed)
                else if (rewardsClaimed)
                {
                    float buttonW = 300.0f;
                    float buttonH = 60.0f;
                    float buttonX = (windowSize.x - buttonW) / 2.0f;
                    float buttonY = windowSize.y * 0.85f;
                    sf::FloatRect buttonBounds({buttonX, buttonY}, {buttonW, buttonH});

                    if (buttonBounds.contains(mousePos))
                    {
                        std::cout << "[Victory] Continue - returning to map" << std::endl;
                        game->changeScreen(Game::ScreenType::GAMEPLAY);
                    }
                }
            }
        }
    }
}

// Update logic
void VictoryScreen::update(float deltaTime)
{
    // Fade in
    if (fadeAlpha < 255.0f)
    {
        fadeAlpha += deltaTime * 400.0f;
        if (fadeAlpha > 255.0f)
            fadeAlpha = 255.0f;
    }

    // Fade in card detail popup
    if (showCardDetail && cardDetailAlpha < 255.0f)
    {
        cardDetailAlpha += deltaTime * 500.0f;
        if (cardDetailAlpha > 255.0f)
            cardDetailAlpha = 255.0f;
    }

    // Update hover states
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*game->getWindow());
    sf::Vector2f mousePos = game->getWindow()->mapPixelToCoords(mousePixelPos);
    sf::Vector2u windowSize = game->getWindow()->getSize();

    hoveredCardIndex = -1;
    hoveredContinue = false;
    hoveredConfirmButton = false;

    // If card detail is showing, only check confirm button hover
    if (showCardDetail)
    {
        float popupWidth = 500.0f;
        float popupHeight = 600.0f;
        float popupX = (windowSize.x - popupWidth) / 2.0f;
        float popupY = (windowSize.y - popupHeight) / 2.0f;

        float buttonW = 300.0f;
        float buttonH = 50.0f;
        float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
        float buttonY = popupY + popupHeight - 70.0f;
        sf::FloatRect confirmBounds({buttonX, buttonY}, {buttonW, buttonH});

        if (confirmBounds.contains(mousePos))
        {
            hoveredConfirmButton = true;
        }
        return; // Don't check other hovers when popup is open
    }

    if (!rewardsClaimed)
    {
        float cardWidth = 180.0f;
        float cardHeight = 250.0f;
        float spacing = 30.0f;
        float totalWidth = cardOptions.size() * cardWidth + (cardOptions.size() - 1) * spacing;
        float startX = (windowSize.x - totalWidth) / 2.0f;
        float cardY = windowSize.y * 0.45f;

        for (int i = 0; i < cardOptions.size(); i++)
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

    if (rewardsClaimed)
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

// Render the victory screen
void VictoryScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    // Background
    window.clear(sf::Color(240, 240, 230));

    // Title
    sf::Text titleText(font, "VICTORY!", 72);
    titleText.setFillColor(sf::Color(0, 150, 0, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setPosition({(windowSize.x - titleBounds.size.x) / 2, windowSize.y * 0.1f});
    window.draw(titleText);

    // Bitcoin reward
    std::string bitcoinText = "+" + std::to_string(bitcoinReward) + " BTC";
    sf::Text bitcoinLabel(font, bitcoinText, 36);
    bitcoinLabel.setFillColor(sf::Color(255, 165, 0, static_cast<std::uint8_t>(fadeAlpha)));
    bitcoinLabel.setStyle(sf::Text::Bold);
    sf::FloatRect bitcoinBounds = bitcoinLabel.getLocalBounds();
    bitcoinLabel.setPosition({(windowSize.x - bitcoinBounds.size.x) / 2, windowSize.y * 0.22f});
    window.draw(bitcoinLabel);

    // Instruction
    std::string instructionText = rewardsClaimed ? "Card added to deck!" : "Choose a card to add to your deck:";
    sf::Text instruction(font, instructionText, 28);
    instruction.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect instrBounds = instruction.getLocalBounds();
    instruction.setPosition({(windowSize.x - instrBounds.size.x) / 2, windowSize.y * 0.35f});
    window.draw(instruction);

    // Draw card options
    if (!rewardsClaimed)
    {
        float cardWidth = 180.0f;
        float cardHeight = 250.0f;
        float spacing = 30.0f;
        float totalWidth = cardOptions.size() * cardWidth + (cardOptions.size() - 1) * spacing;
        float startX = (windowSize.x - totalWidth) / 2.0f;
        float cardY = windowSize.y * 0.45f;

        for (int i = 0; i < cardOptions.size(); i++)
        {
            float cardX = startX + i * (cardWidth + spacing);
            bool hovered = (i == hoveredCardIndex);

            // Card background
            sf::RectangleShape cardBg({cardWidth, cardHeight});
            cardBg.setPosition({cardX, cardY});
            cardBg.setFillColor(hovered ? sf::Color(255, 255, 200, static_cast<std::uint8_t>(fadeAlpha))
                                        : sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
            cardBg.setOutlineColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
            cardBg.setOutlineThickness(3.0f);
            window.draw(cardBg);

            // MYSTERY CARD - Show "?" only
            // Giant question mark
            sf::Text questionMark(font, "?", 120);
            questionMark.setFillColor(sf::Color(200, 150, 0, static_cast<std::uint8_t>(fadeAlpha)));
            questionMark.setStyle(sf::Text::Bold);
            sf::FloatRect questionBounds = questionMark.getLocalBounds();
            questionMark.setOrigin({questionBounds.size.x / 2.0f, questionBounds.size.y / 2.0f});
            questionMark.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight / 2.0f});
            window.draw(questionMark);

            // Reward hint text
            sf::Text rewardText(font, "REWARD", 18);
            rewardText.setFillColor(sf::Color(150, 100, 0, static_cast<std::uint8_t>(fadeAlpha)));
            rewardText.setStyle(sf::Text::Bold);
            sf::FloatRect rewardBounds = rewardText.getLocalBounds();
            rewardText.setOrigin({rewardBounds.size.x / 2.0f, 0});
            rewardText.setPosition({cardX + cardWidth / 2.0f, cardY + 20});
            window.draw(rewardText);

            // Click to view hint (on hover)
            if (hoveredCardIndex == i)
            {
                sf::Text hintText(font, "Click to reveal", 16);
                hintText.setFillColor(sf::Color(255, 200, 0, static_cast<std::uint8_t>(fadeAlpha)));
                sf::FloatRect hintBounds = hintText.getLocalBounds();
                hintText.setOrigin({hintBounds.size.x / 2.0f, 0});
                hintText.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight - 30});
                window.draw(hintText);
            }
        }
    }

    // Continue button (only after reward claimed)
    if (rewardsClaimed)
    {
        float buttonW = 300.0f;
        float buttonH = 60.0f;
        float buttonX = (windowSize.x - buttonW) / 2.0f;
        float buttonY = windowSize.y * 0.85f;

        sf::RectangleShape button({buttonW, buttonH});
        button.setPosition({buttonX, buttonY});
        button.setFillColor(hoveredContinue ? sf::Color(100, 200, 100, static_cast<std::uint8_t>(fadeAlpha))
                                            : sf::Color(80, 180, 80, static_cast<std::uint8_t>(fadeAlpha)));
        button.setOutlineColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
        button.setOutlineThickness(3.0f);
        window.draw(button);

        sf::Text buttonText(font, "Continue", 32);
        buttonText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
        buttonText.setStyle(sf::Text::Bold);
        sf::FloatRect textBounds = buttonText.getLocalBounds();
        buttonText.setPosition({buttonX + (buttonW - textBounds.size.x) / 2, buttonY + (buttonH - textBounds.size.y) / 2 - 5});
        window.draw(buttonText);
    }

    // Draw card detail popup if active
    if (showCardDetail)
    {
        drawCardDetailPopup();
    }
}

// Draw the card detail popup
void VictoryScreen::drawCardDetailPopup()
{
    sf::RenderWindow *window = game->getWindow();
    sf::Vector2u windowSize = window->getSize();
    Card *card = cardOptions[selectedCardForDetail];

    // Dark overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha * 0.7f)));
    window->draw(overlay);

    // Popup box
    float popupWidth = 500.0f;
    float popupHeight = 600.0f;
    float popupX = (windowSize.x - popupWidth) / 2.0f;
    float popupY = (windowSize.y - popupHeight) / 2.0f;

    sf::RectangleShape popup({popupWidth, popupHeight});
    popup.setPosition({popupX, popupY});
    popup.setFillColor(sf::Color(250, 240, 220, static_cast<std::uint8_t>(cardDetailAlpha)));
    popup.setOutlineColor(sf::Color(150, 100, 50, static_cast<std::uint8_t>(cardDetailAlpha)));
    popup.setOutlineThickness(5.0f);
    window->draw(popup);

    sf::Font &font = game->getFont();

    // Card name
    sf::Text nameText(font, card->getName(), 36);
    nameText.setFillColor(sf::Color(20, 20, 20, static_cast<std::uint8_t>(cardDetailAlpha)));
    nameText.setStyle(sf::Text::Bold);
    sf::FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setPosition({popupX + (popupWidth - nameBounds.size.x) / 2, popupY + 20});
    window->draw(nameText);

    sf::Sprite *iconSprite = card->getIconSprite();
    if (iconSprite)
    {
        const sf::Texture &iconTexture = iconSprite->getTexture();
        sf::Vector2u iconSize = iconTexture.getSize();

        float iconScale = 200.0f / iconSize.x;
        iconSprite->setScale({iconScale, iconScale});
        iconSprite->setPosition({popupX + (popupWidth - 200) / 2.0f, popupY + 80});

        sf::Color iconColor = iconSprite->getColor();
        iconColor.a = static_cast<std::uint8_t>(cardDetailAlpha);
        iconSprite->setColor(iconColor);

        window->draw(*iconSprite);

        iconColor.a = 255;
        iconSprite->setColor(iconColor);
    }

    std::string typeStr;
    sf::Color typeColor;
    Card::CardType cardType = card->getType();

    switch (cardType)
    {
    case Card::CardType::ATTACK:
        typeStr = "ATTACK";
        typeColor = sf::Color(220, 50, 50, static_cast<std::uint8_t>(cardDetailAlpha));
        break;
    case Card::CardType::SKILL:
        typeStr = "SKILL";
        typeColor = sf::Color(50, 150, 220, static_cast<std::uint8_t>(cardDetailAlpha));
        break;
    case Card::CardType::POWER:
        typeStr = "POWER";
        typeColor = sf::Color(100, 200, 100, static_cast<std::uint8_t>(cardDetailAlpha));
        break;
    }

    sf::Text typeText(font, typeStr, 24);
    typeText.setFillColor(typeColor);
    typeText.setStyle(sf::Text::Bold);
    sf::FloatRect typeBounds = typeText.getLocalBounds();
    typeText.setPosition({popupX + (popupWidth - typeBounds.size.x) / 2, popupY + 300});
    window->draw(typeText);

    sf::Text costText(font, "CPU Cost: " + std::to_string(card->getEnergyCost()), 22);
    costText.setFillColor(sf::Color(60, 60, 60, static_cast<std::uint8_t>(cardDetailAlpha)));
    costText.setPosition({popupX + 30, popupY + 340});
    window->draw(costText);

    // Description with word wrapping
    std::string desc = card->getDescription();
    std::vector<std::string> wrappedLines;
    float maxWidth = popupWidth - 60;
    float descFontSize = 18.0f;
    std::string currentLine = "";
    std::istringstream descStream(desc);
    std::string word;

    while (descStream >> word)
    {
        sf::Text testText(font, currentLine + word + " ", static_cast<unsigned int>(descFontSize));
        if (testText.getLocalBounds().size.x > maxWidth)
        {
            if (!currentLine.empty())
            {
                wrappedLines.push_back(currentLine);
                currentLine = word + " ";
            }
            else
            {
                wrappedLines.push_back(word + " ");
                currentLine = "";
            }
        }
        else
        {
            currentLine += word + " ";
        }
    }
    if (!currentLine.empty())
    {
        wrappedLines.push_back(currentLine);
    }

    float lineSpacing = descFontSize * 1.4f;
    float currentY = popupY + 380;
    for (const std::string &line : wrappedLines)
    {
        sf::Text lineText(font, line, static_cast<unsigned int>(descFontSize));
        lineText.setFillColor(sf::Color(40, 40, 40, static_cast<std::uint8_t>(cardDetailAlpha)));
        lineText.setPosition({popupX + 30, currentY});
        window->draw(lineText);
        currentY += lineSpacing;
    }

    // Confirm button (VICTORY GOLD color)
    float buttonW = 300.0f;
    float buttonH = 50.0f;
    float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
    float buttonY = popupY + popupHeight - 70.0f;

    sf::RectangleShape confirmButton({buttonW, buttonH});
    confirmButton.setPosition({buttonX, buttonY});
    confirmButton.setFillColor(hoveredConfirmButton ? sf::Color(255, 215, 50, static_cast<std::uint8_t>(cardDetailAlpha))
                                                    : sf::Color(220, 180, 30, static_cast<std::uint8_t>(cardDetailAlpha)));
    confirmButton.setOutlineColor(sf::Color(150, 100, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
    confirmButton.setOutlineThickness(3.0f);
    window->draw(confirmButton);

    sf::Text confirmText(font, "CLAIM REWARD", 28);
    confirmText.setFillColor(sf::Color(20, 20, 20, static_cast<std::uint8_t>(cardDetailAlpha)));
    confirmText.setStyle(sf::Text::Bold);
    sf::FloatRect confirmBounds = confirmText.getLocalBounds();
    confirmText.setPosition({buttonX + (buttonW - confirmBounds.size.x) / 2, buttonY + (buttonH - confirmBounds.size.y) / 2 - 5});
    window->draw(confirmText);
}
