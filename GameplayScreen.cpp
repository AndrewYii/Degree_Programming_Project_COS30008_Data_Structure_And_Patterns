#include "GameplayScreen.h"
#include "Game.h"
#include <iostream>
#include <cmath>

// Constructor [Implementation]
GameplayScreen::GameplayScreen(Game *gameInstance) : Screen(gameInstance), gameMap(static_cast<Map *>(0)), playerRef(static_cast<Player *>(0)), mousePressed(false), hoveredElement(-1), hoveredRoom(static_cast<Room *>(0)), viewingDeck(false), deckScrollOffset(0.0f), showCardDetail(false), selectedCardForDetail(nullptr), cardDetailAlpha(0.0f), showStatsDetail(false), statsDetailAlpha(0.0f),
                                                     showScenarioDetail(false), scenarioDetailAlpha(0.0f), deckIconSprite(nullptr), redeemIconSprite(nullptr), showRedeemOverlay(false), redeemCodeInput(""), redeemOverlayAlpha(0.0f), redeemInputActive(false), showRedeemResult(false), redeemResultMessage(""), redeemSuccess(false), redeemResultAlpha(0.0f), showBuffDetail(false), buffDetailAlpha(0.0f), buffScrollOffset(0.0f), selectedBuffName(""), cameraOffset(0, 0), currentFrame(0), animationTimer(0.0f), frameTime(0.5f), wasGenerating(false)
{
  std::cout << "[GameplayScreen] Created" << std::endl;
  loadBuffTextures();
  if (deckIconTexture.loadFromFile("asset/icon/deck.png"))
  {
    deckIconSprite = new sf::Sprite(deckIconTexture);
    std::cout << "[GameplayScreen] Deck icon loaded" << std::endl;
  }
  else
  {
    std::cout << "[GameplayScreen] ERROR: Failed to load asset/icon/deck.png" << std::endl;
  }

  // Load redeem icon (hack icon)
  if (redeemIconTexture.loadFromFile("asset/icon/hack.png"))
  {
    redeemIconSprite = new sf::Sprite(redeemIconTexture);
    std::cout << "[GameplayScreen] Redeem icon loaded (hack.png)" << std::endl;
  }
  else
  {
    std::cout << "[GameplayScreen] ERROR: Failed to load asset/icon/hack.png" << std::endl;
  }
}

// Destructor [Implementation]
GameplayScreen::~GameplayScreen()
{
  std::cout << "[GameplayScreen] Destroyed" << std::endl;
  delete gameMap;
  if (deckIconSprite)
  {
    delete deckIconSprite;
  }
  if (redeemIconSprite)
  {
    delete redeemIconSprite;
  }
}

// Navigate to this screen [Implementation]
void GameplayScreen::onEnter()
{
  std::cout << "[GameplayScreen] Entered" << std::endl;

  animationTimer = 0.0f;
  currentFrame = 0;
  wasGenerating = false;

  // Get player reference from game
  playerRef = &game->getPlayer();

  sf::RenderWindow *window = game->getWindow();

  if (!gameMap)
  {
    gameMap = new Map();

    sf::Vector2u windowSize = window->getSize();
    float mapWidth = static_cast<float>(windowSize.x);
    float mapHeight = static_cast<float>(windowSize.y) * 0.60f;

    gameMap->generateMap(3, 2, mapWidth, mapHeight, window, &game->getFont());
    gameMap->printMapTree();
  }

  float mapViewHeight = static_cast<float>(window->getSize().y) * 0.60f;
  gameView = sf::View(sf::FloatRect({0, 0}, {static_cast<float>(window->getSize().x), mapViewHeight}));

  animationTimer = 0.0f;
  currentFrame = 0;

  playerRef->refreshBaseCost();
  std::cout << "[GameplayScreen] Energy restored to max: " << playerRef->getMaxBaseCost() << std::endl;

  std::cout << "[GameplayScreen] BINARY TREE map generated with player: " << playerRef->getName() << std::endl;
}

// Exit from this screen [Implementation]
void GameplayScreen::onExit()
{
  std::cout << "[GameplayScreen] Exited" << std::endl;
}

// Handle events [Implementation]
void GameplayScreen::handleEvents()
{
  sf::RenderWindow *window = game->getWindow();

  while (const std::optional event = window->pollEvent())
  {
    if (event->is<sf::Event::Closed>())
    {
      std::cout << "[Gameplay] Close event" << std::endl;
      game->quit();
    }

    // ESC to close popups/deck or return to main menu
    if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
    {
      if (keyPressed->code == sf::Keyboard::Key::Escape)
      {
        if (showRedeemOverlay)
        {
          std::cout << "[Gameplay] ESC pressed - closing redeem overlay" << std::endl;
          showRedeemOverlay = false;
          redeemOverlayAlpha = 0.0f;
          redeemInputActive = false;
          redeemCodeInput = "";
        }
        else if (showScenarioDetail)
        {
          std::cout << "[Gameplay] ESC pressed - closing scenario detail" << std::endl;
          showScenarioDetail = false;
        }
        else if (showBuffDetail)
        {
          std::cout << "[Gameplay] ESC pressed - closing buff detail" << std::endl;
          showBuffDetail = false;
          buffDetailAlpha = 0.0f;
          selectedBuffName = "";
        }
        else if (showStatsDetail)
        {
          std::cout << "[Gameplay] ESC pressed - closing stats detail" << std::endl;
          showStatsDetail = false;
          buffScrollOffset = 0.0f;
        }
        else if (showCardDetail)
        {
          std::cout << "[Gameplay] ESC pressed - closing card detail" << std::endl;
          showCardDetail = false;
          selectedCardForDetail = nullptr;
        }
        else if (viewingDeck)
        {
          std::cout << "[Gameplay] ESC pressed - closing deck view" << std::endl;
          viewingDeck = false;
        }
      }
      else if (keyPressed->code == sf::Keyboard::Key::Enter && showRedeemOverlay)
      {
        // Submit redeem code
        processRedeemCode(redeemCodeInput);
      }
      else if (keyPressed->code == sf::Keyboard::Key::Backspace && showRedeemOverlay && !redeemCodeInput.empty())
      {
        // Delete last character
        redeemCodeInput.pop_back();
        std::cout << "[Redeem] Backspace pressed | Current input: '" << redeemCodeInput << "'" << std::endl;
      }
    }

    // Text input for redeem code
    if (showRedeemOverlay && redeemInputActive)
    {
      if (const auto *textEntered = event->getIf<sf::Event::TextEntered>())
      {
        char32_t unicode = textEntered->unicode;
        // Filter out control characters (except space which is 32)
        // Backspace is 8, Enter is 13, etc - these are handled by KeyPressed
        if (unicode >= 32 && unicode < 127 && unicode != 127)
        {
          if (redeemCodeInput.length() < 30)
          {
            redeemCodeInput += static_cast<char>(unicode);
            std::cout << "[Redeem] Character entered: '" << static_cast<char>(unicode) << "' | Current input: '" << redeemCodeInput << "'" << std::endl;
          }
        }
      }
    }

    // Handle redeem overlay clicks (submit button or click outside to close)
    if (showRedeemOverlay)
    {
      if (const auto *mousePress = event->getIf<sf::Event::MouseButtonPressed>())
      {
        if (mousePress->button == sf::Mouse::Button::Left)
        {
          sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
          sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);
          sf::Vector2u windowSize = window->getSize();

          // Check submit button
          float popupWidth = 600.0f;
          float popupHeight = 400.0f;
          float popupX = (windowSize.x - popupWidth) / 2.0f;
          float popupY = (windowSize.y - popupHeight) / 2.0f;

          float buttonWidth = 200.0f;
          float buttonHeight = 50.0f;
          float buttonX = popupX + (popupWidth - buttonWidth) / 2.0f;
          float buttonY = popupY + 220;

          sf::FloatRect submitBounds({buttonX, buttonY}, {buttonWidth, buttonHeight});

          if (submitBounds.contains(mousePos))
          {
            // Submit button clicked
            processRedeemCode(redeemCodeInput);
          }
          else
          {
            // Check if clicked outside popup to close
            sf::FloatRect popupBounds({popupX, popupY}, {popupWidth, popupHeight});
            if (!popupBounds.contains(mousePos))
            {
              // Clicked outside - close overlay
              std::cout << "[Redeem] Clicked outside - closing overlay" << std::endl;
              showRedeemOverlay = false;
              redeemOverlayAlpha = 0.0f;
              redeemInputActive = false;
              redeemCodeInput = "";
            }
          }
        }
      }
      continue;
    }

    if (const auto *wheel = event->getIf<sf::Event::MouseWheelScrolled>())
    {
      if (showStatsDetail)
      {
        const Player::RedeemBuffs &buffs = playerRef->getRedeemBuffs();
        int activeBuffCount = 0;
        if (buffs.onePunchMan)
          activeBuffCount++;
        if (buffs.iterator)
          activeBuffCount++;
        if (buffs.singleton)
          activeBuffCount++;
        if (buffs.factory)
          activeBuffCount++;
        if (buffs.adapter)
          activeBuffCount++;
        if (buffs.observer)
          activeBuffCount++;

        float buffSectionHeight = activeBuffCount * 28.0f + 55.0f;
        float popupHeight = 500.0f;
        float contentHeight = 100.0f + 50.0f + buffSectionHeight + 50.0f + 100.0f;
        float visibleHeight = popupHeight - 100.0f;
        float maxScroll = std::max(0.0f, contentHeight - visibleHeight);

        buffScrollOffset -= wheel->delta * 40.0f;
        if (buffScrollOffset < 0.0f)
          buffScrollOffset = 0.0f;
        if (buffScrollOffset > maxScroll)
          buffScrollOffset = maxScroll;
      }
      else if (viewingDeck && playerRef)
      {
        sf::Vector2u windowSize = window->getSize();
        float panelWidth = windowSize.x * 0.8f;
        float panelHeight = windowSize.y * 0.8f;
        float cardWidth = 140.0f;
        float cardHeight = 200.0f;
        float cardSpacing = 20.0f;
        int cardsPerRow = static_cast<int>((panelWidth - 40) / (cardWidth + cardSpacing));

        const std::vector<Card *> &deck = playerRef->getMasterDeck();
        std::map<std::string, std::map<int, int>> cardCounts;

        for (Card *card : deck)
        {
          if (card)
          {
            std::string name = card->getName();
            int upgradeLevel = card->isUpgraded() ? 1 : 0;
            cardCounts[name][upgradeLevel]++;
          }
        }

        int totalUniqueCards = 0;
        for (const auto &namePair : cardCounts)
        {
          totalUniqueCards += static_cast<int>(namePair.second.size());
        }

        int rows = (totalUniqueCards + cardsPerRow - 1) / cardsPerRow;
        float totalHeight = rows * (cardHeight + cardSpacing);
        float visibleHeight = panelHeight - 170.0f;
        float maxScroll = std::max(0.0f, totalHeight - visibleHeight);

        deckScrollOffset -= wheel->delta * 60.0f;
        if (deckScrollOffset < 0.0f)
          deckScrollOffset = 0.0f;
        if (deckScrollOffset > maxScroll)
          deckScrollOffset = maxScroll;
      }
    }

    if (!showRedeemOverlay)
    {
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
}

// Update logic [Implementation]
void GameplayScreen::update(float deltaTime)
{
  sf::RenderWindow *window = game->getWindow();
  sf::Vector2u windowSize = window->getSize();
  sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
  sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos, gameView);

  // Update map floating animation
  if (gameMap)
  {
    gameMap->update(deltaTime);
  }

  // Check generation state and reset animation when generation starts or ends
  bool isGenerating = (gameMap && gameMap->isGenerating());

  // Reset animation when generation starts (wasGenerating false -> true)
  if (!wasGenerating && isGenerating)
  {
    // Generation just started - reset to prevent any accumulated time
    animationTimer = 0.0f;
    currentFrame = 0;
  }

  // Reset animation when generation finishes (wasGenerating true -> false)
  if (wasGenerating && !isGenerating)
  {
    // Generation just finished - reset to prevent cramping
    animationTimer = 0.0f;
    currentFrame = 0;
  }

  wasGenerating = isGenerating;

  // Update character animation ONLY when not generating map (time does not accumulate during generation)
  if (!isGenerating)
  {
    animationTimer += deltaTime;
    if (animationTimer >= frameTime)
    {
      animationTimer -= frameTime;
      currentFrame = (currentFrame + 1) % 4; // Cycle through 4 frames
    }
  }
  // Note: When generating, animationTimer does NOT increase, so no time is accumulated

  // Update player entity (but not during generation to be safe)
  if (playerRef && !isGenerating)
  {
    playerRef->update(deltaTime);
  }

  // Animate card detail popup fade
  if (showCardDetail && cardDetailAlpha < 255)
  {
    cardDetailAlpha += FADE_SPEED * deltaTime;
    if (cardDetailAlpha > 255)
      cardDetailAlpha = 255;
  }
  else if (!showCardDetail && cardDetailAlpha > 0)
  {
    cardDetailAlpha -= FADE_SPEED * deltaTime;
    if (cardDetailAlpha < 0)
      cardDetailAlpha = 0;
  }

  // Animate stats detail popup fade
  if (showStatsDetail && statsDetailAlpha < 255)
  {
    statsDetailAlpha += FADE_SPEED * deltaTime;
    if (statsDetailAlpha > 255)
      statsDetailAlpha = 255;
  }
  else if (!showStatsDetail && statsDetailAlpha > 0)
  {
    statsDetailAlpha -= FADE_SPEED * deltaTime;
    if (statsDetailAlpha < 0)
      statsDetailAlpha = 0;
  }

  // Animate scenario detail popup fade
  if (showScenarioDetail && scenarioDetailAlpha < 255)
  {
    scenarioDetailAlpha += FADE_SPEED * deltaTime;
    if (scenarioDetailAlpha > 255)
      scenarioDetailAlpha = 255;
  }
  else if (!showScenarioDetail && scenarioDetailAlpha > 0)
  {
    scenarioDetailAlpha -= FADE_SPEED * deltaTime;
    if (scenarioDetailAlpha < 0)
      scenarioDetailAlpha = 0;
  }

  // Animate redeem result popup fade
  if (showRedeemResult && redeemResultAlpha < 255)
  {
    redeemResultAlpha += FADE_SPEED * deltaTime;
    if (redeemResultAlpha > 255)
      redeemResultAlpha = 255;
  }
  else if (!showRedeemResult && redeemResultAlpha > 0)
  {
    redeemResultAlpha -= FADE_SPEED * deltaTime;
    if (redeemResultAlpha < 0)
      redeemResultAlpha = 0;
  }

  // Animate buff detail popup fade
  if (showBuffDetail && buffDetailAlpha < 255)
  {
    buffDetailAlpha += FADE_SPEED * deltaTime;
    if (buffDetailAlpha > 255)
      buffDetailAlpha = 255;
  }
  else if (!showBuffDetail && buffDetailAlpha > 0)
  {
    buffDetailAlpha -= FADE_SPEED * deltaTime;
    if (buffDetailAlpha < 0)
      buffDetailAlpha = 0;
  }

  // Check if clicking redeem result popup to close
  if (showRedeemResult && mousePressed)
  {
    sf::Vector2f uiMousePos = window->mapPixelToCoords(mousePixelPos);
    showRedeemResult = false;
    mousePressed = false;
  }

  // Check if clicking buff detail popup to close
  if (showBuffDetail && mousePressed)
  {
    sf::Vector2f uiMousePos = window->mapPixelToCoords(mousePixelPos);
    showBuffDetail = false;
    mousePressed = false;
  }

  // Check if clicking scenario detail popup to close
  if (showScenarioDetail && mousePressed)
  {
    sf::Vector2f uiMousePos = window->mapPixelToCoords(mousePixelPos);

    // Popup bounds
    float popupW = 600.0f;
    float popupH = 550.0f;
    float popupX = (windowSize.x - popupW) / 2.0f;
    float popupY = (windowSize.y - popupH) / 2.0f;
    sf::FloatRect popupBounds({popupX, popupY}, {popupW, popupH});

    // If clicking outside popup, close it
    if (!popupBounds.contains(uiMousePos))
    {
      showScenarioDetail = false;
      mousePressed = false;
    }
  }

  // Check if clicking stats detail popup to close or open scenario detail
  if (showStatsDetail && mousePressed)
  {
    sf::Vector2f uiMousePos = window->mapPixelToCoords(mousePixelPos);

    float popupW = 600.0f;
    float popupH = 500.0f;
    float popupX = (windowSize.x - popupW) / 2.0f;
    float popupY = (windowSize.y - popupH) / 2.0f;
    sf::FloatRect popupBounds({popupX, popupY}, {popupW, popupH});

    float leftMargin = popupX + 40;
    float yPos = popupY + 100 - buffScrollOffset;

    yPos += 50;
    yPos += 50;
    yPos += 60;

    const Player::RedeemBuffs &buffs = playerRef->getRedeemBuffs();
    int activeBuffCount = 0;
    if (buffs.onePunchMan)
      activeBuffCount++;
    if (buffs.iterator)
      activeBuffCount++;
    if (buffs.singleton)
      activeBuffCount++;
    if (buffs.factory)
      activeBuffCount++;
    if (buffs.adapter)
      activeBuffCount++;
    if (buffs.observer)
      activeBuffCount++;

    float contentTop = popupY + 80;
    float contentBottom = popupY + popupH - 60;
    bool clickedOnBuff = false;

    if (activeBuffCount > 0)
    {
      float buffYPos = yPos;
      buffYPos += 35;

      float buffIndent = leftMargin + 20;

      if (buffs.onePunchMan)
      {
        sf::Text buffText(game->getFont(), "One Punch Man: MASSIVE damage", 20);
        sf::FloatRect buffBounds = buffText.getLocalBounds();
        sf::FloatRect buffClickArea({buffIndent, buffYPos}, {buffBounds.size.x, 28.0f});

        if (buffClickArea.contains(uiMousePos) && buffYPos >= contentTop && buffYPos <= contentBottom)
        {
          std::cout << "[Gameplay] One Punch Man buff clicked - showing detail" << std::endl;
          selectedBuffName = "One Punch Man";
          showBuffDetail = true;
          buffDetailAlpha = 0.0f;
          clickedOnBuff = true;
        }
        buffYPos += 28;
      }

      if (buffs.iterator)
      {
        sf::Text buffText(game->getFont(), "Iterator: All cards cost 1 CPU", 20);
        sf::FloatRect buffBounds = buffText.getLocalBounds();
        sf::FloatRect buffClickArea({buffIndent, buffYPos}, {buffBounds.size.x, 28.0f});

        if (buffClickArea.contains(uiMousePos) && buffYPos >= contentTop && buffYPos <= contentBottom)
        {
          std::cout << "[Gameplay] Iterator buff clicked - showing detail" << std::endl;
          selectedBuffName = "Iterator";
          showBuffDetail = true;
          buffDetailAlpha = 0.0f;
          clickedOnBuff = true;
        }
        buffYPos += 28;
      }

      if (buffs.singleton)
      {
        sf::Text buffText(game->getFont(), "Singleton: Green slot (no CPU cost)", 20);
        sf::FloatRect buffBounds = buffText.getLocalBounds();
        sf::FloatRect buffClickArea({buffIndent, buffYPos}, {buffBounds.size.x, 28.0f});

        if (buffClickArea.contains(uiMousePos) && buffYPos >= contentTop && buffYPos <= contentBottom)
        {
          std::cout << "[Gameplay] Singleton buff clicked - showing detail" << std::endl;
          selectedBuffName = "Singleton";
          showBuffDetail = true;
          buffDetailAlpha = 0.0f;
          clickedOnBuff = true;
        }
        buffYPos += 28;
      }

      if (buffs.factory)
      {
        sf::Text buffText(game->getFont(), "Factory: 3 copies of random card", 20);
        sf::FloatRect buffBounds = buffText.getLocalBounds();
        sf::FloatRect buffClickArea({buffIndent, buffYPos}, {buffBounds.size.x, 28.0f});

        if (buffClickArea.contains(uiMousePos) && buffYPos >= contentTop && buffYPos <= contentBottom)
        {
          std::cout << "[Gameplay] Factory buff clicked - showing detail" << std::endl;
          selectedBuffName = "Factory";
          showBuffDetail = true;
          buffDetailAlpha = 0.0f;
          clickedOnBuff = true;
        }
        buffYPos += 28;
      }

      if (buffs.adapter)
      {
        sf::Text buffText(game->getFont(), "Adapter: Position-free bonuses", 20);
        sf::FloatRect buffBounds = buffText.getLocalBounds();
        sf::FloatRect buffClickArea({buffIndent, buffYPos}, {buffBounds.size.x, 28.0f});

        if (buffClickArea.contains(uiMousePos) && buffYPos >= contentTop && buffYPos <= contentBottom)
        {
          std::cout << "[Gameplay] Adapter buff clicked - showing detail" << std::endl;
          selectedBuffName = "Adapter";
          showBuffDetail = true;
          buffDetailAlpha = 0.0f;
          clickedOnBuff = true;
        }
        buffYPos += 28;
      }

      if (buffs.observer)
      {
        sf::Text buffText(game->getFont(), "Observer: Purple slot (3x activation)", 20);
        sf::FloatRect buffBounds = buffText.getLocalBounds();
        sf::FloatRect buffClickArea({buffIndent, buffYPos}, {buffBounds.size.x, 28.0f});

        if (buffClickArea.contains(uiMousePos) && buffYPos >= contentTop && buffYPos <= contentBottom)
        {
          std::cout << "[Gameplay] Observer buff clicked - showing detail" << std::endl;
          selectedBuffName = "Observer";
          showBuffDetail = true;
          buffDetailAlpha = 0.0f;
          clickedOnBuff = true;
        }
        buffYPos += 28;
      }

      yPos += 35;
      yPos += activeBuffCount * 28;
      yPos += 20;
    }

    if (clickedOnBuff)
    {
      mousePressed = false;
      return;
    }

    float scenarioY = yPos;

    std::string fullScenario = playerRef->getScenarioDescription();
    std::string scenarioName = fullScenario.substr(0, fullScenario.find('\n'));
    std::string scenarioDisplay = "Scenario: " + scenarioName;
    sf::Text tempText(game->getFont(), scenarioDisplay, 24);
    sf::FloatRect textBounds = tempText.getLocalBounds();

    sf::FloatRect scenarioBounds({leftMargin, scenarioY}, {textBounds.size.x, 35.0f});

    if (scenarioBounds.contains(uiMousePos) && scenarioY >= contentTop && scenarioY <= contentBottom)
    {
      std::cout << "[Gameplay] Scenario name clicked - showing detail" << std::endl;
      showScenarioDetail = true;
      scenarioDetailAlpha = 0.0f;
      mousePressed = false;
    }
    else if (!popupBounds.contains(uiMousePos))
    {
      showStatsDetail = false;
      buffScrollOffset = 0.0f;
      mousePressed = false;
    }

    return;
  }

  if (viewingDeck || showStatsDetail)
  {
    // Only handle deck-related interactions
    sf::Vector2f uiMousePos = window->mapPixelToCoords(mousePixelPos);

    // Check if clicking card detail popup close
    if (showCardDetail && mousePressed)
    {
      // Popup bounds
      float popupW = 500.0f;
      float popupH = 600.0f;
      float popupX = (windowSize.x - popupW) / 2.0f;
      float popupY = (windowSize.y - popupH) / 2.0f;
      sf::FloatRect popupBounds({popupX, popupY}, {popupW, popupH});

      // If clicking outside popup, close it
      if (!popupBounds.contains(uiMousePos))
      {
        showCardDetail = false;
        selectedCardForDetail = nullptr;
        mousePressed = false;
      }
    }
    else if (!showCardDetail && mousePressed)
    {
      // Check if clicked on a card in deck overlay
      if (playerRef)
      {
        const std::vector<Card *> &deck = playerRef->getMasterDeck();

        // Build card grid (same logic as drawDeckOverlay)
        std::map<std::string, Card *> cardExamples;
        std::map<std::string, int> cardCounts;
        for (Card *card : deck)
        {
          if (card)
          {
            std::string name = card->getName();
            cardCounts[name]++;
            if (cardExamples.find(name) == cardExamples.end())
            {
              cardExamples[name] = card;
            }
          }
        }

        float panelX = windowSize.x * 0.1f;
        float panelY = windowSize.y * 0.1f;
        float panelWidth = windowSize.x * 0.8f;
        float cardStartY = panelY + 120;
        float cardWidth = 140.0f;
        float cardHeight = 200.0f;
        float cardSpacing = 20.0f;
        int cardsPerRow = static_cast<int>((panelWidth - 40) / (cardWidth + cardSpacing));

        int index = 0;
        bool clickedCard = false;
        for (const auto &pair : cardCounts)
        {
          std::string cardName = pair.first;
          Card *card = cardExamples[cardName];

          if (!card)
            continue;

          int row = index / cardsPerRow;
          int col = index % cardsPerRow;

          float cardX = panelX + 20 + col * (cardWidth + cardSpacing);
          float cardY = cardStartY + row * (cardHeight + cardSpacing);

          sf::FloatRect cardBounds({cardX, cardY}, {cardWidth, cardHeight});

          if (cardBounds.contains(uiMousePos))
          {
            selectedCardForDetail = card;
            showCardDetail = true;
            clickedCard = true;
            mousePressed = false;
            std::cout << "[Gameplay] Opened card detail: " << card->getName() << std::endl;
            break;
          }

          index++;
        }

        // If didn't click card, check if clicked outside deck area to close deck view
        if (!clickedCard)
        {
          sf::FloatRect deckArea({windowSize.x * 0.1f, windowSize.y * 0.1f},
                                 {windowSize.x * 0.8f, windowSize.y * 0.8f});
          if (!deckArea.contains(uiMousePos))
          {
            std::cout << "[Gameplay] Clicked outside deck - closing" << std::endl;
            viewingDeck = false;
            mousePressed = false;
          }
        }
      }
    }

    // Early return - don't process map or other UI interactions
    return;
  }

  // Update map hover states (cleans up hover logic)
  if (gameMap)
  {
    gameMap->updateHoverState(mousePos, windowSize);
  }

  // Reset hover state
  hoveredElement = -1;
  hoveredRoom = static_cast<Room *>(0);

  // If any overlay is open, don't check hover for elements behind it
  if (showRedeemOverlay || viewingDeck || showStatsDetail || showCardDetail)
  {
    return; // Skip all hover detection when overlays are active
  }

  // Check settings button (top right corner)
  float settingsSize = windowSize.y * 0.06f;
  float settingsMargin = windowSize.x * 0.02f;
  float settingsX = windowSize.x - settingsMargin - settingsSize;
  float settingsY = settingsMargin;
  sf::FloatRect settingsBounds({settingsX, settingsY}, {settingsSize, settingsSize});

  // Map mouse to world coordinates for settings button (UI layer)
  sf::Vector2f uiMousePos = window->mapPixelToCoords(mousePixelPos);

  // VIEW DECK icon (below settings) - now same size as settings gear
  float deckIconSize = settingsSize; // Match settings gear size
  float deckButtonX = windowSize.x - settingsMargin - deckIconSize;
  float deckButtonY = settingsY + settingsSize + 10.0f;
  sf::FloatRect deckButtonBounds({deckButtonX, deckButtonY}, {deckIconSize, deckIconSize});

  if (deckButtonBounds.contains(uiMousePos))
  {
    hoveredElement = 2;
    if (mousePressed)
    {
      std::cout << "[Gameplay] VIEW DECK clicked" << std::endl;
      viewingDeck = !viewingDeck; // Toggle deck view

      // Load icons for all cards when opening deck
      if (viewingDeck && playerRef)
      {
        const std::vector<Card *> &deck = playerRef->getMasterDeck();
        for (Card *card : deck)
        {
          if (card && card->getIconSprite() == nullptr)
          {
            card->loadIcon();
          }
        }
      }

      mousePressed = false;
    }
  }

  float redeemButtonSize = settingsSize;
  float redeemButtonX = windowSize.x - settingsMargin - redeemButtonSize;
  float redeemButtonY = settingsY + settingsSize * 2 + 20.0f;
  sf::FloatRect redeemButtonBounds({redeemButtonX, redeemButtonY}, {redeemButtonSize, redeemButtonSize});

  if (redeemButtonBounds.contains(uiMousePos))
  {
    hoveredElement = 3;
    if (mousePressed)
    {
      std::cout << "[Gameplay] REDEEM CODE clicked - opening overlay" << std::endl;
      showRedeemOverlay = true;
      redeemOverlayAlpha = 0.0f;
      redeemInputActive = true;
      redeemCodeInput = "";
      std::cout << "[Gameplay] Redeem overlay opened - input active: " << (redeemInputActive ? "YES" : "NO") << std::endl;
      mousePressed = false;
    }
  }

  // PLAYER STATS icon (bottom right) - clickable for detail view
  float statsIconSize = windowSize.y * 0.12f;
  float statsIconX = windowSize.x - settingsMargin - statsIconSize;
  float statsIconY = windowSize.y - settingsMargin - statsIconSize;
  sf::FloatRect statsIconBounds({statsIconX, statsIconY}, {statsIconSize, statsIconSize});

  if (statsIconBounds.contains(uiMousePos))
  {
    hoveredElement = 4;
    if (mousePressed)
    {
      std::cout << "[Gameplay] STATS icon clicked - showing detail popup" << std::endl;
      showStatsDetail = true;
      statsDetailAlpha = 0.0f; // Start fade-in
      mousePressed = false;
    }
  }

  if (settingsBounds.contains(uiMousePos))
  {
    hoveredElement = 0;
    if (mousePressed)
    {
      std::cout << "[Gameplay] SETTINGS clicked" << std::endl;
      game->changeScreen(Game::ScreenType::SETTINGS);
      mousePressed = false;
    }
  }

  // Check room nodes (game world layer)
  if (gameMap && hoveredElement == -1)
  {
    hoveredRoom = gameMap->findRoomAt(mousePos, windowSize);
    if (hoveredRoom)
    {
      hoveredElement = 1;

      if (mousePressed)
      {
        // Try to move to room
        if (gameMap->canMoveToRoom(hoveredRoom))
        {
          // Check room type BEFORE moving (store type before pointer might become invalid)
          Room::RoomType roomType = hoveredRoom->getType();
          bool isExitRoom = (roomType == Room::RoomType::EXIT);
          bool isStartRoom = (roomType == Room::RoomType::START);
          bool isTutorialRoom = (roomType == Room::RoomType::TUTORIAL);
          bool isCombatRoom = (roomType == Room::RoomType::COMBAT);
          bool isBossRoom = (roomType == Room::RoomType::BOSS);
          bool isTreasureRoom = (roomType == Room::RoomType::TREASURE);
          bool isRestRoom = (roomType == Room::RoomType::REST);
          bool isShopRoom = (roomType == Room::RoomType::SHOP);
          std::string roomName = hoveredRoom->getName();

          gameMap->moveToRoom(hoveredRoom);
          std::cout << "[Gameplay] Moved to: " << roomName << std::endl;

          // Enter START room - links to TUTORIAL
          if (isStartRoom)
          {
            std::cout << "[Gameplay] START room entered! Opening tutorial..." << std::endl;
            game->changeScreen(Game::ScreenType::TUTORIAL);
            mousePressed = false;
            return;
          }

          // Enter TUTORIAL room
          if (isTutorialRoom)
          {
            std::cout << "[Gameplay] TUTORIAL room entered! Learning how to play..." << std::endl;
            game->changeScreen(Game::ScreenType::TUTORIAL);
            mousePressed = false;
            return;
          }

          // Enter COMBAT if it's a combat room
          if (isCombatRoom)
          {
            std::cout << "[Gameplay] COMBAT room entered! Starting battle..." << std::endl;
            playerRef->setIsBossRoom(false); // Normal combat
            game->changeScreen(Game::ScreenType::COMBAT);
            mousePressed = false;
            return; // Exit early - we're switching screens
          }

          // Enter BOSS room (uses same combat screen but with boss enemy)
          if (isBossRoom)
          {
            std::cout << "[Gameplay] BOSS room entered! Starting boss battle..." << std::endl;
            playerRef->setIsBossRoom(true); // Boss combat
            game->changeScreen(Game::ScreenType::COMBAT);
            mousePressed = false;
            return; // Exit early - we're switching screens
          }

          // Enter TREASURE room
          if (isTreasureRoom)
          {
            std::cout << "[Gameplay] TREASURE room entered! Claiming rewards..." << std::endl;
            game->changeScreen(Game::ScreenType::TREASURE);
            mousePressed = false;
            return;
          }

          // Enter REST room
          if (isRestRoom)
          {
            std::cout << "[Gameplay] REST room entered! Time to recover..." << std::endl;
            game->changeScreen(Game::ScreenType::REST);
            mousePressed = false;
            return;
          }

          // Enter SHOP room
          if (isShopRoom)
          {
            std::cout << "[Gameplay] SHOP room entered! Let's buy some cards..." << std::endl;
            game->changeScreen(Game::ScreenType::SHOP);
            mousePressed = false;
            return;
          }

          // Regenerate map for next stage if it was an EXIT room
          if (isExitRoom)
          {
            std::cout << "[Gameplay] EXIT reached! Generating next stage..." << std::endl;
            sf::RenderWindow *mapWindow = game->getWindow();
            sf::Vector2u mapWindowSize = mapWindow->getSize();
            float mapWidth = static_cast<float>(mapWindowSize.x);
            float mapHeight = static_cast<float>(mapWindowSize.y) * 0.60f;

            // Regenerate with simultaneous loading animation
            gameMap->regenerateForNextStage(mapWidth, mapHeight, mapWindow, &game->getFont());
            // Clear hoveredRoom pointer - it's now invalid (dangling pointer to deleted memory)
            hoveredRoom = static_cast<Room *>(0);
          }
        }
        else
        {
          std::cout << "[Gameplay] Cannot move to: " << hoveredRoom->getName() << std::endl;
        }
        mousePressed = false;
      }
    }
  }
}

// Render logic [Implementation]
void GameplayScreen::render(sf::RenderWindow &window, sf::Font &font)
{
  window.clear(Game::COLOR_OFF_WHITE);

  sf::Vector2u windowSize = window.getSize();

  // Create map view that uses top 60% of screen (BIGGER for horizontal tree)
  float mapViewHeight = windowSize.y * 0.60f; // Increased to 60%!
  sf::View mapView(sf::FloatRect({0, 0}, {static_cast<float>(windowSize.x), mapViewHeight}));
  window.setView(mapView);

  // Render map (horizontal tree) in top area
  if (gameMap)
  {
    gameMap->render(window, font);
  }

  // Reset to default view for UI (bottom panels)
  window.setView(window.getDefaultView());

  // LEFT SIDE: Character sprite (bottom 40%, smaller)
  drawCharacterSprite(window, font);

  // CENTER: Room info if hovering (between character and stats)
  if (hoveredRoom)
  {
    drawRoomInfo(window, font, hoveredRoom);
  }

  // RIGHT SIDE: Player stats (bottom 40%, smaller)
  drawPlayerStats(window, font);

  // TOP RIGHT: Settings button
  drawSettingsButton(window);

  // View Deck button
  drawViewDeckButton(window, font);

  // Redeem Code button (below deck button)
  drawRedeemCodeButton(window, font);

  // Deck overlay (on top of everything)
  if (viewingDeck)
  {
    drawDeckOverlay(window, font);

    // Card detail popup (on top of deck overlay)
    if (cardDetailAlpha > 0)
    {
      drawCardDetailPopup(window, font);
    }
  }

  // Redeem code overlay (on top of everything)
  if (showRedeemOverlay)
  {
    drawRedeemOverlay(window, font);
  }

  // Stats detail popup (on top of everything except card detail)
  if (statsDetailAlpha > 0)
  {
    drawStatsDetailPopup(window, font);
  }

  // Scenario detail popup (on top of stats detail popup)
  if (scenarioDetailAlpha > 0)
  {
    drawScenarioDetailPopup(window, font);
  }

  // Buff detail popup (on top of stats detail popup)
  if (buffDetailAlpha > 0)
  {
    drawBuffDetailPopup(window, font);
  }

  // Redeem result popup (on top of everything)
  if (redeemResultAlpha > 0)
  {
    drawRedeemResultPopup(window, font);
  }
}

// Draw redeem result popup
void GameplayScreen::drawRedeemResultPopup(sf::RenderWindow &window, sf::Font &font)
{
  if (!showRedeemResult)
    return;

  sf::Vector2u windowSize = window.getSize();

  // Semi-transparent background overlay
  sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
  overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(redeemResultAlpha * 0.6f)));
  window.draw(overlay);

  // Popup panel (center, smaller than redeem overlay)
  float popupWidth = 400;
  float popupHeight = 250;
  float popupX = (windowSize.x - popupWidth) / 2.0f;
  float popupY = (windowSize.y - popupHeight) / 2.0f;

  sf::RectangleShape popup({popupWidth, popupHeight});
  popup.setPosition({popupX, popupY});

  // Color based on success/failure
  sf::Color bgColor = redeemSuccess ? sf::Color(230, 255, 230, static_cast<std::uint8_t>(redeemResultAlpha))
                                    : sf::Color(255, 230, 230, static_cast<std::uint8_t>(redeemResultAlpha));
  popup.setFillColor(bgColor);

  sf::Color borderColor = redeemSuccess ? sf::Color(0, 180, 0, static_cast<std::uint8_t>(redeemResultAlpha))
                                        : sf::Color(200, 0, 0, static_cast<std::uint8_t>(redeemResultAlpha));
  popup.setOutlineColor(borderColor);
  popup.setOutlineThickness(5.0f);
  window.draw(popup);

  // Title (SUCCESS or FAILED)
  std::string title = redeemSuccess ? "SUCCESS!" : "FAILED";
  sf::Text titleText(font, title, 48);
  titleText.setFillColor(sf::Color(borderColor.r, borderColor.g, borderColor.b, static_cast<std::uint8_t>(redeemResultAlpha)));
  titleText.setStyle(sf::Text::Bold);
  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
  titleText.setPosition({popupX + popupWidth / 2.0f, popupY + 40});
  window.draw(titleText);

  // Message
  sf::Text messageText(font, redeemResultMessage, 24);
  messageText.setFillColor(sf::Color(40, 40, 40, static_cast<std::uint8_t>(redeemResultAlpha)));
  sf::FloatRect messageBounds = messageText.getLocalBounds();
  messageText.setOrigin({messageBounds.size.x / 2.0f, 0});
  messageText.setPosition({popupX + popupWidth / 2.0f, popupY + 120});
  window.draw(messageText);

  // Instruction
  sf::Text instructionText(font, "Click anywhere to continue", 18);
  instructionText.setFillColor(sf::Color(100, 100, 100, static_cast<std::uint8_t>(redeemResultAlpha)));
  sf::FloatRect instructionBounds = instructionText.getLocalBounds();
  instructionText.setOrigin({instructionBounds.size.x / 2.0f, 0});
  instructionText.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 40});
  window.draw(instructionText);
}

// Draw settings button (top right corner)
void GameplayScreen::drawSettingsButton(sf::RenderWindow &window)
{
  sf::Vector2u windowSize = window.getSize();

  float settingsSize = windowSize.y * 0.06f;
  float settingsMargin = windowSize.x * 0.02f;
  float settingsX = windowSize.x - settingsMargin - settingsSize;
  float settingsY = settingsMargin;

  sf::RectangleShape settingsButton({settingsSize, settingsSize});
  settingsButton.setPosition({settingsX, settingsY});

  if (hoveredElement == 0)
  {
    settingsButton.setFillColor(Game::COLOR_BLACK);
    settingsButton.setOutlineColor(Game::COLOR_BLACK);
  }
  else
  {
    settingsButton.setFillColor(sf::Color::White);
    settingsButton.setOutlineColor(Game::COLOR_BLACK);
  }
  settingsButton.setOutlineThickness(3.0f);
  window.draw(settingsButton);

  // Draw gear icon
  sf::Color gearColor = (hoveredElement == 0) ? sf::Color::White : Game::COLOR_BLACK;
  drawGearIcon(window, settingsX + settingsSize / 2, settingsY + settingsSize / 2, settingsSize * 0.35f, gearColor);
}

// Draw character sprite and info (bottom left, smaller)
void GameplayScreen::drawCharacterSprite(sf::RenderWindow &window, sf::Font &font)
{
  if (!playerRef)
    return;

  sf::Vector2u windowSize = window.getSize();
  float margin = windowSize.x * 0.02f;

  float panelWidth = windowSize.x * 0.15f;
  float panelHeight = windowSize.y * 0.30f;
  float panelX = margin;
  float panelY = windowSize.y * 0.65f;

  sf::RectangleShape panel({panelWidth, panelHeight});
  panel.setPosition({panelX, panelY});
  panel.setFillColor(sf::Color::White);
  panel.setOutlineColor(Game::COLOR_BLACK);
  panel.setOutlineThickness(3.0f);
  window.draw(panel);

  // Character name label (smaller)
  float nameSize = windowSize.y * 0.022f;
  sf::Text nameText(font, playerRef->getName(), static_cast<unsigned int>(nameSize));
  nameText.setFillColor(Game::COLOR_BLACK);
  nameText.setStyle(sf::Text::Bold);

  sf::FloatRect nameBounds = nameText.getLocalBounds();
  nameText.setOrigin({nameBounds.size.x / 2.0f, 0});
  nameText.setPosition({panelX + panelWidth / 2.0f, panelY + 8});
  window.draw(nameText);

  // Role label (smaller)
  float roleSize = windowSize.y * 0.018f;
  sf::Text roleText(font, playerRef->getRoleName(), static_cast<unsigned int>(roleSize));
  roleText.setFillColor(Game::COLOR_BLACK);
  roleText.setStyle(sf::Text::Bold);

  sf::FloatRect roleBounds = roleText.getLocalBounds();
  roleText.setOrigin({roleBounds.size.x / 2.0f, 0});
  roleText.setPosition({panelX + panelWidth / 2.0f, panelY + 32});
  window.draw(roleText);

  playerRef->setPosition(panelX + panelWidth / 2.0f, panelY + panelHeight * 0.40f);
  int displayFrame = (gameMap && gameMap->isGenerating()) ? 0 : currentFrame;
  playerRef->setCurrentFrame(displayFrame);
  playerRef->render(window);
}

// Draw player stats icon and info (bottom right, smaller)
void GameplayScreen::drawPlayerStats(sf::RenderWindow &window, sf::Font &font)
{
  if (!playerRef)
    return;

  sf::Vector2u windowSize = window.getSize();
  float margin = windowSize.x * 0.02f;

  float iconSize = windowSize.y * 0.12f;
  float iconX = windowSize.x - margin - iconSize;
  float iconY = windowSize.y - margin - iconSize;

  sf::RectangleShape iconBg({iconSize, iconSize});
  iconBg.setPosition({iconX, iconY});
  iconBg.setFillColor(sf::Color::White);
  iconBg.setOutlineColor(Game::COLOR_BLACK);
  iconBg.setOutlineThickness(3.0f);

  if (hoveredElement == 4)
  {
    iconBg.setFillColor(sf::Color(220, 220, 220));
  }

  window.draw(iconBg);

  sf::Text memoryLabel(font, "MEMORY", static_cast<unsigned int>(windowSize.y * 0.012f));
  memoryLabel.setFillColor(Game::COLOR_BLACK);
  memoryLabel.setStyle(sf::Text::Bold);
  sf::FloatRect memLabelBounds = memoryLabel.getLocalBounds();
  memoryLabel.setPosition({iconX + (iconSize - memLabelBounds.size.x) / 2, iconY + iconSize * 0.05f});
  window.draw(memoryLabel);

  float barWidth = iconSize * 0.85f;
  float barHeight = iconSize * 0.15f;
  float barX = iconX + (iconSize - barWidth) / 2;
  float barY = iconY + iconSize * 0.20f;

  sf::RectangleShape hpBarBg({barWidth, barHeight});
  hpBarBg.setPosition({barX, barY});
  hpBarBg.setFillColor(sf::Color(200, 200, 200));
  hpBarBg.setOutlineColor(Game::COLOR_BLACK);
  hpBarBg.setOutlineThickness(2.0f);
  window.draw(hpBarBg);

  float hpPercent = static_cast<float>(playerRef->getHP()) / playerRef->getMaxHP();
  sf::RectangleShape hpBarFill({barWidth * hpPercent, barHeight});
  hpBarFill.setPosition({barX, barY});

  // Color based on memory percentage
  if (hpPercent > 0.6f)
    hpBarFill.setFillColor(sf::Color(0, 180, 0));
  else if (hpPercent > 0.3f)
    hpBarFill.setFillColor(sf::Color(220, 180, 0));
  else
    hpBarFill.setFillColor(sf::Color(200, 0, 0));

  window.draw(hpBarFill);

  std::string memStr = std::to_string(playerRef->getHP()) + "/" + std::to_string(playerRef->getMaxHP());
  float memTextSize = windowSize.y * 0.016f;
  sf::Text memText(font, memStr, static_cast<unsigned int>(memTextSize));
  memText.setFillColor(sf::Color::White);
  memText.setStyle(sf::Text::Bold);
  sf::FloatRect memBounds = memText.getLocalBounds();
  memText.setPosition({iconX + (iconSize - memBounds.size.x) / 2, barY + 2});
  window.draw(memText);

  sf::Text cpuLabel(font, "CPU", static_cast<unsigned int>(windowSize.y * 0.015f));
  cpuLabel.setFillColor(Game::COLOR_BLACK);
  cpuLabel.setStyle(sf::Text::Bold);
  sf::FloatRect cpuLabelBounds = cpuLabel.getLocalBounds();
  cpuLabel.setPosition({iconX + (iconSize - cpuLabelBounds.size.x) / 2, iconY + iconSize * 0.43f});
  window.draw(cpuLabel);

  float energyCircleRadius = iconSize * 0.16f;
  float energyCircleX = iconX + iconSize / 2;
  float energyCircleY = iconY + iconSize * 0.60f;

  sf::CircleShape energyCircle(energyCircleRadius);
  energyCircle.setOrigin({energyCircleRadius, energyCircleRadius});
  energyCircle.setPosition({energyCircleX, energyCircleY});
  energyCircle.setFillColor(sf::Color(100, 150, 255));
  energyCircle.setOutlineColor(Game::COLOR_BLACK);
  energyCircle.setOutlineThickness(2.0f);
  window.draw(energyCircle);

  // Calculate actual max CPU with scenario modifiers
  int baseCPU = playerRef->getMaxBaseCost();
  int bonusCPU = playerRef->getScenarioModifiers().maxEnergyBonus;
  int actualMaxCPU = baseCPU + bonusCPU;

  std::string energyStr = std::to_string(actualMaxCPU) + "/" + std::to_string(actualMaxCPU);
  float energyTextSize = windowSize.y * 0.018f;
  sf::Text energyText(font, energyStr, static_cast<unsigned int>(energyTextSize));
  energyText.setFillColor(sf::Color::White);
  energyText.setStyle(sf::Text::Bold);
  sf::FloatRect energyBounds = energyText.getLocalBounds();
  energyText.setPosition({energyCircleX - energyBounds.size.x / 2, energyCircleY - energyTextSize / 2 - 2});
  window.draw(energyText);
}

void GameplayScreen::drawRoomInfo(sf::RenderWindow &, sf::Font &, Room *)
{
  return;
}

void GameplayScreen::drawGearIcon(sf::RenderWindow &window, float x, float y, float radius, sf::Color color)
{
  const int teeth = 12;
  const float innerRadius = radius * 0.5f;
  const float outerRadius = radius;

  // Draw outer gear ring
  sf::CircleShape outerCircle(outerRadius, teeth * 4);
  outerCircle.setOrigin({outerRadius, outerRadius});
  outerCircle.setPosition({x, y});
  outerCircle.setFillColor(color);
  outerCircle.setOutlineColor(color);
  outerCircle.setOutlineThickness(1.0f);
  window.draw(outerCircle);

  // Draw teeth
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

  // Draw inner circle (hole)
  sf::CircleShape innerCircle(innerRadius);
  innerCircle.setOrigin({innerRadius, innerRadius});
  innerCircle.setPosition({x, y});
  sf::Color holeColor = (color == sf::Color::White) ? Game::COLOR_BLACK : sf::Color::White;
  innerCircle.setFillColor(holeColor);
  window.draw(innerCircle);

  // Draw center circle
  sf::CircleShape centerCircle(innerRadius * 0.4f);
  centerCircle.setOrigin({innerRadius * 0.4f, innerRadius * 0.4f});
  centerCircle.setPosition({x, y});
  centerCircle.setFillColor(color);
  window.draw(centerCircle);
}

void GameplayScreen::drawViewDeckButton(sf::RenderWindow &window, sf::Font &)
{
  sf::Vector2u windowSize = window.getSize();

  float settingsSize = windowSize.y * 0.06f;
  float settingsMargin = windowSize.x * 0.02f;

  // Icon size (same as settings gear for consistency)
  float iconSize = settingsSize;
  float deckButtonX = windowSize.x - settingsMargin - iconSize;
  float deckButtonY = settingsMargin + settingsSize + 10.0f;

  // Draw icon if loaded
  if (deckIconSprite)
  {
    // Scale icon to fit size
    sf::Vector2u textureSize = deckIconTexture.getSize();
    if (textureSize.x > 0 && textureSize.y > 0)
    {
      float scaleX = iconSize / textureSize.x;
      float scaleY = iconSize / textureSize.y;
      deckIconSprite->setScale({scaleX, scaleY});
      deckIconSprite->setPosition({deckButtonX, deckButtonY});

      // Apply glow effect on hover
      if (hoveredElement == 2)
      {
        // Bright glow when hovered
        deckIconSprite->setColor(sf::Color(255, 255, 255, 255));
      }
      else
      {
        // Slightly dimmed when not hovered
        deckIconSprite->setColor(sf::Color(200, 200, 200, 255));
      }

      window.draw(*deckIconSprite);
    }
  }
}

// Draw redeem code button
void GameplayScreen::drawRedeemCodeButton(sf::RenderWindow &window, sf::Font &font)
{
  sf::Vector2u windowSize = window.getSize();

  float settingsSize = windowSize.y * 0.06f;
  float settingsMargin = windowSize.x * 0.02f;

  // Button size and position (below deck button)
  float buttonSize = settingsSize;
  float buttonX = windowSize.x - settingsMargin - buttonSize;
  float buttonY = settingsMargin + settingsSize * 2 + 20.0f;

  // Draw hack icon sprite as the entire button
  if (redeemIconSprite)
  {
    // Scale to fill entire button
    sf::Vector2u texSize = redeemIconTexture.getSize();
    if (texSize.x > 0 && texSize.y > 0)
    {
      float scaleX = buttonSize / texSize.x;
      float scaleY = buttonSize / texSize.y;
      redeemIconSprite->setScale({scaleX, scaleY});
      redeemIconSprite->setPosition({buttonX, buttonY});

      // Apply glow effect on hover (same as deck icon)
      if (hoveredElement == 3)
      {
        // Bright glow when hovered
        redeemIconSprite->setColor(sf::Color(255, 255, 255, 255));
      }
      else
      {
        // Slightly dimmed when not hovered
        redeemIconSprite->setColor(sf::Color(200, 200, 200, 255));
      }

      window.draw(*redeemIconSprite);
    }
  }
  else
  {
    // Fallback to colored button with text icon if sprite failed to load
    sf::RectangleShape button({buttonSize, buttonSize});
    button.setPosition({buttonX, buttonY});
    button.setFillColor(sf::Color(200, 100, 200, 255));
    button.setOutlineThickness(3.0f);
    button.setOutlineColor(hoveredElement == 3 ? sf::Color::Yellow : sf::Color::White);
    window.draw(button);

    sf::Text giftIcon(font, "</>", 24);
    giftIcon.setFillColor(sf::Color::White);
    giftIcon.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = giftIcon.getLocalBounds();
    giftIcon.setPosition({buttonX + (buttonSize - textBounds.size.x) / 2, buttonY + (buttonSize - textBounds.size.y) / 2 - 5});
    window.draw(giftIcon);
  }
}

// Draw deck overlay
void GameplayScreen::drawDeckOverlay(sf::RenderWindow &window, sf::Font &font)
{
  if (!playerRef)
  {
    std::cout << "[Gameplay] Cannot draw deck - playerRef is null!" << std::endl;
    return;
  }

  sf::Vector2u windowSize = window.getSize();

  // Debug: Log deck size
  const std::vector<Card *> &deck = playerRef->getMasterDeck();
  std::cout << "[Gameplay] Drawing deck overlay - deck size: " << deck.size() << std::endl;

  // Semi-transparent background
  sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
  overlay.setFillColor(sf::Color(0, 0, 0, 180));
  window.draw(overlay);

  // Deck panel
  float panelWidth = windowSize.x * 0.8f;
  float panelHeight = windowSize.y * 0.8f;
  float panelX = windowSize.x * 0.1f;
  float panelY = windowSize.y * 0.1f;

  sf::RectangleShape panel({panelWidth, panelHeight});
  panel.setPosition({panelX, panelY});
  panel.setFillColor(sf::Color(240, 240, 230));
  panel.setOutlineColor(Game::COLOR_BLACK);
  panel.setOutlineThickness(4.0f);
  window.draw(panel);

  // Title
  sf::Text title(font, "DECK", 48);
  title.setFillColor(Game::COLOR_BLACK);
  title.setStyle(sf::Text::Bold);
  sf::FloatRect titleBounds = title.getLocalBounds();
  title.setOrigin({titleBounds.size.x / 2.0f, 0});
  title.setPosition({windowSize.x / 2.0f, panelY + 20});
  window.draw(title);

  // Deck count (use deck variable from above)
  sf::Text countText(font, "Cards: " + std::to_string(deck.size()), 24);
  countText.setFillColor(Game::COLOR_BLACK);
  countText.setPosition({panelX + 20, panelY + 80});
  window.draw(countText);

  std::map<std::string, std::map<int, int>> cardCounts;
  std::map<std::string, std::map<int, Card *>> cardExamples;

  for (Card *card : deck)
  {
    if (card)
    {
      std::string name = card->getName();
      int upgradeLevel = card->isUpgraded() ? 1 : 0;
      cardCounts[name][upgradeLevel]++;
      if (cardExamples[name].find(upgradeLevel) == cardExamples[name].end())
      {
        cardExamples[name][upgradeLevel] = card;
      }
    }
  }

  float cardStartY = panelY + 120;
  float cardWidth = 140.0f;
  float cardHeight = 200.0f;
  float cardSpacing = 20.0f;
  int cardsPerRow = static_cast<int>((panelWidth - 40) / (cardWidth + cardSpacing));

  int index = 0;
  for (const auto &namePair : cardCounts)
  {
    std::string cardName = namePair.first;

    for (const auto &upgradePair : namePair.second)
    {
      int upgradeLevel = upgradePair.first;
      int count = upgradePair.second;
      Card *card = cardExamples[cardName][upgradeLevel];

      if (!card)
        continue;

      int row = index / cardsPerRow;
      int col = index % cardsPerRow;

      float cardX = panelX + 20 + col * (cardWidth + cardSpacing);
      float cardY = cardStartY + row * (cardHeight + cardSpacing) - deckScrollOffset;

      if (cardY + cardHeight < panelY + 110 || cardY > panelY + panelHeight - 50)
      {
        index++;
        continue;
      }

      sf::RectangleShape cardRect({cardWidth, cardHeight});
      cardRect.setPosition({cardX, cardY});
      cardRect.setFillColor(sf::Color::White);

      sf::Color borderColor;
      switch (card->getType())
      {
      case Card::CardType::ATTACK:
        borderColor = sf::Color(255, 255, 0);
        break;
      case Card::CardType::DEFENSE:
        borderColor = sf::Color(0, 150, 255);
        break;
      case Card::CardType::SKILL:
        borderColor = sf::Color(0, 255, 100);
        break;
      case Card::CardType::POWER:
        borderColor = sf::Color(255, 0, 255);
        break;
      default:
        borderColor = Game::COLOR_BLACK;
      }

      cardRect.setOutlineColor(borderColor);
      cardRect.setOutlineThickness(3.0f);
      window.draw(cardRect);

      sf::Sprite *iconSprite = card->getIconSprite();
      if (iconSprite)
      {
        const sf::Texture &iconTexture = iconSprite->getTexture();
        sf::Vector2u iconSize = iconTexture.getSize();

        float iconPadding = 20.0f;
        float availableWidth = cardWidth - (iconPadding * 2);
        float availableHeight = cardHeight * 0.5f;

        float scaleX = availableWidth / iconSize.x;
        float scaleY = availableHeight / iconSize.y;
        float scale = std::min(scaleX, scaleY);

        iconSprite->setScale({scale, scale});

        float scaledWidth = iconSize.x * scale;
        float scaledHeight = iconSize.y * scale;
        float iconX = cardX + (cardWidth - scaledWidth) / 2.0f;
        float iconY = cardY + (cardHeight - scaledHeight) / 2.0f;

        iconSprite->setPosition({iconX, iconY});
        window.draw(*iconSprite);
      }

      sf::CircleShape costCircle(18.0f);
      costCircle.setPosition({cardX + 10, cardY + 10});
      costCircle.setFillColor(sf::Color(100, 150, 255));
      costCircle.setOutlineColor(Game::COLOR_BLACK);
      costCircle.setOutlineThickness(2.0f);
      window.draw(costCircle);

      sf::Text costText(font, std::to_string(card->getEnergyCost()), 20);
      costText.setFillColor(sf::Color::White);
      costText.setStyle(sf::Text::Bold);
      sf::FloatRect costBounds = costText.getLocalBounds();
      costText.setOrigin({costBounds.size.x / 2.0f, costBounds.size.y / 2.0f});
      costText.setPosition({cardX + 10 + 18, cardY + 10 + 18});
      window.draw(costText);

      if (upgradeLevel > 0)
      {
        sf::CircleShape upgradeCircle(12.0f, 5);
        upgradeCircle.setPosition({cardX + cardWidth - 30, cardY + 8});
        upgradeCircle.setFillColor(sf::Color(255, 215, 0));
        upgradeCircle.setOutlineColor(sf::Color(255, 165, 0));
        upgradeCircle.setOutlineThickness(2.0f);
        window.draw(upgradeCircle);

        sf::Text upgradeText(font, "+" + std::to_string(upgradeLevel), 16);
        upgradeText.setFillColor(sf::Color::Black);
        upgradeText.setStyle(sf::Text::Bold);
        sf::FloatRect upgradeBounds = upgradeText.getLocalBounds();
        upgradeText.setOrigin({upgradeBounds.size.x / 2.0f, upgradeBounds.size.y / 2.0f});
        upgradeText.setPosition({cardX + cardWidth - 18, cardY + 20});
        window.draw(upgradeText);
      }

      if (count > 1)
      {
        sf::Text cardCountText(font, "x" + std::to_string(count), 20);
        cardCountText.setFillColor(Game::COLOR_BLACK);
        cardCountText.setStyle(sf::Text::Bold);
        sf::FloatRect countTextBounds = cardCountText.getLocalBounds();
        cardCountText.setOrigin({countTextBounds.size.x / 2.0f, 0});
        cardCountText.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight - 35});
        window.draw(cardCountText);
      }

      index++;
    }
  }

  sf::Text closeText(font, "Press ESC or click outside to close", 20);
  closeText.setFillColor(sf::Color(100, 100, 100));
  sf::FloatRect closeBounds = closeText.getLocalBounds();
  closeText.setOrigin({closeBounds.size.x / 2.0f, 0});
  closeText.setPosition({windowSize.x / 2.0f, panelY + panelHeight - 40});
  window.draw(closeText);
}

// Draw card detail popup
void GameplayScreen::drawCardDetailPopup(sf::RenderWindow &window, sf::Font &font)
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
  popup.setOutlineColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
  popup.setOutlineThickness(4.0f);
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

    // Restore original color
    iconColor.a = 255;
    iconSprite->setColor(iconColor);
  }

  // Card cost
  sf::Text costText(font, "Cost: " + std::to_string(selectedCardForDetail->getEnergyCost()) + " Energy", 24);
  costText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
  costText.setPosition({popupX + 30, popupY + 310});
  window.draw(costText);

  // Card type
  std::string typeStr = "Type: ";
  switch (selectedCardForDetail->getType())
  {
  case Card::CardType::ATTACK:
    typeStr += "Attack";
    break;
  case Card::CardType::DEFENSE:
    typeStr += "Defense";
    break;
  case Card::CardType::SKILL:
    typeStr += "Skill";
    break;
  case Card::CardType::POWER:
    typeStr += "Power";
    break;
  default:
    typeStr += "Unknown";
    break;
  }

  sf::Text typeText(font, typeStr, 24);
  typeText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
  typeText.setPosition({popupX + 30, popupY + 350});
  window.draw(typeText);

  // Rarity
  std::string rarityStr = "Rarity: ";
  sf::Color rarityColor = sf::Color::Black;
  switch (selectedCardForDetail->getRarity())
  {
  case Card::CardRarity::COMMON:
    rarityStr += "Common";
    rarityColor = sf::Color(120, 120, 120); // Gray
    break;
  case Card::CardRarity::UNCOMMON:
    rarityStr += "Uncommon";
    rarityColor = sf::Color(0, 180, 0); // Green
    break;
  case Card::CardRarity::RARE:
    rarityStr += "Rare";
    rarityColor = sf::Color(0, 100, 255); // Blue
    break;
  case Card::CardRarity::EPIC:
    rarityStr += "Epic";
    rarityColor = sf::Color(160, 0, 200); // Purple
    break;
  case Card::CardRarity::LEGENDARY:
    rarityStr += "LEGENDARY";
    rarityColor = sf::Color(255, 165, 0); // Orange/Gold
    break;
  default:
    rarityStr += "Unknown";
    break;
  }

  sf::Text rarityText(font, rarityStr, 24);
  rarityColor.a = static_cast<std::uint8_t>(cardDetailAlpha);
  rarityText.setFillColor(rarityColor);
  rarityText.setStyle(sf::Text::Bold);
  rarityText.setPosition({popupX + 30, popupY + 390});
  window.draw(rarityText);

  // Data structure type
  std::string dataStr = "Data Structure: ";
  switch (selectedCardForDetail->getDataType())
  {
  case Card::CardDataType::STACK:
    dataStr += "Stack (LIFO)";
    break;
  case Card::CardDataType::QUEUE:
    dataStr += "Queue (FIFO)";
    break;
  case Card::CardDataType::TREE:
    dataStr += "Tree (Hierarchical)";
    break;
  case Card::CardDataType::HASH_TABLE:
    dataStr += "Hash Table (Combo)";
    break;
  case Card::CardDataType::DLL:
    dataStr += "Doubly Linked List";
    break;
  case Card::CardDataType::SLL:
    dataStr += "Singly Linked List";
    break;
  case Card::CardDataType::NODE:
    dataStr += "Node";
    break;
  default:
    dataStr += "Unknown";
    break;
  }

  sf::Text dataText(font, dataStr, 20);
  dataText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
  dataText.setPosition({popupX + 30, popupY + 430});
  window.draw(dataText);

  // Description title
  sf::Text descTitle(font, "Description:", 20);
  descTitle.setFillColor(sf::Color(80, 80, 80, static_cast<std::uint8_t>(cardDetailAlpha)));
  descTitle.setStyle(sf::Text::Bold);
  descTitle.setPosition({popupX + 30, popupY + 470});
  window.draw(descTitle);

  // Description text
  sf::Text descText(font, selectedCardForDetail->getDescription(), 18);
  descText.setFillColor(sf::Color(60, 60, 60, static_cast<std::uint8_t>(cardDetailAlpha)));
  descText.setPosition({popupX + 30, popupY + 500});
  window.draw(descText);

  // Close instruction
  sf::Text closeInstruction(font, "Click anywhere to close", 18);
  closeInstruction.setFillColor(sf::Color(120, 120, 120, static_cast<std::uint8_t>(cardDetailAlpha)));
  sf::FloatRect instructionBounds = closeInstruction.getLocalBounds();
  closeInstruction.setOrigin({instructionBounds.size.x / 2.0f, 0});
  closeInstruction.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 40});
  window.draw(closeInstruction);
}

// Draw player stats detail popup
void GameplayScreen::drawStatsDetailPopup(sf::RenderWindow &window, sf::Font &font)
{
  if (!showStatsDetail || !playerRef)
    return;

  sf::Vector2u windowSize = window.getSize();

  // Semi-transparent background overlay
  sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
  overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(statsDetailAlpha * 0.7f)));
  window.draw(overlay);

  // Popup panel (center)
  float popupWidth = 600;
  float popupHeight = 500;
  float popupX = (windowSize.x - popupWidth) / 2.0f;
  float popupY = (windowSize.y - popupHeight) / 2.0f;

  sf::RectangleShape popup({popupWidth, popupHeight});
  popup.setPosition({popupX, popupY});
  popup.setFillColor(sf::Color(240, 240, 230, static_cast<std::uint8_t>(statsDetailAlpha)));
  popup.setOutlineColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(statsDetailAlpha)));
  popup.setOutlineThickness(4.0f);
  window.draw(popup);

  // Title
  sf::Text titleText(font, "Player Stats", 36);
  titleText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(statsDetailAlpha)));
  titleText.setStyle(sf::Text::Bold);
  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
  titleText.setPosition({popupX + popupWidth / 2.0f, popupY + 30});
  window.draw(titleText);

  float contentTop = popupY + 80;
  float contentBottom = popupY + popupHeight - 60;
  float yPos = popupY + 100 - buffScrollOffset;
  float leftMargin = popupX + 40;

  // MEMORY (HP)
  std::string hpStr = "MEMORY: " + std::to_string(playerRef->getHP()) + " / " + std::to_string(playerRef->getMaxHP());
  sf::Text hpText(font, hpStr, 28);
  hpText.setFillColor(sf::Color(200, 0, 0, static_cast<std::uint8_t>(statsDetailAlpha)));
  hpText.setStyle(sf::Text::Bold);
  hpText.setPosition({leftMargin, yPos});
  if (yPos >= contentTop - 30 && yPos <= contentBottom)
    window.draw(hpText);
  yPos += 50;

  // Calculate actual max CPU with scenario modifiers
  int baseCPU = playerRef->getMaxBaseCost();
  int bonusCPU = playerRef->getScenarioModifiers().maxEnergyBonus;
  int actualMaxCPU = baseCPU + bonusCPU;

  std::string energyStr = "CPU: " + std::to_string(actualMaxCPU) + " / " + std::to_string(actualMaxCPU);

  sf::Text energyText(font, energyStr, 28);
  energyText.setFillColor(sf::Color(0, 120, 200, static_cast<std::uint8_t>(statsDetailAlpha)));
  energyText.setStyle(sf::Text::Bold);
  energyText.setPosition({leftMargin, yPos});
  if (yPos >= contentTop - 30 && yPos <= contentBottom)
    window.draw(energyText);
  yPos += 50;

  std::string bitcoinStr = "BITCOIN: " + std::to_string(playerRef->getGold()) + " BTC";
  sf::Text bitcoinText(font, bitcoinStr, 28);
  bitcoinText.setFillColor(sf::Color(255, 165, 0, static_cast<std::uint8_t>(statsDetailAlpha)));
  bitcoinText.setStyle(sf::Text::Bold);
  bitcoinText.setPosition({leftMargin, yPos});
  if (yPos >= contentTop - 30 && yPos <= contentBottom)
    window.draw(bitcoinText);
  yPos += 60;

  // ACTIVE BUFFS
  const Player::RedeemBuffs &buffs = playerRef->getRedeemBuffs();
  int activeBuffCount = 0;
  if (buffs.onePunchMan)
    activeBuffCount++;
  if (buffs.iterator)
    activeBuffCount++;
  if (buffs.singleton)
    activeBuffCount++;
  if (buffs.factory)
    activeBuffCount++;
  if (buffs.adapter)
    activeBuffCount++;
  if (buffs.observer)
    activeBuffCount++;

  if (activeBuffCount > 0)
  {
    sf::Text buffsTitle(font, "Active Buffs:", 24);
    buffsTitle.setFillColor(sf::Color(120, 0, 180, static_cast<std::uint8_t>(statsDetailAlpha)));
    buffsTitle.setStyle(sf::Text::Bold);
    buffsTitle.setPosition({leftMargin, yPos});
    if (yPos >= contentTop - 30 && yPos <= contentBottom)
      window.draw(buffsTitle);

    yPos += 35;

    float buffIndent = leftMargin + 20;
    if (buffs.onePunchMan)
    {
      sf::Text buffText(font, "One Punch Man: MASSIVE damage", 20);
      buffText.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha)));
      buffText.setPosition({buffIndent, yPos});
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(buffText);

      sf::FloatRect bounds = buffText.getLocalBounds();
      sf::RectangleShape underline({bounds.size.x, 2.0f});
      underline.setPosition({buffIndent, yPos + 22});
      underline.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha * 0.5f)));
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(underline);

      yPos += 28;
    }
    if (buffs.iterator)
    {
      sf::Text buffText(font, "Iterator: All cards cost 1 CPU", 20);
      buffText.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha)));
      buffText.setPosition({buffIndent, yPos});
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(buffText);

      sf::FloatRect bounds = buffText.getLocalBounds();
      sf::RectangleShape underline({bounds.size.x, 2.0f});
      underline.setPosition({buffIndent, yPos + 22});
      underline.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha * 0.5f)));
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(underline);

      yPos += 28;
    }
    if (buffs.singleton)
    {
      sf::Text buffText(font, "Singleton: Green slot (no CPU cost)", 20);
      buffText.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha)));
      buffText.setPosition({buffIndent, yPos});
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(buffText);

      sf::FloatRect bounds = buffText.getLocalBounds();
      sf::RectangleShape underline({bounds.size.x, 2.0f});
      underline.setPosition({buffIndent, yPos + 22});
      underline.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha * 0.5f)));
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(underline);

      yPos += 28;
    }
    if (buffs.factory)
    {
      sf::Text buffText(font, "Factory: 3 copies of random card", 20);
      buffText.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha)));
      buffText.setPosition({buffIndent, yPos});
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(buffText);

      sf::FloatRect bounds = buffText.getLocalBounds();
      sf::RectangleShape underline({bounds.size.x, 2.0f});
      underline.setPosition({buffIndent, yPos + 22});
      underline.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha * 0.5f)));
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(underline);

      yPos += 28;
    }
    if (buffs.adapter)
    {
      sf::Text buffText(font, "Adapter: Position-free bonuses", 20);
      buffText.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha)));
      buffText.setPosition({buffIndent, yPos});
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(buffText);

      sf::FloatRect bounds = buffText.getLocalBounds();
      sf::RectangleShape underline({bounds.size.x, 2.0f});
      underline.setPosition({buffIndent, yPos + 22});
      underline.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha * 0.5f)));
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(underline);

      yPos += 28;
    }
    if (buffs.observer)
    {
      sf::Text buffText(font, "Observer: Purple slot (3x activation)", 20);
      buffText.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha)));
      buffText.setPosition({buffIndent, yPos});
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(buffText);

      sf::FloatRect bounds = buffText.getLocalBounds();
      sf::RectangleShape underline({bounds.size.x, 2.0f});
      underline.setPosition({buffIndent, yPos + 22});
      underline.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha * 0.5f)));
      if (yPos >= contentTop - 30 && yPos <= contentBottom)
        window.draw(underline);

      yPos += 28;
    }
    yPos += 20;
  }

  std::string fullScenario = playerRef->getScenarioDescription();
  std::string scenarioName = fullScenario.substr(0, fullScenario.find('\n'));
  std::string scenarioDisplay = "Scenario: " + scenarioName;

  sf::Text scenarioText(font, scenarioDisplay, 24);
  scenarioText.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha)));
  scenarioText.setStyle(sf::Text::Bold);
  scenarioText.setPosition({leftMargin, yPos});
  if (yPos >= contentTop - 30 && yPos <= contentBottom)
    window.draw(scenarioText);

  sf::FloatRect scenarioBounds = scenarioText.getLocalBounds();
  sf::RectangleShape underline({scenarioBounds.size.x, 2.0f});
  underline.setPosition({leftMargin, yPos + 30});
  underline.setFillColor(sf::Color(0, 80, 160, static_cast<std::uint8_t>(statsDetailAlpha * 0.5f)));
  if (yPos >= contentTop - 30 && yPos <= contentBottom)
    window.draw(underline);

  yPos += 50;

  float instructionFontSize = windowSize.y * 0.02f;
  sf::Text instruction(font, "Click scenario to see details", static_cast<unsigned int>(instructionFontSize));
  instruction.setFillColor(sf::Color(120, 120, 120, static_cast<std::uint8_t>(statsDetailAlpha)));
  sf::FloatRect instructionBounds = instruction.getLocalBounds();
  instruction.setOrigin({instructionBounds.size.x / 2.0f, 0});
  instruction.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 40});
  window.draw(instruction);
}

// Draw scenario detail popup
void GameplayScreen::drawScenarioDetailPopup(sf::RenderWindow &window, sf::Font &font)
{
  if (!showScenarioDetail || !playerRef)
    return;

  sf::Vector2u windowSize = window.getSize();

  // Semi-transparent background overlay (darker than stats popup)
  sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
  overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(scenarioDetailAlpha * 0.8f)));
  window.draw(overlay);

  // Popup panel (center)
  float popupWidth = 600;
  float popupHeight = 550;
  float popupX = (windowSize.x - popupWidth) / 2.0f;
  float popupY = (windowSize.y - popupHeight) / 2.0f;

  sf::RectangleShape popup({popupWidth, popupHeight});
  popup.setPosition({popupX, popupY});
  popup.setFillColor(sf::Color(250, 245, 230, static_cast<std::uint8_t>(scenarioDetailAlpha)));
  popup.setOutlineColor(sf::Color(100, 80, 50, static_cast<std::uint8_t>(scenarioDetailAlpha)));
  popup.setOutlineThickness(5.0f);
  window.draw(popup);

  float yPos = popupY + 40;
  float leftMargin = popupX + 40;

  std::string fullScenario = playerRef->getScenarioDescription();
  std::string scenarioName = fullScenario.substr(0, fullScenario.find('\n'));
  std::string modifiersLine = "";
  size_t newlinePos = fullScenario.find('\n');
  if (newlinePos != std::string::npos)
  {
    modifiersLine = fullScenario.substr(newlinePos + 1);
  }

  sf::Text titleText(font, scenarioName, 32);
  titleText.setFillColor(sf::Color(100, 50, 0, static_cast<std::uint8_t>(scenarioDetailAlpha)));
  titleText.setStyle(sf::Text::Bold);
  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setOrigin({titleBounds.size.x / 2.0f, 0});
  titleText.setPosition({popupX + popupWidth / 2.0f, yPos});
  window.draw(titleText);
  yPos += 60;

  // Flavor description based on role and scenario
  std::string roleName = playerRef->getRoleName();
  std::string flavorText = "";

  if (scenarioName.find("potato laptop") != std::string::npos)
  {
    flavorText = "Your ancient laptop wheezes with every\ncompile. The fans sound like jet engines,\nbut hey, at least card costs are cheaper!";
  }
  else if (scenarioName.find("4K monitor") != std::string::npos)
  {
    flavorText = "With crystal-clear 4K resolution, you can\nsee every pixel of your bugs! You deal\nmore damage but cards cost more energy.";
  }
  else if (scenarioName.find("CSS at 3 AM") != std::string::npos)
  {
    flavorText = "Coffee-fueled midnight debugging session.\nYour brain is fried but you've unlocked\n+1 max energy from pure caffeine power!";
  }
  else if (scenarioName.find("legacy code") != std::string::npos)
  {
    flavorText = "Maintaining code from 2005... at least the\nbugs are documented. More damage and gold\nrewards for surviving this nightmare.";
  }
  else if (scenarioName.find("beast workstation") != std::string::npos)
  {
    flavorText = "64GB RAM, RTX 4090, water-cooled CPU.\nYou're unstoppable! Massive damage boost\nbut your power bills... I mean card costs.";
  }
  else if (scenarioName.find("database queries") != std::string::npos)
  {
    flavorText = "SELECT * FROM enemies WHERE defeated=0;\nOptimizing queries makes cards cheaper,\nbut you take more damage from SQL errors.";
  }
  else if (scenarioName.find("frontend and backend") != std::string::npos)
  {
    flavorText = "Jack of all trades, master of getting paid.\nGold rewards up, damage up. You're doing\nthe work of three devs, might as well!";
  }
  else if (scenarioName.find("unlimited coffee") != std::string::npos)
  {
    flavorText = "The office coffee machine is your best\nfriend. Max energy +1 from infinite\ncaffeine! You're vibrating with power.";
  }
  else if (scenarioName.find("tight deadline") != std::string::npos)
  {
    flavorText = "Production deploy in 2 hours! Panic mode\nactivated. Double gold rewards but stress\nmakes you take way more damage.";
  }
  else if (scenarioName.find("deployment pipelines") != std::string::npos)
  {
    flavorText = "CI/CD master! Your pipelines are perfectly\noptimized. Huge damage output but all\nthat responsibility takes its toll.";
  }
  else if (scenarioName.find("production issues") != std::string::npos)
  {
    flavorText = "The site is down! Everything is on fire!\nYou take massive damage from stress but\nthe overtime pay (gold) is worth it.";
  }
  else if (scenarioName.find("remotely") != std::string::npos)
  {
    flavorText = "Working from your cozy home office.\nYou dodge damage (-50%) by not being\nin the chaotic office, but cards cost more.";
  }
  else if (scenarioName.find("massive datasets") != std::string::npos)
  {
    flavorText = "Crunching terabytes of data like breakfast.\nGold rewards up but your damage output\nsuffers from analysis paralysis.";
  }
  else if (scenarioName.find("ML models overnight") != std::string::npos)
  {
    flavorText = "Training neural networks while you sleep.\nGood gold rewards but your cards cost more\nenergy from all that GPU usage.";
  }
  else if (scenarioName.find("stakeholders") != std::string::npos)
  {
    flavorText = "Explaining p-values to non-technical execs.\nGold rewards up but your damage is nerfed\nfrom dumbing down your brilliant insights.";
  }
  else
  {
    flavorText = "A unique scenario with interesting\nmodifiers. Good luck!";
  }

  // Draw flavor text
  sf::Text flavorTextObj(font, flavorText, 20);
  flavorTextObj.setFillColor(sf::Color(60, 40, 20, static_cast<std::uint8_t>(scenarioDetailAlpha)));
  flavorTextObj.setPosition({leftMargin, yPos});
  window.draw(flavorTextObj);
  yPos += 100;

  // Modifiers title
  sf::Text modsTitle(font, "Modifiers:", 24);
  modsTitle.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(scenarioDetailAlpha)));
  modsTitle.setStyle(sf::Text::Bold);
  modsTitle.setPosition({leftMargin, yPos});
  window.draw(modsTitle);
  yPos += 40;

  // Draw modifiers from the scenario description
  const Player::ScenarioModifiers &mods = playerRef->getScenarioModifiers();

  if (mods.cardCostModifier != 0.0f)
  {
    std::string modStr = "Card Costs: ";
    if (mods.cardCostModifier < 0)
      modStr += std::to_string(static_cast<int>(-mods.cardCostModifier * 100)) + "% cheaper";
    else if (mods.cardCostModifier < 1.0f)
      modStr += "+" + std::to_string(static_cast<int>(mods.cardCostModifier * 100)) + "%";
    else
      modStr += "x" + std::to_string(static_cast<int>(mods.cardCostModifier + 1));

    sf::Text modText(font, modStr, 28);
    modText.setFillColor(sf::Color(40, 40, 40, static_cast<std::uint8_t>(scenarioDetailAlpha)));
    modText.setStyle(sf::Text::Bold);
    modText.setPosition({leftMargin, yPos});
    window.draw(modText);
    yPos += 40;
  }

  if (mods.damageModifier != 0.0f)
  {
    std::string modStr = "Damage: ";
    if (mods.damageModifier < 0)
      modStr += std::to_string(static_cast<int>(mods.damageModifier * 100)) + "%";
    else
      modStr += "+" + std::to_string(static_cast<int>(mods.damageModifier * 100)) + "%";

    sf::Text modText(font, modStr, 28);
    modText.setFillColor(sf::Color(40, 40, 40, static_cast<std::uint8_t>(scenarioDetailAlpha)));
    modText.setStyle(sf::Text::Bold);
    modText.setPosition({leftMargin, yPos});
    window.draw(modText);
    yPos += 40;
  }

  if (mods.maxEnergyBonus > 0)
  {
    std::string modStr = "Max Energy: +" + std::to_string(mods.maxEnergyBonus);

    sf::Text modText(font, modStr, 28);
    modText.setFillColor(sf::Color(40, 40, 40, static_cast<std::uint8_t>(scenarioDetailAlpha)));
    modText.setStyle(sf::Text::Bold);
    modText.setPosition({leftMargin, yPos});
    window.draw(modText);
    yPos += 40;
  }

  if (mods.rewardModifier != 0.0f)
  {
    std::string modStr = "Gold Rewards: ";
    if (mods.rewardModifier < 1.0f)
      modStr += "+" + std::to_string(static_cast<int>(mods.rewardModifier * 100)) + "%";
    else
      modStr += "x" + std::to_string(static_cast<int>(mods.rewardModifier + 1));

    sf::Text modText(font, modStr, 28);
    modText.setFillColor(sf::Color(40, 40, 40, static_cast<std::uint8_t>(scenarioDetailAlpha)));
    modText.setStyle(sf::Text::Bold);
    modText.setPosition({leftMargin, yPos});
    window.draw(modText);
    yPos += 40;
  }

  if (mods.speedModifier != 0.0f)
  {
    std::string modStr = "Damage Taken: ";
    if (mods.speedModifier < 0)
      modStr += std::to_string(static_cast<int>(mods.speedModifier * 100)) + "%";
    else
      modStr += "+" + std::to_string(static_cast<int>(mods.speedModifier * 100)) + "%";

    sf::Text modText(font, modStr, 28);
    modText.setFillColor(sf::Color(40, 40, 40, static_cast<std::uint8_t>(scenarioDetailAlpha)));
    modText.setStyle(sf::Text::Bold);
    modText.setPosition({leftMargin, yPos});
    window.draw(modText);
    yPos += 40;
  }

  // Close instruction
  sf::Text closeInstruction(font, "Click anywhere to close", 18);
  closeInstruction.setFillColor(sf::Color(120, 100, 80, static_cast<std::uint8_t>(scenarioDetailAlpha)));
  sf::FloatRect instructionBounds = closeInstruction.getLocalBounds();
  closeInstruction.setOrigin({instructionBounds.size.x / 2.0f, 0});
  closeInstruction.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 40});
  window.draw(closeInstruction);
}

// Draw redeem code overlay
void GameplayScreen::drawRedeemOverlay(sf::RenderWindow &window, sf::Font &font)
{
  sf::Vector2u windowSize = window.getSize();

  // Fade in overlay
  if (redeemOverlayAlpha < 255.0f)
  {
    redeemOverlayAlpha = std::min(255.0f, redeemOverlayAlpha + 800.0f * 0.016f);
  }

  // Black overlay background
  sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
  overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(redeemOverlayAlpha * 0.8f)));
  window.draw(overlay);

  // Popup box
  float popupWidth = 600.0f;
  float popupHeight = 400.0f;
  float popupX = (windowSize.x - popupWidth) / 2.0f;
  float popupY = (windowSize.y - popupHeight) / 2.0f;

  sf::RectangleShape popup({popupWidth, popupHeight});
  popup.setPosition({popupX, popupY});
  popup.setFillColor(sf::Color(240, 235, 220, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  popup.setOutlineThickness(5.0f);
  popup.setOutlineColor(sf::Color(100, 80, 60, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  window.draw(popup);

  // Title
  sf::Text title(font, "REDEEM CODE", 48);
  title.setFillColor(sf::Color(60, 40, 20, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  title.setStyle(sf::Text::Bold);
  sf::FloatRect titleBounds = title.getLocalBounds();
  title.setOrigin({titleBounds.size.x / 2.0f, 0});
  title.setPosition({popupX + popupWidth / 2.0f, popupY + 30});
  window.draw(title);

  // Input box
  float inputBoxWidth = 500.0f;
  float inputBoxHeight = 60.0f;
  float inputBoxX = popupX + (popupWidth - inputBoxWidth) / 2.0f;
  float inputBoxY = popupY + 120;

  sf::RectangleShape inputBox({inputBoxWidth, inputBoxHeight});
  inputBox.setPosition({inputBoxX, inputBoxY});
  inputBox.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  inputBox.setOutlineThickness(3.0f);
  inputBox.setOutlineColor(redeemInputActive ? sf::Color(100, 200, 255, static_cast<std::uint8_t>(redeemOverlayAlpha)) : sf::Color(100, 100, 100, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  window.draw(inputBox);

  // Input text
  sf::Text inputText(font, redeemCodeInput.empty() ? "Enter code here..." : redeemCodeInput, 28);
  inputText.setFillColor(redeemCodeInput.empty() ? sf::Color(150, 150, 150, static_cast<std::uint8_t>(redeemOverlayAlpha)) : sf::Color(40, 40, 40, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  inputText.setPosition({inputBoxX + 15, inputBoxY + 12});
  window.draw(inputText);

  // Submit button
  float buttonWidth = 200.0f;
  float buttonHeight = 50.0f;
  float buttonX = popupX + (popupWidth - buttonWidth) / 2.0f;
  float buttonY = popupY + 220;

  sf::RectangleShape submitButton({buttonWidth, buttonHeight});
  submitButton.setPosition({buttonX, buttonY});
  submitButton.setFillColor(sf::Color(100, 200, 100, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  submitButton.setOutlineThickness(3.0f);
  submitButton.setOutlineColor(sf::Color(60, 120, 60, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  window.draw(submitButton);

  sf::Text submitText(font, "REDEEM", 32);
  submitText.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  submitText.setStyle(sf::Text::Bold);
  sf::FloatRect submitBounds = submitText.getLocalBounds();
  submitText.setOrigin({submitBounds.size.x / 2.0f, 0});
  submitText.setPosition({buttonX + buttonWidth / 2.0f, buttonY + 8});
  window.draw(submitText);

  // Close instruction
  sf::Text closeText(font, "ESC to close", 20);
  closeText.setFillColor(sf::Color(100, 100, 100, static_cast<std::uint8_t>(redeemOverlayAlpha)));
  sf::FloatRect closeBounds = closeText.getLocalBounds();
  closeText.setOrigin({closeBounds.size.x / 2.0f, 0});
  closeText.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 50});
  window.draw(closeText);
}

// Process redeem code input
void GameplayScreen::processRedeemCode(const std::string &code)
{
  if (!playerRef)
    return;

  // Convert code to lowercase for comparison
  std::string lowerCode = code;
  std::transform(lowerCode.begin(), lowerCode.end(), lowerCode.begin(),
                 [](unsigned char c)
                 { return static_cast<char>(std::tolower(c)); });

  Player::RedeemBuffs &buffs = playerRef->getRedeemBuffs();
  bool success = false;
  bool alreadyRedeemed = false;

  if (lowerCode == "one punch man" || lowerCode == "onepunchman")
  {
    if (!buffs.onePunchMan)
    {
      buffs.onePunchMan = true;
      success = true;
      std::cout << "[Redeem] ONE PUNCH MAN activated! Damage +100000000000%" << std::endl;
    }
    else
    {
      alreadyRedeemed = true;
    }
  }
  else if (lowerCode == "iterator")
  {
    if (!buffs.iterator)
    {
      buffs.iterator = true;
      success = true;
      std::cout << "[Redeem] ITERATOR activated! All cards cost 1 less CPU" << std::endl;
    }
    else
    {
      alreadyRedeemed = true;
    }
  }
  else if (lowerCode == "singleton")
  {
    if (!buffs.singleton)
    {
      buffs.singleton = true;
      success = true;
      std::cout << "[Redeem] SINGLETON activated! Extra active bar slot (always active)" << std::endl;
    }
    else
    {
      alreadyRedeemed = true;
    }
  }
  else if (lowerCode == "factory")
  {
    if (!buffs.factory)
    {
      buffs.factory = true;
      success = true;
      std::cout << "[Redeem] FACTORY activated! Draw 3 more cards per turn" << std::endl;
    }
    else
    {
      alreadyRedeemed = true;
    }
  }
  else if (lowerCode == "adapter")
  {
    if (!buffs.adapter)
    {
      buffs.adapter = true;
      success = true;
      std::cout << "[Redeem] ADAPTER activated! Ignore all card requirements" << std::endl;
    }
    else
    {
      alreadyRedeemed = true;
    }
  }
  else if (lowerCode == "observer" || lowerCode == "design pattern" || lowerCode == "designpattern")
  {
    if (!buffs.observer)
    {
      buffs.observer = true;
      success = true;
      std::cout << "[Redeem] OBSERVER activated! Extra slot that activates card 3x" << std::endl;
    }
    else
    {
      alreadyRedeemed = true;
    }
  }

  // Show result popup
  if (success)
  {
    redeemResultMessage = "CODE REDEEMED!\n" + code;
    redeemSuccess = true;
    showRedeemResult = true;
    redeemResultAlpha = 0.0f;
    std::cout << "[Redeem] SUCCESS: " << code << std::endl;
  }
  else if (alreadyRedeemed)
  {
    redeemResultMessage = "ALREADY REDEEMED\n" + code;
    redeemSuccess = false;
    showRedeemResult = true;
    redeemResultAlpha = 0.0f;
    std::cout << "[Redeem] Code already redeemed: " << code << std::endl;
  }
  else
  {
    redeemResultMessage = "INVALID CODE\n" + code;
    redeemSuccess = false;
    showRedeemResult = true;
    redeemResultAlpha = 0.0f;
    std::cout << "[Redeem] Invalid code: " << code << std::endl;
  }

  // Close overlay
  redeemCodeInput = "";
  showRedeemOverlay = false;
  redeemOverlayAlpha = 0.0f;
  redeemInputActive = false;
}

void GameplayScreen::loadBuffTextures()
{
  buffTextures.clear();

  std::map<std::string, std::string> buffIconPaths = {
      {"One Punch Man", "asset/buff/onepunch.png"},
      {"Iterator", "asset/buff/iterator.png"},
      {"Singleton", "asset/buff/singleton.png"},
      {"Factory", "asset/buff/factory.png"},
      {"Adapter", "asset/buff/adapter.png"},
      {"Observer", "asset/buff/observer.png"}};

  for (const auto &pair : buffIconPaths)
  {
    sf::Texture texture;
    if (texture.loadFromFile(pair.second))
    {
      buffTextures[pair.first] = std::move(texture);
      std::cout << "[GameplayScreen] Loaded buff texture: " << pair.second << std::endl;
    }
    else
    {
      std::cout << "[GameplayScreen] WARNING: Failed to load buff texture: " << pair.second << std::endl;
    }
  }
}

void GameplayScreen::drawBuffDetailPopup(sf::RenderWindow &window, sf::Font &font)
{
  if (!showBuffDetail || selectedBuffName.empty() || !playerRef)
    return;

  sf::Vector2u windowSize = window.getSize();

  sf::RectangleShape overlay({static_cast<float>(windowSize.x), static_cast<float>(windowSize.y)});
  overlay.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(buffDetailAlpha * 0.7f)));
  window.draw(overlay);

  float popupWidth = 500;
  float popupHeight = 550;
  float popupX = (windowSize.x - popupWidth) / 2.0f;
  float popupY = (windowSize.y - popupHeight) / 2.0f;

  sf::RectangleShape popup({popupWidth, popupHeight});
  popup.setPosition({popupX, popupY});
  popup.setFillColor(sf::Color(240, 240, 230, static_cast<std::uint8_t>(buffDetailAlpha)));
  popup.setOutlineColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(buffDetailAlpha)));
  popup.setOutlineThickness(6.0f);
  window.draw(popup);

  sf::Text nameText(font, selectedBuffName, 40);
  nameText.setFillColor(sf::Color(80, 40, 100, static_cast<std::uint8_t>(buffDetailAlpha)));
  nameText.setStyle(sf::Text::Bold);
  sf::FloatRect nameBounds = nameText.getLocalBounds();
  nameText.setOrigin({nameBounds.size.x / 2.0f, 0});
  nameText.setPosition({popupX + popupWidth / 2.0f, popupY + 40});
  window.draw(nameText);

  float iconSize = 200.0f;
  float iconX = popupX + (popupWidth - iconSize) / 2.0f;
  float iconY = popupY + 120;

  auto it = buffTextures.find(selectedBuffName);
  if (it != buffTextures.end())
  {
    sf::Sprite iconSprite(it->second);

    sf::Vector2u textureSize = it->second.getSize();
    float scaleX = iconSize / textureSize.x;
    float scaleY = iconSize / textureSize.y;
    float scale = std::min(scaleX, scaleY);
    iconSprite.setScale({scale, scale});

    sf::FloatRect spriteBounds = iconSprite.getLocalBounds();
    float scaledWidth = spriteBounds.size.x * scale;
    float scaledHeight = spriteBounds.size.y * scale;
    float centerX = iconX + (iconSize - scaledWidth) / 2.0f;
    float centerY = iconY + (iconSize - scaledHeight) / 2.0f;

    iconSprite.setPosition({centerX, centerY});

    sf::Color spriteColor = iconSprite.getColor();
    spriteColor.a = static_cast<std::uint8_t>(buffDetailAlpha);
    iconSprite.setColor(spriteColor);

    window.draw(iconSprite);
  }
  else
  {
    sf::RectangleShape iconBox({iconSize, iconSize});
    iconBox.setPosition({iconX, iconY});
    iconBox.setFillColor(sf::Color(200, 200, 200, static_cast<std::uint8_t>(buffDetailAlpha * 0.5f)));
    iconBox.setOutlineColor(sf::Color(100, 100, 100, static_cast<std::uint8_t>(buffDetailAlpha)));
    iconBox.setOutlineThickness(4.0f);
    window.draw(iconBox);

    sf::Text errorText(font, "?", 80);
    errorText.setFillColor(sf::Color(100, 100, 100, static_cast<std::uint8_t>(buffDetailAlpha)));
    sf::FloatRect errorBounds = errorText.getLocalBounds();
    errorText.setOrigin({errorBounds.size.x / 2.0f, errorBounds.size.y / 2.0f});
    errorText.setPosition({iconX + iconSize / 2.0f, iconY + iconSize / 2.0f});
    window.draw(errorText);
  }

  float yPos = popupY + 340;

  sf::Text descTitle(font, "Effect:", 24);
  descTitle.setFillColor(sf::Color(80, 80, 80, static_cast<std::uint8_t>(buffDetailAlpha)));
  descTitle.setStyle(sf::Text::Bold);
  descTitle.setPosition({popupX + 50, yPos});
  window.draw(descTitle);
  yPos += 40;

  std::string description = "";
  if (selectedBuffName == "One Punch Man")
    description = "Deals MASSIVE damage with attacks.\nYour attacks become unstoppable!";
  else if (selectedBuffName == "Iterator")
    description = "All cards cost exactly 1 CPU.\nEfficiently traverse your deck!";
  else if (selectedBuffName == "Singleton")
    description = "Unlocks a green special slot that\nplays cards for FREE (0 CPU cost)!";
  else if (selectedBuffName == "Factory")
    description = "At the start of each turn,\ncreate 3 copies of a random card!";
  else if (selectedBuffName == "Adapter")
    description = "All pattern bonuses work\nregardless of card position!";
  else if (selectedBuffName == "Observer")
    description = "Unlocks a purple special slot that\nactivates cards 3 times!";

  sf::Text descText(font, description, 20);
  descText.setFillColor(sf::Color(60, 60, 60, static_cast<std::uint8_t>(buffDetailAlpha)));
  descText.setPosition({popupX + 50, yPos});
  window.draw(descText);

  sf::Text closeInstruction(font, "Click anywhere to close", 18);
  closeInstruction.setFillColor(sf::Color(120, 120, 120, static_cast<std::uint8_t>(buffDetailAlpha)));
  sf::FloatRect instructionBounds = closeInstruction.getLocalBounds();
  closeInstruction.setOrigin({instructionBounds.size.x / 2.0f, 0});
  closeInstruction.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 40});
  window.draw(closeInstruction);
}
