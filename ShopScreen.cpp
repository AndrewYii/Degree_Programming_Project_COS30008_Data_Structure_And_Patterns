#include "ShopScreen.h"
#include "Game.h"
#include "Player.h"
#include "Card.h"
#include "CardFactory.h"
#include "AIStoryteller.h"
#include <iostream>

// Constructor
ShopScreen::ShopScreen(Game *g) : Screen(g), game(g), playerRef(nullptr), hoveredCardIndex(-1), hoveredBuffIndex(-1), hoveredLeave(false), hoveredGacha(false), showCardDetail(false), selectedCardForDetail(nullptr), showBuffDetail(false), selectedBuffIndex(-1), cardDetailAlpha(0.0f), buffDetailAlpha(0.0f), hoveredConfirmButton(false), hoveredBuffConfirmButton(false), fadeAlpha(0.0f), gachaPrice(100), showGachaResult(false), gachaResultCard(nullptr), gachaResultTimer(0.0f)
{
}
// Destructor
ShopScreen::~ShopScreen()
{
    shopCards.clear();
    buffItems.clear();
    if (gachaResultCard)
    {
        delete gachaResultCard;
        gachaResultCard = nullptr;
    }
}
// Navigate to shop screen
void ShopScreen::onEnter()
{
    std::cout << "[ShopScreen] Entered - shop area" << std::endl;
    playerRef = &game->getPlayer();
    fadeAlpha = 0.0f;
    showGachaResult = false;
    gachaResultTimer = 0.0f;
    showBuffDetail = false;
    selectedBuffIndex = -1;
    buffDetailAlpha = 0.0f;
    if (gachaResultCard)
    {
        delete gachaResultCard;
        gachaResultCard = nullptr;
    }
    generateShop();
    initializeBuffShop();
}
// Exit shop screen
void ShopScreen::onExit()
{
    std::cout << "[ShopScreen] Exiting" << std::endl;
    shopCards.clear();
    cardPrices.clear();
    buffItems.clear();
    if (gachaResultCard)
    {
        delete gachaResultCard;
        gachaResultCard = nullptr;
    }
}
// Initialize buff shop with available buffs
void ShopScreen::initializeBuffShop()
{
    buffItems.clear();
    Player::RedeemBuffs &buffs = playerRef->getRedeemBuffs();

    // Add all available buffs with descriptions, prices, and icon paths
    buffItems.push_back(BuffItem(
        "One Punch Man",
        "Deal 999 damage on your next attack.\nUltimate power!",
        300,
        &buffs.onePunchMan,
        "asset/buff/onepunch.png"));

    buffItems.push_back(BuffItem(
        "Iterator Pattern",
        "Draw +1 card each turn.\nTraverse efficiently!",
        200,
        &buffs.iterator,
        "asset/buff/iterator.png"));

    buffItems.push_back(BuffItem(
        "Singleton Pattern",
        "Add special green slot in Active Bar.\nUnique instance!",
        250,
        &buffs.singleton,
        "asset/buff/singleton.png"));

    buffItems.push_back(BuffItem(
        "Factory Pattern",
        "Reduce all card costs by 1 energy.\nMass production!",
        280,
        &buffs.factory,
        "asset/buff/factory.png"));

    buffItems.push_back(BuffItem(
        "Adapter Pattern",
        "Gain +5 block at start of each turn.\nAdapt and survive!",
        220,
        &buffs.adapter,
        "asset/buff/adapter.png"));

    buffItems.push_back(BuffItem(
        "Observer Pattern",
        "Add special observer slot.\nExecutes last in Active Bar.\nWatch and react to battle!",
        240,
        &buffs.observer,
        "asset/buff/observer.png"));

    // Check which buffs are already purchased
    for (auto &item : buffItems)
    {
        if (item.buffPtr && *item.buffPtr)
        {
            item.purchased = true;
        }
    }

    // Load buff textures
    loadBuffTextures();

    std::cout << "[Shop] Initialized buff shop with " << buffItems.size() << " buffs" << std::endl;
}

// Load buff icon textures
void ShopScreen::loadBuffTextures()
{
    buffTextures.clear();

    for (const auto &buff : buffItems)
    {
        sf::Texture texture;
        if (texture.loadFromFile(buff.iconPath))
        {
            buffTextures[buff.name] = std::move(texture);
            std::cout << "[Shop] Loaded buff texture: " << buff.iconPath << std::endl;
        }
        else
        {
            std::cout << "[Shop] WARNING: Failed to load buff texture: " << buff.iconPath << std::endl;
        }
    }
}
// Calculate card price based on rarity
int ShopScreen::calculateCardPrice(Card *card)
{
    int basePrice = 50;
    Card::CardRarity rarity = card->getRarity();
    switch (rarity)
    {
    case Card::CardRarity::COMMON:
        basePrice = 40;
        break;
    case Card::CardRarity::UNCOMMON:
        basePrice = 70;
        break;
    case Card::CardRarity::RARE:
        basePrice = 120;
        break;
    case Card::CardRarity::EPIC:
        basePrice = 180;
        break;
    case Card::CardRarity::LEGENDARY:
        basePrice = 250;
        break;
    default:
        basePrice = 50;
        break;
    }

    return basePrice;
}
// Generate shop inventory
void ShopScreen::generateShop()
{
    shopCards.clear();
    cardPrices.clear();
    CardFactory *factory = CardFactory::getInstance();
    for (int i = 0; i < 2; i++)
    {
        Card *common = factory->getRandomCard(Card::CardRarity::COMMON);
        if (common)
        {
            shopCards.push_back(common);
            cardPrices.push_back(calculateCardPrice(common));
        }
    }

    for (int i = 0; i < 2; i++)
    {
        Card *uncommon = factory->getRandomCard(Card::CardRarity::UNCOMMON);
        if (uncommon)
        {
            shopCards.push_back(uncommon);
            cardPrices.push_back(calculateCardPrice(uncommon));
        }
    }

    Card *rare = factory->getRandomCard(Card::CardRarity::RARE);
    if (rare)
    {
        shopCards.push_back(rare);
        cardPrices.push_back(calculateCardPrice(rare));
    }

    std::cout << "[Shop] Generated shop with " << shopCards.size() << " cards for sale" << std::endl;
}
// Buy card from shop
void ShopScreen::buyCard(int cardIndex)
{
    if (cardIndex < 0 || cardIndex >= shopCards.size())
        return;

    int price = cardPrices[cardIndex];
    int playerGold = playerRef->getGold();

    if (playerGold >= price)
    {
        Card *card = shopCards[cardIndex];
        playerRef->addCardToDeck(card->clone());
        playerRef->addGold(-price);

        AIStoryteller::getInstance()->onShopVisit(price);
        bool isRareCard = false;
        switch (card->getRarity())
        {
        case Card::CardRarity::RARE:
        case Card::CardRarity::EPIC:
            isRareCard = true;
            break;
        default:
            break;
        }
        AIStoryteller::getInstance()->onCardAcquired(isRareCard);

        std::cout << "[Shop] Purchased: " << card->getName() << " for " << price << " BTC" << std::endl;
        shopCards.erase(shopCards.begin() + cardIndex);
        cardPrices.erase(cardPrices.begin() + cardIndex);
    }
    else
    {
        std::cout << "[Shop] Not enough Bitcoin! Need " << price << ", have " << playerGold << std::endl;
    }
}
// Buy buff from shop
void ShopScreen::buyBuff(int buffIndex)
{
    if (buffIndex < 0 || buffIndex >= static_cast<int>(buffItems.size()))
        return;

    BuffItem &buff = buffItems[buffIndex];

    // Check if already purchased
    if (buff.purchased || (buff.buffPtr && *buff.buffPtr))
    {
        std::cout << "[Shop] Buff already purchased: " << buff.name << std::endl;
        return;
    }

    int playerGold = playerRef->getGold();

    if (playerGold >= buff.price)
    {
        // Purchase buff
        playerRef->addGold(-buff.price);
        if (buff.buffPtr)
        {
            *buff.buffPtr = true;
        }
        buff.purchased = true;

        std::cout << "[Shop] Purchased buff: " << buff.name << " for " << buff.price << " BTC" << std::endl;
        AIStoryteller::getInstance()->onShopVisit(buff.price);
    }
    else
    {
        std::cout << "[Shop] Not enough Bitcoin for buff! Need " << buff.price << ", have " << playerGold << std::endl;
    }
}
// Perform gacha (random card draw)
void ShopScreen::performGacha()
{
    int playerGold = playerRef->getGold();

    if (playerGold < gachaPrice)
    {
        std::cout << "[Shop] Not enough Bitcoin for gacha! Need " << gachaPrice << ", have " << playerGold << std::endl;
        return;
    }

    // Deduct cost
    playerRef->addGold(-gachaPrice);
    AIStoryteller::getInstance()->onShopVisit(gachaPrice);

    // Gacha rarity probabilities
    // Common: 50%, Uncommon: 30%, Rare: 15%, Epic: 4%, Legendary: 1%
    int roll = rand() % 100;
    Card::CardRarity rarity;

    if (roll < 50)
    {
        rarity = Card::CardRarity::COMMON;
    }
    else if (roll < 80)
    {
        rarity = Card::CardRarity::UNCOMMON;
    }
    else if (roll < 95)
    {
        rarity = Card::CardRarity::RARE;
    }
    else if (roll < 99)
    {
        rarity = Card::CardRarity::EPIC;
    }
    else
    {
        rarity = Card::CardRarity::LEGENDARY;
    }

    // Get random card of that rarity
    CardFactory *factory = CardFactory::getInstance();
    Card *drawnCard = factory->getRandomCard(rarity);

    if (drawnCard)
    {
        // Add to player's deck
        playerRef->addCardToDeck(drawnCard->clone());

        // Store for result display
        if (gachaResultCard)
        {
            delete gachaResultCard;
        }
        gachaResultCard = drawnCard;
        showGachaResult = true;
        gachaResultTimer = 0.0f;

        bool isRareCard = (rarity == Card::CardRarity::RARE || rarity == Card::CardRarity::EPIC || rarity == Card::CardRarity::LEGENDARY);
        AIStoryteller::getInstance()->onCardAcquired(isRareCard);

        std::cout << "[Shop] Gacha result: " << drawnCard->getName() << " (" << static_cast<int>(rarity) << ")" << std::endl;
    }
}
// Handle input events
void ShopScreen::handleEvents()
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

                // Check gacha result popup
                if (showGachaResult)
                {
                    // Click anywhere to close
                    showGachaResult = false;
                    if (gachaResultCard)
                    {
                        delete gachaResultCard;
                        gachaResultCard = nullptr;
                    }
                    return;
                }

                // Check if clicked confirm button in buff detail popup
                if (showBuffDetail && selectedBuffIndex >= 0)
                {
                    float buttonW = 250.0f;
                    float buttonH = 60.0f;
                    float popupWidth = 500;
                    float popupHeight = 550;
                    float popupX = (windowSize.x - popupWidth) / 2.0f;
                    float popupY = (windowSize.y - popupHeight) / 2.0f;
                    float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
                    float buttonY = popupY + popupHeight - 90;
                    sf::FloatRect confirmBounds({buttonX, buttonY}, {buttonW, buttonH});

                    if (confirmBounds.contains(mousePos))
                    {
                        // Buy the buff
                        buyBuff(selectedBuffIndex);
                        showBuffDetail = false;
                        selectedBuffIndex = -1;
                        buffDetailAlpha = 0.0f;
                        return;
                    }

                    // Click outside popup closes it
                    float popupLeft = popupX;
                    float popupRight = popupX + popupWidth;
                    float popupTop = popupY;
                    float popupBottom = popupY + popupHeight;

                    if (mousePos.x < popupLeft || mousePos.x > popupRight ||
                        mousePos.y < popupTop || mousePos.y > popupBottom)
                    {
                        showBuffDetail = false;
                        selectedBuffIndex = -1;
                        buffDetailAlpha = 0.0f;
                    }
                    return;
                }

                // Check if clicked confirm button in card detail popup
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
                        // Find index of selected card and buy it
                        for (int i = 0; i < static_cast<int>(shopCards.size()); i++)
                        {
                            if (shopCards[i] == selectedCardForDetail)
                            {
                                buyCard(i);
                                showCardDetail = false;
                                selectedCardForDetail = nullptr;
                                cardDetailAlpha = 0.0f;
                                break;
                            }
                        }
                        return;
                    }

                    // Click outside popup closes it
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

                // Check gacha button
                if (!showCardDetail)
                {
                    float gachaW = 200.0f;
                    float gachaH = 50.0f;
                    float gachaX = windowSize.x * 0.75f - gachaW / 2.0f;
                    float gachaY = windowSize.y * 0.2f;
                    sf::FloatRect gachaBounds({gachaX, gachaY}, {gachaW, gachaH});

                    if (gachaBounds.contains(mousePos) && playerRef->getGold() >= gachaPrice)
                    {
                        performGacha();
                        return;
                    }
                }

                // Check buff buttons (show preview)
                if (!showCardDetail && !showBuffDetail)
                {
                    float buffWidth = 180.0f;
                    float buffHeight = 140.0f;
                    float buffSpacing = 15.0f;
                    int buffsPerRow = 3;
                    float totalBuffWidth = buffsPerRow * buffWidth + (buffsPerRow - 1) * buffSpacing;
                    float buffStartX = windowSize.x * 0.25f - totalBuffWidth / 2.0f;
                    float buffStartY = windowSize.y * 0.35f;

                    for (int i = 0; i < static_cast<int>(buffItems.size()); i++)
                    {
                        int row = i / buffsPerRow;
                        int col = i % buffsPerRow;
                        float buffX = buffStartX + col * (buffWidth + buffSpacing);
                        float buffY = buffStartY + row * (buffHeight + buffSpacing);
                        sf::FloatRect buffBounds({buffX, buffY}, {buffWidth, buffHeight});

                        if (buffBounds.contains(mousePos))
                        {
                            // Show preview instead of buying immediately
                            showBuffDetail = true;
                            selectedBuffIndex = i;
                            buffDetailAlpha = 0.0f;
                            std::cout << "[Shop] Previewing buff: " << buffItems[i].name << std::endl;
                            break;
                        }
                    }
                }

                // Check if clicked on card (show preview)
                if (!showCardDetail && !showBuffDetail)
                {
                    float cardWidth = 140.0f;
                    float cardHeight = 220.0f;
                    float spacing = 20.0f;
                    float totalWidth = shopCards.size() * cardWidth + (shopCards.size() - 1) * spacing;
                    float startX = windowSize.x * 0.75f - totalWidth / 2.0f;
                    float cardY = windowSize.y * 0.35f;

                    for (int i = 0; i < shopCards.size(); i++)
                    {
                        float cardX = startX + i * (cardWidth + spacing);
                        sf::FloatRect cardBounds({cardX, cardY}, {cardWidth, cardHeight});

                        if (cardBounds.contains(mousePos))
                        {
                            // Show preview instead of buying immediately
                            showCardDetail = true;
                            selectedCardForDetail = shopCards[i];
                            cardDetailAlpha = 0.0f;
                            std::cout << "[Shop] Previewing card: " << selectedCardForDetail->getName() << std::endl;
                            break;
                        }
                    }
                }

                // Check leave button
                if (!showCardDetail)
                {
                    float buttonW = 200.0f;
                    float buttonH = 50.0f;
                    float buttonX = (windowSize.x - buttonW) / 2.0f;
                    float buttonY = windowSize.y * 0.90f;
                    sf::FloatRect buttonBounds({buttonX, buttonY}, {buttonW, buttonH});

                    if (buttonBounds.contains(mousePos))
                    {
                        std::cout << "[Shop] Leave - returning to map" << std::endl;
                        game->changeScreen(Game::ScreenType::GAMEPLAY);
                    }
                }
            }
        }
    }
}

// Update shop screen
void ShopScreen::update(float deltaTime)
{
    // Fade in
    if (fadeAlpha < 255.0f)
    {
        fadeAlpha += deltaTime * 400.0f;
        if (fadeAlpha > 255.0f)
            fadeAlpha = 255.0f;
    }

    // Update gacha result timer
    if (showGachaResult)
    {
        gachaResultTimer += deltaTime;
    }

    // Fade in card detail popup
    if (showCardDetail && cardDetailAlpha < 255.0f)
    {
        cardDetailAlpha += deltaTime * 600.0f;
        if (cardDetailAlpha > 255.0f)
            cardDetailAlpha = 255.0f;
    }

    // Fade in buff detail popup
    if (showBuffDetail && buffDetailAlpha < 255.0f)
    {
        buffDetailAlpha += deltaTime * 600.0f;
        if (buffDetailAlpha > 255.0f)
            buffDetailAlpha = 255.0f;
    }

    // Update hover states
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*game->getWindow());
    sf::Vector2f mousePos = game->getWindow()->mapPixelToCoords(mousePixelPos);
    sf::Vector2u windowSize = game->getWindow()->getSize();

    hoveredCardIndex = -1;
    hoveredBuffIndex = -1;
    hoveredLeave = false;
    hoveredGacha = false;
    hoveredConfirmButton = false;
    hoveredBuffConfirmButton = false;

    // Don't update hover if gacha result is showing
    if (showGachaResult)
    {
        return;
    }

    // Check confirm button hover in buff detail popup
    if (showBuffDetail)
    {
        float buttonW = 250.0f;
        float buttonH = 60.0f;
        float popupWidth = 500;
        float popupHeight = 550;
        float popupX = (windowSize.x - popupWidth) / 2.0f;
        float popupY = (windowSize.y - popupHeight) / 2.0f;
        float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
        float buttonY = popupY + popupHeight - 90;
        sf::FloatRect confirmBounds({buttonX, buttonY}, {buttonW, buttonH});

        if (confirmBounds.contains(mousePos))
        {
            hoveredBuffConfirmButton = true;
        }
        return;
    }

    // Check confirm button hover in card detail popup
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
        return;
    }

    // Check gacha button hover
    float gachaW = 200.0f;
    float gachaH = 50.0f;
    float gachaX = windowSize.x * 0.75f - gachaW / 2.0f;
    float gachaY = windowSize.y * 0.2f;
    sf::FloatRect gachaBounds({gachaX, gachaY}, {gachaW, gachaH});

    if (gachaBounds.contains(mousePos))
    {
        hoveredGacha = true;
    }

    // Check buff hover
    float buffWidth = 180.0f;
    float buffHeight = 140.0f;
    float buffSpacing = 15.0f;
    int buffsPerRow = 3;
    float totalBuffWidth = buffsPerRow * buffWidth + (buffsPerRow - 1) * buffSpacing;
    float buffStartX = windowSize.x * 0.25f - totalBuffWidth / 2.0f;
    float buffStartY = windowSize.y * 0.35f;

    for (int i = 0; i < static_cast<int>(buffItems.size()); i++)
    {
        int row = i / buffsPerRow;
        int col = i % buffsPerRow;
        float buffX = buffStartX + col * (buffWidth + buffSpacing);
        float buffY = buffStartY + row * (buffHeight + buffSpacing);
        sf::FloatRect buffBounds({buffX, buffY}, {buffWidth, buffHeight});

        if (buffBounds.contains(mousePos))
        {
            hoveredBuffIndex = i;
            break;
        }
    }

    // Check card hover
    float cardWidth = 140.0f;
    float cardHeight = 220.0f;
    float spacing = 20.0f;
    float totalWidth = shopCards.size() * cardWidth + (shopCards.size() - 1) * spacing;
    float startX = windowSize.x * 0.75f - totalWidth / 2.0f;
    float cardY = windowSize.y * 0.35f;

    for (int i = 0; i < shopCards.size(); i++)
    {
        float cardX = startX + i * (cardWidth + spacing);
        sf::FloatRect cardBounds({cardX, cardY}, {cardWidth, cardHeight});

        if (cardBounds.contains(mousePos))
        {
            hoveredCardIndex = i;
            break;
        }
    }

    // Check leave button hover
    float buttonW = 200.0f;
    float buttonH = 50.0f;
    float buttonX = (windowSize.x - buttonW) / 2.0f;
    float buttonY = windowSize.y * 0.90f;
    sf::FloatRect buttonBounds({buttonX, buttonY}, {buttonW, buttonH});

    if (buttonBounds.contains(mousePos))
    {
        hoveredLeave = true;
    }
}

// Render shop screen
void ShopScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    window.clear(sf::Color(180, 150, 200));

    // Title
    sf::Text titleText(font, "BITCOIN SHOP", 64);
    titleText.setFillColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(fadeAlpha)));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({windowSize.x / 2.0f, 30});
    window.draw(titleText);

    // Player's Bitcoin
    int playerGold = playerRef->getGold();
    sf::Text goldText(font, "Your Bitcoin: " + std::to_string(playerGold) + " BTC", 28);
    goldText.setFillColor(sf::Color(255, 165, 0, static_cast<std::uint8_t>(fadeAlpha)));
    goldText.setStyle(sf::Text::Bold);
    sf::FloatRect goldBounds = goldText.getLocalBounds();
    goldText.setOrigin({goldBounds.size.x / 2.0f, 0});
    goldText.setPosition({windowSize.x / 2.0f, 110});
    window.draw(goldText);

    // === LEFT SIDE: BUFFS ===
    sf::Text buffTitle(font, "PERMANENT BUFFS", 32);
    buffTitle.setFillColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(fadeAlpha)));
    buffTitle.setStyle(sf::Text::Bold);
    buffTitle.setPosition({windowSize.x * 0.25f - 120, 170});
    window.draw(buffTitle);

    // Draw buff items (3 columns x 2 rows)
    float buffWidth = 180.0f;
    float buffHeight = 140.0f;
    float buffSpacing = 15.0f;
    int buffsPerRow = 3;
    float totalBuffWidth = buffsPerRow * buffWidth + (buffsPerRow - 1) * buffSpacing;
    float buffStartX = windowSize.x * 0.25f - totalBuffWidth / 2.0f;
    float buffStartY = windowSize.y * 0.35f;

    for (int i = 0; i < static_cast<int>(buffItems.size()); i++)
    {
        const BuffItem &buff = buffItems[i];
        int row = i / buffsPerRow;
        int col = i % buffsPerRow;
        float buffX = buffStartX + col * (buffWidth + buffSpacing);
        float buffY = buffStartY + row * (buffHeight + buffSpacing);
        bool isHovered = (hoveredBuffIndex == i);
        bool canAfford = (playerRef->getGold() >= buff.price && !buff.purchased);

        // Buff box
        sf::RectangleShape buffBox({buffWidth, buffHeight});
        buffBox.setPosition({buffX, buffY});
        if (buff.purchased)
        {
            buffBox.setFillColor(sf::Color(100, 200, 100, static_cast<std::uint8_t>(fadeAlpha))); // Green = purchased
        }
        else if (canAfford)
        {
            buffBox.setFillColor(sf::Color(250, 250, 240, static_cast<std::uint8_t>(fadeAlpha)));
        }
        else
        {
            buffBox.setFillColor(sf::Color(200, 200, 200, static_cast<std::uint8_t>(fadeAlpha))); // Gray = can't afford
        }
        buffBox.setOutlineThickness(isHovered ? 3.0f : 2.0f);
        buffBox.setOutlineColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(fadeAlpha)));
        window.draw(buffBox);

        // Buff name
        sf::Text buffName(font, buff.name, 16);
        buffName.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(fadeAlpha)));
        buffName.setStyle(sf::Text::Bold);
        sf::FloatRect buffNameBounds = buffName.getLocalBounds();
        buffName.setOrigin({buffNameBounds.size.x / 2.0f, 0});
        buffName.setPosition({buffX + buffWidth / 2.0f, buffY + 10});
        window.draw(buffName);

        // Description (wrapped)
        sf::Text buffDesc(font, buff.description, 12);
        buffDesc.setFillColor(sf::Color(50, 50, 50, static_cast<std::uint8_t>(fadeAlpha)));
        buffDesc.setPosition({buffX + 10, buffY + 40});
        window.draw(buffDesc);

        // Price or "OWNED"
        if (buff.purchased)
        {
            sf::Text ownedText(font, "OWNED", 20);
            ownedText.setFillColor(sf::Color(0, 100, 0, static_cast<std::uint8_t>(fadeAlpha)));
            ownedText.setStyle(sf::Text::Bold);
            sf::FloatRect ownedBounds = ownedText.getLocalBounds();
            ownedText.setOrigin({ownedBounds.size.x / 2.0f, 0});
            ownedText.setPosition({buffX + buffWidth / 2.0f, buffY + buffHeight - 30});
            window.draw(ownedText);
        }
        else
        {
            sf::Text priceText(font, std::to_string(buff.price) + " BTC", 18);
            priceText.setFillColor(canAfford ? sf::Color(255, 165, 0, static_cast<std::uint8_t>(fadeAlpha))
                                             : sf::Color(150, 80, 80, static_cast<std::uint8_t>(fadeAlpha)));
            priceText.setStyle(sf::Text::Bold);
            sf::FloatRect priceBounds = priceText.getLocalBounds();
            priceText.setOrigin({priceBounds.size.x / 2.0f, 0});
            priceText.setPosition({buffX + buffWidth / 2.0f, buffY + buffHeight - 30});
            window.draw(priceText);
        }

        // Hover glow
        if (isHovered && canAfford)
        {
            sf::RectangleShape glow({buffWidth + 6, buffHeight + 6});
            glow.setPosition({buffX - 3, buffY - 3});
            glow.setFillColor(sf::Color::Transparent);
            glow.setOutlineThickness(3.0f);
            glow.setOutlineColor(sf::Color(255, 200, 0, static_cast<std::uint8_t>(fadeAlpha * 0.8f)));
            window.draw(glow);
        }
    }

    // === RIGHT SIDE: GACHA + CARDS ===
    sf::Text cardTitle(font, "RANDOM CARDS", 32);
    cardTitle.setFillColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(fadeAlpha)));
    cardTitle.setStyle(sf::Text::Bold);
    cardTitle.setPosition({windowSize.x * 0.75f - 120, 170});
    window.draw(cardTitle);

    // Gacha button
    float gachaW = 200.0f;
    float gachaH = 50.0f;
    float gachaX = windowSize.x * 0.75f - gachaW / 2.0f;
    float gachaY = windowSize.y * 0.2f;
    bool canAffordGacha = (playerRef->getGold() >= gachaPrice);

    sf::RectangleShape gachaButton({gachaW, gachaH});
    gachaButton.setPosition({gachaX, gachaY});
    if (canAffordGacha)
    {
        gachaButton.setFillColor(hoveredGacha ? sf::Color(255, 180, 0, static_cast<std::uint8_t>(fadeAlpha))
                                              : sf::Color(255, 165, 0, static_cast<std::uint8_t>(fadeAlpha)));
    }
    else
    {
        gachaButton.setFillColor(sf::Color(150, 150, 150, static_cast<std::uint8_t>(fadeAlpha)));
    }
    gachaButton.setOutlineThickness(2.0f);
    gachaButton.setOutlineColor(sf::Color(200, 100, 0, static_cast<std::uint8_t>(fadeAlpha)));
    window.draw(gachaButton);

    sf::Text gachaText(font, "LUCKY DRAW", 24);
    gachaText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
    gachaText.setStyle(sf::Text::Bold);
    sf::FloatRect gachaTextBounds = gachaText.getLocalBounds();
    gachaText.setOrigin({gachaTextBounds.size.x / 2.0f, gachaTextBounds.size.y / 2.0f});
    gachaText.setPosition({gachaX + gachaW / 2.0f, gachaY + gachaH / 2.0f - 8});
    window.draw(gachaText);

    sf::Text gachaPriceText(font, std::to_string(gachaPrice) + " BTC", 16);
    gachaPriceText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect gachaPriceBounds = gachaPriceText.getLocalBounds();
    gachaPriceText.setOrigin({gachaPriceBounds.size.x / 2.0f, 0});
    gachaPriceText.setPosition({gachaX + gachaW / 2.0f, gachaY + gachaH / 2.0f + 5});
    window.draw(gachaPriceText);

    // Gacha description
    sf::Text gachaDesc(font, "50% Common | 30% Uncommon | 15% Rare | 4% Epic | 1% Legendary", 14);
    gachaDesc.setFillColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect gachaDescBounds = gachaDesc.getLocalBounds();
    gachaDesc.setOrigin({gachaDescBounds.size.x / 2.0f, 0});
    gachaDesc.setPosition({windowSize.x * 0.75f, gachaY + gachaH + 10});
    window.draw(gachaDesc);

    // Draw shop cards
    float cardWidth = 140.0f;
    float cardHeight = 220.0f;
    float spacing = 20.0f;
    float totalWidth = shopCards.size() * cardWidth + (shopCards.size() - 1) * spacing;
    float startX = windowSize.x * 0.75f - totalWidth / 2.0f;
    float cardY = windowSize.y * 0.35f;

    for (int i = 0; i < static_cast<int>(shopCards.size()); i++)
    {
        int price = cardPrices[i];
        float cardX = startX + i * (cardWidth + spacing);
        bool isHovered = (hoveredCardIndex == i);
        bool canAfford = (playerRef->getGold() >= price);

        // Card background
        sf::RectangleShape cardBg({cardWidth, cardHeight});
        cardBg.setPosition({cardX, cardY});
        cardBg.setFillColor(canAfford ? sf::Color(250, 250, 240, static_cast<std::uint8_t>(fadeAlpha))
                                      : sf::Color(200, 200, 200, static_cast<std::uint8_t>(fadeAlpha)));
        cardBg.setOutlineThickness(isHovered ? 3.0f : 2.0f);
        cardBg.setOutlineColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(fadeAlpha)));
        window.draw(cardBg);

        sf::Text questionMark(font, "?", 100);
        questionMark.setFillColor(sf::Color(120, 80, 140, static_cast<std::uint8_t>(fadeAlpha)));
        questionMark.setStyle(sf::Text::Bold);
        sf::FloatRect questionBounds = questionMark.getLocalBounds();
        questionMark.setOrigin({questionBounds.size.x / 2.0f, questionBounds.size.y / 2.0f});
        questionMark.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight / 2.0f - 20});
        window.draw(questionMark);

        sf::Text priceText(font, std::to_string(price) + " BTC", 20);
        priceText.setFillColor(canAfford ? sf::Color(255, 165, 0, static_cast<std::uint8_t>(fadeAlpha)) : sf::Color(150, 80, 80, static_cast<std::uint8_t>(fadeAlpha)));
        priceText.setStyle(sf::Text::Bold);
        sf::FloatRect priceBounds = priceText.getLocalBounds();
        priceText.setOrigin({priceBounds.size.x / 2.0f, 0});
        priceText.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight - 35});
        window.draw(priceText);

        // Click to view hint
        if (isHovered && canAfford)
        {
            sf::Text hintText(font, "Click to view", 14);
            hintText.setFillColor(sf::Color(255, 200, 0, static_cast<std::uint8_t>(fadeAlpha)));
            sf::FloatRect hintBounds = hintText.getLocalBounds();
            hintText.setOrigin({hintBounds.size.x / 2.0f, 0});
            hintText.setPosition({cardX + cardWidth / 2.0f, cardY + 10});
            window.draw(hintText);
        }

        // Hover glow
        if (isHovered && canAfford)
        {
            sf::RectangleShape glow({cardWidth + 6, cardHeight + 6});
            glow.setPosition({cardX - 3, cardY - 3});
            glow.setFillColor(sf::Color::Transparent);
            glow.setOutlineThickness(3.0f);
            glow.setOutlineColor(sf::Color(255, 200, 0, static_cast<std::uint8_t>(fadeAlpha * 0.8f)));
            window.draw(glow);
        }
    }

    // Leave button
    float buttonW = 200.0f;
    float buttonH = 50.0f;
    float buttonX = (windowSize.x - buttonW) / 2.0f;
    float buttonY = windowSize.y * 0.90f;

    sf::RectangleShape leaveButton({buttonW, buttonH});
    leaveButton.setPosition({buttonX, buttonY});
    leaveButton.setFillColor(hoveredLeave ? sf::Color(100, 60, 120, static_cast<std::uint8_t>(fadeAlpha))
                                          : sf::Color(120, 80, 140, static_cast<std::uint8_t>(fadeAlpha)));
    window.draw(leaveButton);

    sf::Text leaveText(font, "Leave Shop", 24);
    leaveText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(fadeAlpha)));
    sf::FloatRect leaveTextBounds = leaveText.getLocalBounds();
    leaveText.setOrigin({leaveTextBounds.size.x / 2.0f, leaveTextBounds.size.y / 2.0f});
    leaveText.setPosition({buttonX + buttonW / 2.0f, buttonY + buttonH / 2.0f});
    window.draw(leaveText);

    // Draw popups on top
    if (showGachaResult)
    {
        drawGachaResultPopup(window, font);
    }

    if (showBuffDetail)
    {
        drawBuffDetailPopup(window, font);
    }

    if (showCardDetail)
    {
        drawCardDetailPopup(window, font);
    }
}

// Draw card detail popup
void ShopScreen::drawCardDetailPopup(sf::RenderWindow &window, sf::Font &font)
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
    popup.setOutlineColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(cardDetailAlpha))); // Purple border
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

    // Price
    int cardIndex = -1;
    for (int i = 0; i < static_cast<int>(shopCards.size()); i++)
    {
        if (shopCards[i] == selectedCardForDetail)
        {
            cardIndex = i;
            break;
        }
    }

    if (cardIndex >= 0)
    {
        int price = cardPrices[cardIndex];
        bool canAfford = (playerRef->getGold() >= price);

        sf::Text priceText(font, "Price: " + std::to_string(price) + " BTC", 28);
        priceText.setFillColor(canAfford ? sf::Color(255, 165, 0, static_cast<std::uint8_t>(cardDetailAlpha)) : sf::Color(200, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
        priceText.setStyle(sf::Text::Bold);
        priceText.setPosition({popupX + 50, yPos});
        window.draw(priceText);
        yPos += 60;
    }

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

    // Confirm button (Purchase)
    float buttonW = 250.0f;
    float buttonH = 60.0f;
    float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
    float buttonY = popupY + popupHeight - 90;

    sf::RectangleShape confirmButton({buttonW, buttonH});
    confirmButton.setPosition({buttonX, buttonY});

    bool canAfford = (cardIndex >= 0 && playerRef->getGold() >= cardPrices[cardIndex]);

    if (!canAfford)
    {
        confirmButton.setFillColor(sf::Color(100, 100, 100, static_cast<std::uint8_t>(cardDetailAlpha))); // Gray
    }
    else if (hoveredConfirmButton)
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

    sf::Text confirmText(font, canAfford ? "PURCHASE CARD" : "NOT ENOUGH BTC", 24);
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

// Draw buff detail popup
void ShopScreen::drawBuffDetailPopup(sf::RenderWindow &window, sf::Font &font)
{
    if (!showBuffDetail || selectedBuffIndex < 0 || selectedBuffIndex >= static_cast<int>(buffItems.size()))
        return;

    const BuffItem &buff = buffItems[selectedBuffIndex];
    sf::Vector2u windowSize = window.getSize();

    // Semi-transparent background overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(buffDetailAlpha * 0.7f)));
    window.draw(overlay);

    // Popup panel (center)
    float popupWidth = 500;
    float popupHeight = 550;
    float popupX = (windowSize.x - popupWidth) / 2.0f;
    float popupY = (windowSize.y - popupHeight) / 2.0f;

    sf::RectangleShape popup({popupWidth, popupHeight});
    popup.setPosition({popupX, popupY});
    popup.setFillColor(sf::Color(240, 240, 230, static_cast<std::uint8_t>(buffDetailAlpha)));
    popup.setOutlineColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(buffDetailAlpha))); // Purple border
    popup.setOutlineThickness(6.0f);
    window.draw(popup);

    // Buff name (title)
    sf::Text nameText(font, buff.name, 40);
    nameText.setFillColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(buffDetailAlpha)));
    nameText.setStyle(sf::Text::Bold);
    sf::FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setOrigin({nameBounds.size.x / 2.0f, 0});
    nameText.setPosition({popupX + popupWidth / 2.0f, popupY + 40});
    window.draw(nameText);

    // Buff icon (load and display actual image)
    float iconSize = 200.0f;
    float iconX = popupX + (popupWidth - iconSize) / 2.0f;
    float iconY = popupY + 120;

    // Draw icon if texture is loaded
    auto it = buffTextures.find(buff.name);
    if (it != buffTextures.end())
    {
        sf::Sprite iconSprite(it->second);

        // Get texture size to calculate scale
        sf::Vector2u textureSize = it->second.getSize();
        float scaleX = iconSize / textureSize.x;
        float scaleY = iconSize / textureSize.y;

        // Use the smaller scale to fit the icon within the square while maintaining aspect ratio
        float scale = std::min(scaleX, scaleY);
        iconSprite.setScale({scale, scale});

        // Center the sprite within the icon box
        sf::FloatRect spriteBounds = iconSprite.getLocalBounds();
        float scaledWidth = spriteBounds.size.x * scale;
        float scaledHeight = spriteBounds.size.y * scale;
        float centerX = iconX + (iconSize - scaledWidth) / 2.0f;
        float centerY = iconY + (iconSize - scaledHeight) / 2.0f;

        iconSprite.setPosition({centerX, centerY});

        // Set alpha for fade effect
        sf::Color spriteColor = iconSprite.getColor();
        spriteColor.a = static_cast<std::uint8_t>(buffDetailAlpha);
        iconSprite.setColor(spriteColor);

        window.draw(iconSprite);
    }
    else
    {
        // Fallback: draw a simple rectangle if texture not found
        sf::RectangleShape iconBox({iconSize, iconSize});
        iconBox.setPosition({iconX, iconY});
        iconBox.setFillColor(sf::Color(200, 200, 200, static_cast<std::uint8_t>(buffDetailAlpha * 0.5f)));
        iconBox.setOutlineColor(sf::Color(100, 100, 100, static_cast<std::uint8_t>(buffDetailAlpha)));
        iconBox.setOutlineThickness(4.0f);
        window.draw(iconBox);

        // Show error text
        sf::Text errorText(font, "?", 80);
        errorText.setFillColor(sf::Color(100, 100, 100, static_cast<std::uint8_t>(buffDetailAlpha)));
        sf::FloatRect errorBounds = errorText.getLocalBounds();
        errorText.setOrigin({errorBounds.size.x / 2.0f, errorBounds.size.y / 2.0f});
        errorText.setPosition({iconX + iconSize / 2.0f, iconY + iconSize / 2.0f});
        window.draw(errorText);
    }

    float yPos = popupY + 340;

    // Description title
    sf::Text descTitle(font, "Effect:", 24);
    descTitle.setFillColor(sf::Color(80, 80, 80, static_cast<std::uint8_t>(buffDetailAlpha)));
    descTitle.setStyle(sf::Text::Bold);
    descTitle.setPosition({popupX + 50, yPos});
    window.draw(descTitle);
    yPos += 40;

    // Description text (multi-line)
    sf::Text descText(font, buff.description, 20);
    descText.setFillColor(sf::Color(60, 60, 60, static_cast<std::uint8_t>(buffDetailAlpha)));
    descText.setPosition({popupX + 50, yPos});
    window.draw(descText);
    yPos += 90;

    // Price display
    sf::Text priceLabel(font, "Price:", 24);
    priceLabel.setFillColor(sf::Color(80, 80, 80, static_cast<std::uint8_t>(buffDetailAlpha)));
    priceLabel.setStyle(sf::Text::Bold);
    priceLabel.setPosition({popupX + 50, yPos});
    window.draw(priceLabel);

    sf::Text priceValue(font, std::to_string(buff.price) + " BTC", 28);
    priceValue.setFillColor(sf::Color(255, 165, 0, static_cast<std::uint8_t>(buffDetailAlpha)));
    priceValue.setStyle(sf::Text::Bold);
    priceValue.setPosition({popupX + 150, yPos});
    window.draw(priceValue);

    // Confirm button (Purchase or Already Owned)
    float buttonW = 250.0f;
    float buttonH = 60.0f;
    float buttonX = popupX + (popupWidth - buttonW) / 2.0f;
    float buttonY = popupY + popupHeight - 90;

    sf::RectangleShape confirmButton({buttonW, buttonH});
    confirmButton.setPosition({buttonX, buttonY});

    bool canAfford = (playerRef->getGold() >= buff.price && !buff.purchased);

    if (buff.purchased)
    {
        confirmButton.setFillColor(sf::Color(100, 200, 100, static_cast<std::uint8_t>(buffDetailAlpha))); // Green = owned
    }
    else if (!canAfford)
    {
        confirmButton.setFillColor(sf::Color(100, 100, 100, static_cast<std::uint8_t>(buffDetailAlpha))); // Gray
    }
    else if (hoveredBuffConfirmButton)
    {
        confirmButton.setFillColor(sf::Color(0, 200, 0, static_cast<std::uint8_t>(buffDetailAlpha))); // Bright green
    }
    else
    {
        confirmButton.setFillColor(sf::Color(0, 150, 0, static_cast<std::uint8_t>(buffDetailAlpha))); // Green
    }
    confirmButton.setOutlineColor(sf::Color(0, 100, 0, static_cast<std::uint8_t>(buffDetailAlpha)));
    confirmButton.setOutlineThickness(3.0f);
    window.draw(confirmButton);

    std::string buttonText;
    if (buff.purchased)
        buttonText = "ALREADY OWNED";
    else if (!canAfford)
        buttonText = "NOT ENOUGH BTC";
    else
        buttonText = "PURCHASE BUFF";

    sf::Text confirmText(font, buttonText, 24);
    confirmText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(buffDetailAlpha)));
    confirmText.setStyle(sf::Text::Bold);
    sf::FloatRect confirmBounds = confirmText.getLocalBounds();
    confirmText.setOrigin({confirmBounds.size.x / 2.0f, confirmBounds.size.y / 2.0f});
    confirmText.setPosition({buttonX + buttonW / 2.0f, buttonY + buttonH / 2.0f});
    window.draw(confirmText);

    // Close instruction
    sf::Text closeText(font, "Click outside to close", 16);
    closeText.setFillColor(sf::Color(120, 120, 120, static_cast<std::uint8_t>(buffDetailAlpha)));
    sf::FloatRect closeBounds = closeText.getLocalBounds();
    closeText.setOrigin({closeBounds.size.x / 2.0f, 0});
    closeText.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 25});
    window.draw(closeText);
}

// Draw gacha result popup
void ShopScreen::drawGachaResultPopup(sf::RenderWindow &window, sf::Font &font)
{
    if (!showGachaResult || !gachaResultCard)
        return;

    sf::Vector2u windowSize = window.getSize();

    // Semi-transparent background overlay
    sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
    overlay.setFillColor(sf::Color(0, 0, 0, 200));
    window.draw(overlay);

    // Popup panel (center, larger for dramatic effect)
    float popupWidth = 600;
    float popupHeight = 700;
    float popupX = (windowSize.x - popupWidth) / 2.0f;
    float popupY = (windowSize.y - popupHeight) / 2.0f;

    // Get rarity color
    sf::Color rarityColor;
    std::string rarityName;
    switch (gachaResultCard->getRarity())
    {
    case Card::CardRarity::COMMON:
        rarityColor = sf::Color(150, 150, 150);
        rarityName = "COMMON";
        break;
    case Card::CardRarity::UNCOMMON:
        rarityColor = sf::Color(50, 200, 50);
        rarityName = "UNCOMMON";
        break;
    case Card::CardRarity::RARE:
        rarityColor = sf::Color(50, 100, 255);
        rarityName = "RARE";
        break;
    case Card::CardRarity::EPIC:
        rarityColor = sf::Color(150, 50, 200);
        rarityName = "EPIC";
        break;
    case Card::CardRarity::LEGENDARY:
        rarityColor = sf::Color(255, 180, 0);
        rarityName = "LEGENDARY";
        break;
    }

    // Popup background with rarity-colored border
    sf::RectangleShape popup({popupWidth, popupHeight});
    popup.setPosition({popupX, popupY});
    popup.setFillColor(sf::Color(240, 240, 230));
    popup.setOutlineColor(rarityColor);
    popup.setOutlineThickness(8.0f);
    window.draw(popup);

    // Sparkle effect for rare+ cards
    if (gachaResultCard->getRarity() >= Card::CardRarity::RARE)
    {
        // Pulsing glow
        float glowAlpha = 128.0f + 127.0f * std::sin(gachaResultTimer * 5.0f);
        sf::RectangleShape glow({popupWidth + 20, popupHeight + 20});
        glow.setPosition({popupX - 10, popupY - 10});
        glow.setFillColor(sf::Color::Transparent);
        glow.setOutlineThickness(10.0f);
        glow.setOutlineColor(sf::Color(rarityColor.r, rarityColor.g, rarityColor.b, static_cast<std::uint8_t>(glowAlpha)));
        window.draw(glow);
    }

    // Title
    sf::Text titleText(font, "LUCKY DRAW RESULT!", 48);
    titleText.setFillColor(sf::Color(80, 40, 100));
    titleText.setStyle(sf::Text::Bold);
    sf::FloatRect titleBounds = titleText.getLocalBounds();
    titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
    titleText.setPosition({popupX + popupWidth / 2.0f, popupY + 30});
    window.draw(titleText);

    // Rarity banner
    sf::Text rarityText(font, rarityName, 36);
    rarityText.setFillColor(rarityColor);
    rarityText.setStyle(sf::Text::Bold);
    sf::FloatRect rarityBounds = rarityText.getLocalBounds();
    rarityText.setOrigin({rarityBounds.size.x / 2.0f, 0});
    rarityText.setPosition({popupX + popupWidth / 2.0f, popupY + 100});
    window.draw(rarityText);

    // Card name
    sf::Text nameText(font, gachaResultCard->getName(), 32);
    nameText.setFillColor(sf::Color(0, 0, 0));
    nameText.setStyle(sf::Text::Bold);
    sf::FloatRect nameBounds = nameText.getLocalBounds();
    nameText.setOrigin({nameBounds.size.x / 2.0f, 0});
    nameText.setPosition({popupX + popupWidth / 2.0f, popupY + 160});
    window.draw(nameText);

    // Card icon (large)
    sf::Sprite *iconSprite = gachaResultCard->getIconSprite();
    if (iconSprite)
    {
        const sf::Texture &iconTexture = iconSprite->getTexture();
        sf::Vector2u iconSize = iconTexture.getSize();

        float iconScale = 250.0f / iconSize.x;
        iconSprite->setScale({iconScale, iconScale});
        iconSprite->setPosition({popupX + (popupWidth - 250) / 2.0f, popupY + 220});
        window.draw(*iconSprite);
    }

    // Card stats
    float yPos = popupY + 490;

    sf::Text costText(font, "Cost: " + std::to_string(gachaResultCard->getEnergyCost()) + " Energy", 24);
    costText.setFillColor(sf::Color(0, 0, 0));
    sf::FloatRect costBounds = costText.getLocalBounds();
    costText.setOrigin({costBounds.size.x / 2.0f, 0});
    costText.setPosition({popupX + popupWidth / 2.0f, yPos});
    window.draw(costText);
    yPos += 35;

    // Description
    sf::Text descText(font, gachaResultCard->getDescription(), 18);
    descText.setFillColor(sf::Color(60, 60, 60));
    sf::FloatRect descBounds = descText.getLocalBounds();
    descText.setOrigin({descBounds.size.x / 2.0f, 0});
    descText.setPosition({popupX + popupWidth / 2.0f, yPos});
    window.draw(descText);

    // Added to deck message
    sf::Text addedText(font, "Added to your deck!", 28);
    addedText.setFillColor(sf::Color(0, 150, 0));
    addedText.setStyle(sf::Text::Bold);
    sf::FloatRect addedBounds = addedText.getLocalBounds();
    addedText.setOrigin({addedBounds.size.x / 2.0f, 0});
    addedText.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 100});
    window.draw(addedText);

    // Click to continue
    sf::Text clickText(font, "Click anywhere to continue", 20);
    clickText.setFillColor(sf::Color(120, 120, 120));
    sf::FloatRect clickBounds = clickText.getLocalBounds();
    clickText.setOrigin({clickBounds.size.x / 2.0f, 0});
    clickText.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 50});
    window.draw(clickText);
}
