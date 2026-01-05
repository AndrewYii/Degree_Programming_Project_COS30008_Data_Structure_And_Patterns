#include "TutorialScreen.h"
#include "Game.h"
#include "Card.h"
#include "CardFactory.h"
#include <iostream>

// Constructor
TutorialScreen::TutorialScreen(Game *g): Screen(g), game(g), currentStage(TutorialStage::WELCOME),fadeAlpha(0.0f), stageTimer(0.0f), hoveredNext(false),hoveredCardIndex(-1), hoveredActiveIndex(-1), demoPlayerHP(80), demoPlayerMaxHP(80), demoPlayerMemory(3), demoPlayerMaxMemory(3), demoPlayerBlock(0), demoEnemyHP(30), demoEnemyMaxHP(30), animationTimer(0.0f), animationPlaying(false)
{
}

// Destructor
TutorialScreen::~TutorialScreen()
{
    clearDemoCards();
}

// Navigate to this screen
void TutorialScreen::onEnter()
{
    std::cout << "[TutorialScreen] Starting interactive tutorial..." << std::endl;
    currentStage = TutorialStage::WELCOME;
    fadeAlpha = 0.0f;
    stageTimer = 0.0f;
    setupDemoCards();
}

// Exit from this screen
void TutorialScreen::onExit()
{
    std::cout << "[TutorialScreen] Tutorial completed!" << std::endl;
    clearDemoCards();
}

// Setup demo cards for tutorial
void TutorialScreen::setupDemoCards()
{
    clearDemoCards();
    CardFactory *factory = CardFactory::getInstance();
    Card *attack = factory->getRandomCard(Card::CardRarity::COMMON);
    Card *defense = factory->getRandomCard(Card::CardRarity::COMMON);
    Card *skill = factory->getRandomCard(Card::CardRarity::COMMON);

    if (attack)
        demoCards.push_back(attack);
    if (defense)
        demoCards.push_back(defense);
    if (skill)
        demoCards.push_back(skill);
}

// Clear demo cards
void TutorialScreen::clearDemoCards()
{
    demoCards.clear();
    activeBar.clear();
}

// Advance to next tutorial stage
void TutorialScreen::advanceStage()
{
    int stageInt = static_cast<int>(currentStage);
    stageInt++;

    if (stageInt >= static_cast<int>(TutorialStage::COMPLETE))
    {
        std::cout << "[Tutorial] Complete! Returning to gameplay..." << std::endl;
        game->changeScreen(Game::ScreenType::GAMEPLAY);
    }
    else
    {
        currentStage = static_cast<TutorialStage>(stageInt);
        stageTimer = 0.0f;
        animationPlaying = false;
        animationTimer = 0.0f;
        std::cout << "[Tutorial] Advanced to stage " << stageInt << std::endl;
    }
}

// Handle input events
void TutorialScreen::handleEvents()
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

                // Check Next button
                float buttonW = 250.0f;
                float buttonH = 60.0f;
                float buttonX = windowSize.x - buttonW - 50;
                float buttonY = windowSize.y - buttonH - 50;
                sf::FloatRect buttonBounds({buttonX, buttonY}, {buttonW, buttonH});

                if (buttonBounds.contains(mousePos))
                {
                    advanceStage();
                }

                // Interactive demo in DEMONSTRATE_COMBAT stage
                if (currentStage == TutorialStage::DEMONSTRATE_COMBAT && !animationPlaying)
                {
                    // Check if clicked on demo card
                    float cardWidth = 120.0f;
                    float cardHeight = 160.0f;
                    float cardSpacing = 15.0f;
                    float handY = windowSize.y * 0.75f;
                    float totalHandWidth = demoCards.size() * cardWidth + (demoCards.size() - 1) * cardSpacing;
                    float handStartX = (windowSize.x - totalHandWidth) / 2.0f;

                    for (int i = 0; i < static_cast<int>(demoCards.size()); i++)
                    {
                        float cardX = handStartX + i * (cardWidth + cardSpacing);
                        sf::FloatRect cardBounds({cardX, handY}, {cardWidth, cardHeight});

                        if (cardBounds.contains(mousePos))
                        {
                            // Simulate playing the card
                            simulateCombatAction();
                            break;
                        }
                    }
                }
            }
        }
    }
}

// Simulate a combat action in the demo
void TutorialScreen::simulateCombatAction()
{
    if (demoCards.empty())
        return;

    // Play the first card as demo
    Card *card = demoCards[0];
    animationPlaying = true;
    animationTimer = 0.0f;

    // Simulate damage/block
    if (card->getType() == Card::CardType::ATTACK)
    {
        int damage = 8;
        demoEnemyHP -= damage;
        if (demoEnemyHP < 0)
            demoEnemyHP = 0;
        animationMessage = "Dealt " + std::to_string(damage) + " damage!";
    }
    else if (card->getType() == Card::CardType::DEFENSE)
    {
        int block = 5;
        demoPlayerBlock += block;
        animationMessage = "Gained " + std::to_string(block) + " block!";
    }
    else
    {
        animationMessage = "Card played!";
    }

    demoPlayerMemory -= card->getEnergyCost();
    if (demoPlayerMemory < 0)
        demoPlayerMemory = 0;

    std::cout << "[Tutorial] Demo action: " << animationMessage << std::endl;
}

void TutorialScreen::update(float deltaTime)
{
    // Fade in
    if (fadeAlpha < 255.0f)
    {
        fadeAlpha += deltaTime * 400.0f;
        if (fadeAlpha > 255.0f)
            fadeAlpha = 255.0f;
    }

    stageTimer += deltaTime;

    // Animation timer
    if (animationPlaying)
    {
        animationTimer += deltaTime;
        if (animationTimer > 2.0f)
        {
            animationPlaying = false;
            animationTimer = 0.0f;
        }
    }

    // Update hover states
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*game->getWindow());
    sf::Vector2f mousePos = game->getWindow()->mapPixelToCoords(mousePixelPos);
    sf::Vector2u windowSize = game->getWindow()->getSize();

    // Check Next button hover
    float buttonW = 250.0f;
    float buttonH = 60.0f;
    float buttonX = windowSize.x - buttonW - 50;
    float buttonY = windowSize.y - buttonH - 50;
    sf::FloatRect buttonBounds({buttonX, buttonY}, {buttonW, buttonH});
    hoveredNext = buttonBounds.contains(mousePos);
}

// Render the tutorial screen
void TutorialScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    window.clear(sf::Color(230, 230, 250));

    // Render current stage
    switch (currentStage)
    {
    case TutorialStage::WELCOME:
        renderWelcome(window, font);
        break;
    case TutorialStage::EXPLAIN_GOAL:
        renderExplainGoal(window, font);
        break;
    case TutorialStage::EXPLAIN_CARDS:
        renderExplainCards(window, font);
        break;
    case TutorialStage::EXPLAIN_ENERGY:
        renderExplainEnergy(window, font);
        break;
    case TutorialStage::EXPLAIN_ACTIVE_BAR:
        renderExplainActiveBar(window, font);
        break;
    case TutorialStage::DEMONSTRATE_COMBAT:
        renderDemonstrateCombat(window, font);
        break;
    case TutorialStage::EXPLAIN_ROOMS:
        renderExplainRooms(window, font);
        break;
    default:
        break;
    }

    // Always render Next button
    renderNextButton(window, font);
}

// Render the welcome stage
void TutorialScreen::renderWelcome(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    // Title
    sf::Text titleText(font, "WELCOME TO CODE COMBAT!", 60);
    titleText.setFillColor(sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 100});
    window.draw(titleText);

    // Message
    sf::Text msgText(font, "Let's learn how to play!", 32);
    msgText.setFillColor(sf::Color(100, 70, 170, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect msgBounds = msgText.getLocalBounds();
    msgText.setOrigin({msgBounds.size.x / 2.0f, 0});
    msgText.setPosition({windowSize.x / 2.0f, 250});
    window.draw(msgText);

    // Animated pulse
    float pulse = std::sin(stageTimer * 3.0f) * 0.3f + 0.7f;
    sf::Text pulseText(font, "Click NEXT to continue", 28);
    pulseText.setFillColor(sf::Color(150, 100, 200, static_cast<std::uint8_t>(fadeAlpha * pulse)));
    sf::FloatRect pulseBounds = pulseText.getLocalBounds();
    pulseText.setOrigin({pulseBounds.size.x / 2.0f, 0});
    pulseText.setPosition({windowSize.x / 2.0f, 400});
    window.draw(pulseText);
}

// Render the explain goal stage
void TutorialScreen::renderExplainGoal(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    sf::Text titleText(font, "YOUR MISSION", 50);
    titleText.setFillColor(sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 80});
    window.draw(titleText);

    sf::Text line1(font, "Navigate through rooms filled with BUGS", 28);
    line1.setFillColor(sf::Color(60, 40, 120, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect line1Bounds = line1.getLocalBounds();
    line1.setOrigin({line1Bounds.size.x / 2.0f, 0});
    line1.setPosition({windowSize.x / 2.0f, 200});
    window.draw(line1);

    sf::Text line2(font, "Defeat enemies using your CARD DECK", 28);
    line2.setFillColor(sf::Color(60, 40, 120, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect line2Bounds = line2.getLocalBounds();
    line2.setOrigin({line2Bounds.size.x / 2.0f, 0});
    line2.setPosition({windowSize.x / 2.0f, 280});
    window.draw(line2);

    sf::Text line3(font, "Reach the EXIT to win!", 28);
    line3.setFillColor(sf::Color(60, 40, 120, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect line3Bounds = line3.getLocalBounds();
    line3.setOrigin({line3Bounds.size.x / 2.0f, 0});
    line3.setPosition({windowSize.x / 2.0f, 360});
    window.draw(line3);
}

// Render the explain cards stage
void TutorialScreen::renderExplainCards(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    sf::Text titleText(font, "CARD TYPES", 50);
    titleText.setFillColor(sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 60});
    window.draw(titleText);

    float yPos = 180;
    float spacing = 80;

    // ATTACK
    sf::Text attack(font, "ATTACK - Deal damage to enemies", 26);
    attack.setFillColor(sf::Color(255, 80, 80, static_cast<std::uint8_t>(fadeAlpha)));
    attack.setStyle(sf::Text::Bold);
    attack.setPosition({100, yPos});
    window.draw(attack);

    // DEFENSE
    sf::Text defense(font, "DEFENSE - Gain block to protect yourself", 26);
    defense.setFillColor(sf::Color(80, 150, 255, static_cast<std::uint8_t>(fadeAlpha)));
    defense.setStyle(sf::Text::Bold);
    defense.setPosition({100, yPos + spacing});
    window.draw(defense);

    // SKILL
    sf::Text skill(font, "SKILL - Special effects and abilities", 26);
    skill.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
    skill.setStyle(sf::Text::Bold);
    skill.setPosition({100, yPos + spacing * 2});
    window.draw(skill);

    // POWER
    sf::Text power(font, "POWER - Permanent buffs and enhancements", 26);
    power.setFillColor(sf::Color(255, 0, 255, static_cast<std::uint8_t>(fadeAlpha)));
    power.setStyle(sf::Text::Bold);
    power.setPosition({100, yPos + spacing * 3});
    window.draw(power);
}

// Render the explain energy stage
void TutorialScreen::renderExplainEnergy(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    sf::Text titleText(font, "MEMORY SYSTEM", 50);
    titleText.setFillColor(sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 80});
    window.draw(titleText);

    // Animated memory icon
    float pulse = std::sin(stageTimer * 4.0f) * 10.0f;
    sf::CircleShape memoryOrb(60 + pulse);
    memoryOrb.setFillColor(sf::Color(100, 150, 255, static_cast<std::uint8_t>(fadeAlpha * 0.7f)));
    memoryOrb.setOrigin({memoryOrb.getRadius(), memoryOrb.getRadius()});
    memoryOrb.setPosition({windowSize.x / 2.0f, 250});
    window.draw(memoryOrb);

    sf::Text memoryText(font, "3/3", 40);
    memoryText.setFillColor(sf::Color::White);
    memoryText.setStyle(sf::Text::Bold);
    sf::FloatRect memBounds = memoryText.getLocalBounds();
    memoryText.setOrigin({memBounds.size.x / 2.0f, memBounds.size.y / 2.0f});
    memoryText.setPosition({windowSize.x / 2.0f, 250});
    window.draw(memoryText);

    sf::Text line1(font, "Each card costs MEMORY to play", 28);
    line1.setFillColor(sf::Color(60, 40, 120, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect line1Bounds = line1.getLocalBounds();
    line1.setOrigin({line1Bounds.size.x / 2.0f, 0});
    line1.setPosition({windowSize.x / 2.0f, 380});
    window.draw(line1);

    sf::Text line2(font, "Memory refills each turn", 28);
    line2.setFillColor(sf::Color(60, 40, 120, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect line2Bounds = line2.getLocalBounds();
    line2.setOrigin({line2Bounds.size.x / 2.0f, 0});
    line2.setPosition({windowSize.x / 2.0f, 450});
    window.draw(line2);
}

// Render the explain active bar stage
void TutorialScreen::renderExplainActiveBar(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    sf::Text titleText(font, "ACTIVE BAR", 50);
    titleText.setFillColor(sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 60});
    window.draw(titleText);

    sf::Text line1(font, "Cards are played to the ACTIVE BAR", 28);
    line1.setFillColor(sf::Color(60, 40, 120, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect line1Bounds = line1.getLocalBounds();
    line1.setOrigin({line1Bounds.size.x / 2.0f, 0});
    line1.setPosition({windowSize.x / 2.0f, 180});
    window.draw(line1);

    sf::Text line2(font, "Cards execute in order: LEFT to RIGHT", 28);
    line2.setFillColor(sf::Color(60, 40, 120, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect line2Bounds = line2.getLocalBounds();
    line2.setOrigin({line2Bounds.size.x / 2.0f, 0});
    line2.setPosition({windowSize.x / 2.0f, 250});
    window.draw(line2);

    // Visual demo - animated bar
    float barWidth = 600.0f;
    float barHeight = 100.0f;
    float barX = (windowSize.x - barWidth) / 2.0f;
    float barY = 350;

    sf::RectangleShape bar({barWidth, barHeight});
    bar.setPosition({barX, barY});
    bar.setFillColor(sf::Color(200, 200, 220, static_cast<std::uint8_t>(fadeAlpha)));
    bar.setOutlineThickness(3);
    bar.setOutlineColor(sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    window.draw(bar);

    sf::Text barLabel(font, "ACTIVE BAR", 24);
    barLabel.setFillColor(sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    barLabel.setStyle(sf::Text::Bold);
    sf::FloatRect barLabelBounds = barLabel.getLocalBounds();
    barLabel.setOrigin({barLabelBounds.size.x / 2.0f, 0});
    barLabel.setPosition({windowSize.x / 2.0f, barY + barHeight + 20});
    window.draw(barLabel);

    // Animated arrow showing execution order
    float arrowOffset = std::sin(stageTimer * 2.0f) * 50.0f;
    sf::Text arrow(font, ">>>", 40);
    arrow.setFillColor(sf::Color(255, 150, 0, static_cast<std::uint8_t>(fadeAlpha)));
    arrow.setStyle(sf::Text::Bold);
    arrow.setPosition({barX + 50 + arrowOffset, barY + 25});
    window.draw(arrow);
}

// Render the demonstrate combat stage
void TutorialScreen::renderDemonstrateCombat(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    sf::Text titleText(font, "TRY IT YOURSELF!", 50);
    titleText.setFillColor(sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 40});
    window.draw(titleText);

    sf::Text instructionText(font, "Click a card to play it!", 26);
    instructionText.setFillColor(sf::Color(100, 70, 170, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect instrBounds = instructionText.getLocalBounds();
    instructionText.setOrigin({instrBounds.size.x / 2.0f, 0});
    instructionText.setPosition({windowSize.x / 2.0f, 120});
    window.draw(instructionText);

    // Demo enemy
    float enemyX = windowSize.x / 2.0f;
    float enemyY = 250;

    sf::RectangleShape enemyBox({150, 150});
    enemyBox.setOrigin({75, 75});
    enemyBox.setPosition({enemyX, enemyY});
    enemyBox.setFillColor(sf::Color(255, 100, 100, static_cast<std::uint8_t>(fadeAlpha)));
    enemyBox.setOutlineThickness(3);
    enemyBox.setOutlineColor(sf::Color(150, 50, 50, static_cast<std::uint8_t>(fadeAlpha)));
    window.draw(enemyBox);

    sf::Text enemyLabel(font, "BUG", 28);
    enemyLabel.setFillColor(sf::Color::White);
    enemyLabel.setStyle(sf::Text::Bold);
    sf::FloatRect enemyLabelBounds = enemyLabel.getLocalBounds();
    enemyLabel.setOrigin({enemyLabelBounds.size.x / 2.0f, enemyLabelBounds.size.y / 2.0f});
    enemyLabel.setPosition({enemyX, enemyY - 30});
    window.draw(enemyLabel);

    sf::Text enemyHP(font, std::to_string(demoEnemyHP) + "/" + std::to_string(demoEnemyMaxHP), 24);
    enemyHP.setFillColor(sf::Color::White);
    enemyHP.setStyle(sf::Text::Bold);
    sf::FloatRect enemyHPBounds = enemyHP.getLocalBounds();
    enemyHP.setOrigin({enemyHPBounds.size.x / 2.0f, enemyHPBounds.size.y / 2.0f});
    enemyHP.setPosition({enemyX, enemyY + 20});
    window.draw(enemyHP);

    // Player stats
    float statsX = 100;
    float statsY = 200;

    sf::Text playerHPText(font, "HP: " + std::to_string(demoPlayerHP) + "/" + std::to_string(demoPlayerMaxHP), 24);
    playerHPText.setFillColor(sf::Color(255, 100, 100, static_cast<std::uint8_t>(fadeAlpha)));
    playerHPText.setStyle(sf::Text::Bold);
    playerHPText.setPosition({statsX, statsY});
    window.draw(playerHPText);

    sf::Text playerMemText(font, "Memory: " + std::to_string(demoPlayerMemory) + "/" + std::to_string(demoPlayerMaxMemory), 24);
    playerMemText.setFillColor(sf::Color(100, 150, 255, static_cast<std::uint8_t>(fadeAlpha)));
    playerMemText.setStyle(sf::Text::Bold);
    playerMemText.setPosition({statsX, statsY + 40});
    window.draw(playerMemText);

    sf::Text playerBlockText(font, "Block: " + std::to_string(demoPlayerBlock), 24);
    playerBlockText.setFillColor(sf::Color(150, 150, 150, static_cast<std::uint8_t>(fadeAlpha)));
    playerBlockText.setStyle(sf::Text::Bold);
    playerBlockText.setPosition({statsX, statsY + 80});
    window.draw(playerBlockText);

    // Draw demo cards
    float cardWidth = 120.0f;
    float cardHeight = 160.0f;
    float cardSpacing = 15.0f;
    float handY = windowSize.y * 0.75f;
    float totalHandWidth = demoCards.size() * cardWidth + (demoCards.size() - 1) * cardSpacing;
    float handStartX = (windowSize.x - totalHandWidth) / 2.0f;

    for (int i = 0; i < static_cast<int>(demoCards.size()); i++)
    {
        Card *card = demoCards[i];
        float cardX = handStartX + i * (cardWidth + cardSpacing);

        sf::RectangleShape cardBg({cardWidth, cardHeight});
        cardBg.setPosition({cardX, handY});
        cardBg.setFillColor(sf::Color(240, 240, 240, static_cast<std::uint8_t>(fadeAlpha)));
        cardBg.setOutlineThickness(2);
        cardBg.setOutlineColor(sf::Color(100, 100, 100, static_cast<std::uint8_t>(fadeAlpha)));
        window.draw(cardBg);

        sf::Text cardName(font, card->getName(), 18);
        cardName.setFillColor(sf::Color::Black);
        cardName.setStyle(sf::Text::Bold);
        sf::FloatRect cardNameBounds = cardName.getLocalBounds();
        cardName.setOrigin({cardNameBounds.size.x / 2.0f, 0});
        cardName.setPosition({cardX + cardWidth / 2.0f, handY + 10});
        window.draw(cardName);

        sf::Text cardCost(font, std::to_string(card->getEnergyCost()), 22);
        cardCost.setFillColor(sf::Color(100, 150, 255));
        cardCost.setStyle(sf::Text::Bold);
        cardCost.setPosition({cardX + 10, handY + cardHeight - 35});
        window.draw(cardCost);
    }

    // Animation message
    if (animationPlaying)
    {
        float animAlpha = (2.0f - animationTimer) / 2.0f * fadeAlpha;
        sf::Text animText(font, animationMessage, 36);
        animText.setFillColor(sf::Color(255, 200, 0, static_cast<std::uint8_t>(animAlpha)));
        animText.setStyle(sf::Text::Bold);
        sf::FloatRect animBounds = animText.getLocalBounds();
        animText.setOrigin({animBounds.size.x / 2.0f, 0});
        animText.setPosition({windowSize.x / 2.0f, 450 - animationTimer * 50});
        window.draw(animText);
    }
}

// Render the explain rooms stage
void TutorialScreen::renderExplainRooms(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    sf::Text titleText(font, "ROOM TYPES", 50);
    titleText.setFillColor(sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 50});
    window.draw(titleText);

    float yPos = 150;
    float spacing = 70;

    sf::Text combat(font, "COMBAT - Fight bugs and enemies", 26);
    combat.setFillColor(sf::Color(255, 100, 100, static_cast<std::uint8_t>(fadeAlpha)));
    combat.setStyle(sf::Text::Bold);
    combat.setPosition({100, yPos});
    window.draw(combat);

    sf::Text treasure(font, "TREASURE - Find rare cards", 26);
    treasure.setFillColor(sf::Color(255, 200, 0, static_cast<std::uint8_t>(fadeAlpha)));
    treasure.setStyle(sf::Text::Bold);
    treasure.setPosition({100, yPos + spacing});
    window.draw(treasure);

    sf::Text rest(font, "REST - Heal HP or upgrade cards", 26);
    rest.setFillColor(sf::Color(100, 255, 150, static_cast<std::uint8_t>(fadeAlpha)));
    rest.setStyle(sf::Text::Bold);
    rest.setPosition({100, yPos + spacing * 2});
    window.draw(rest);

    sf::Text shop(font, "SHOP - Buy cards with Bitcoin", 26);
    shop.setFillColor(sf::Color(180, 100, 255, static_cast<std::uint8_t>(fadeAlpha)));
    shop.setStyle(sf::Text::Bold);
    shop.setPosition({100, yPos + spacing * 3});
    window.draw(shop);

    sf::Text exitRoom(font, "EXIT - Advance to next stage!", 26);
    exitRoom.setFillColor(sf::Color(100, 200, 255, static_cast<std::uint8_t>(fadeAlpha)));
    exitRoom.setStyle(sf::Text::Bold);
    exitRoom.setPosition({100, yPos + spacing * 4});
    window.draw(exitRoom);

    sf::Text readyText(font, "You're ready! Good luck!", 32);
    readyText.setFillColor(sf::Color(80, 150, 80, static_cast<std::uint8_t>(fadeAlpha)));
    readyText.setStyle(sf::Text::Bold);
    sf::FloatRect readyBounds = readyText.getLocalBounds();
    readyText.setOrigin({readyBounds.size.x / 2.0f, 0});
    readyText.setPosition({windowSize.x / 2.0f, yPos + spacing * 5 + 30});
    window.draw(readyText);
}

// Render the Next button
void TutorialScreen::renderNextButton(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    float buttonW = 250.0f;
    float buttonH = 60.0f;
    float buttonX = windowSize.x - buttonW - 50;
    float buttonY = windowSize.y - buttonH - 50;

    sf::RectangleShape button({buttonW, buttonH});
    button.setPosition({buttonX, buttonY});
    button.setFillColor(hoveredNext ? sf::Color(100, 70, 180, static_cast<std::uint8_t>(fadeAlpha))
                                    : sf::Color(80, 50, 150, static_cast<std::uint8_t>(fadeAlpha)));
    button.setOutlineThickness(3);
    button.setOutlineColor(sf::Color(60, 40, 120, static_cast<std::uint8_t>(fadeAlpha)));
    window.draw(button);

    std::string buttonLabel = (currentStage == TutorialStage::EXPLAIN_ROOMS) ? "START GAME!" : "NEXT";

    sf::Text buttonText(font, buttonLabel, 28);
    buttonText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
    buttonText.setStyle(sf::Text::Bold);
    sf::FloatRect buttonTextBounds = buttonText.getLocalBounds();
    buttonText.setOrigin({buttonTextBounds.size.x / 2.0f, buttonTextBounds.size.y / 2.0f});
    buttonText.setPosition({buttonX + buttonW / 2.0f, buttonY + buttonH / 2.0f});
    window.draw(buttonText);
}
