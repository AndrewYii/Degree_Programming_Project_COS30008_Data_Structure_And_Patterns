#include "RoleSelectionScreen.h"
#include "Game.h"
#include "Logger.h"
#include <iostream>
#include <cmath>

// Constructor [Implementation]
RoleSelectionScreen::RoleSelectionScreen(Game *gameInstance): Screen(gameInstance), head(nullptr), currentMiddle(nullptr),totalRoles(0), selectedRoleIndex(-1), hoveredElement(-1), mousePressed(false),frameTime(0.40f), playerName(""), inputActive(false),cursorBlinkTimer(0.0f), cursorVisible(true)
{
  std::cout << "[RoleSelectionScreen] Created" << std::endl;
  // Initialise animation state
  for (int i = 0; i < 3; i++)
  {
    currentFrame[i] = 0;
    animationTimer[i] = 0.0f;
  }
  buildCircularList();
  loadSpriteSheets();
}

// Destructor [Implementation]
RoleSelectionScreen::~RoleSelectionScreen()
{
  std::cout << "[RoleSelectionScreen] Destroyed" << std::endl;
  destroyCircularList();
}

// Build circular doubly linked list of roles [Implementation]
void RoleSelectionScreen::buildCircularList()
{
  std::cout << "[RoleSelection] Building circular doubly linked list..." << std::endl;
  ProgrammerRole roles[5] = {
      {
          "FRONTEND DEV",
          "Master of CSS and",
          "pixel-perfect designs",
          "\"It works on my browser!\"",
          "Trees +3 | Nodes +2",
          80,
          100,
          "asset/character/Frontenddeveloper.png"},
      {
          "BACKEND DEV",
          "Architect of databases",
          "and RESTful APIs",
          "\"It's not a bug, feature!\"",
          "Hash +3 | DLL +2",
          100,
          80,
          "asset/character/BackendDeveloper.png"},
      {
          "FULL STACK DEV",
          "Jack of all trades,",
          "master of... some?",
          "\"I fix both ends!\"",
          "All Cards +1",
          90,
          90,
          "asset/character/Fullstackdeveloper.png"},
      {
          "DEVOPS ENGINEER",
          "Pipeline wizard and",
          "deployment ninja",
          "\"Works everywhere!\"",
          "Queue +3 | Stack +2",
          85,
          95,
          "asset/character/DevOsDeveloper.png"},
      {
          "DATA SCIENTIST",
          "Turning coffee into",
          "ML models",
          "\"Correlation = causation!\"",
          "Trees +3 | Skills +3",
          75,
          110,
          "asset/character/Datascientist.png"}};

  RoleNode *nodes[5];
  for (int i = 0; i < 5; i++)
  {
    nodes[i] = new RoleNode(roles[i]);
    totalRoles++;
  }

  // Link nodes in circular doubly linked list
  for (int i = 0; i < 5; i++)
  {
    nodes[i]->next = nodes[(i + 1) % 5]; // Circular: last -> first
    nodes[i]->prev = nodes[(i + 4) % 5]; // Circular: first -> last
  }

  // Set head and current middle
  head = nodes[0];
  currentMiddle = nodes[0]; // Start with first role in middle

  std::cout << "[RoleSelection] Circular list built with " << totalRoles << " nodes" << std::endl;
}

// Destroy circular doubly linked list [Implementation]
void RoleSelectionScreen::destroyCircularList()
{
  if (!head)
    return;

  std::cout << "[RoleSelection] Destroying circular list..." << std::endl;

  // Break circular link
  RoleNode *last = head->prev;
  last->next = nullptr;

  // Delete all nodes
  RoleNode *current = head;
  while (current)
  {
    RoleNode *next = current->next;
    delete current;
    current = next;
  }

  head = nullptr;
  currentMiddle = nullptr;
  totalRoles = 0;
}

// Rotate left (show previous roles) [Implementation]
void RoleSelectionScreen::rotateLeft()
{
  if (!currentMiddle)
    return;
  currentMiddle = currentMiddle->prev; // Move to previous node
  std::cout << "[RoleSelection] Rotated LEFT - now showing: " << currentMiddle->data.name << std::endl;
}

// Rotate right (show next roles) [Implementation]
void RoleSelectionScreen::rotateRight()
{
  if (!currentMiddle)
    return;
  currentMiddle = currentMiddle->next; // Move to next node
  std::cout << "[RoleSelection] Rotated RIGHT - now showing: " << currentMiddle->data.name << std::endl;
}

// Navigate to this screen [Implementation]
void RoleSelectionScreen::onEnter()
{
  std::cout << "[RoleSelectionScreen] Entered" << std::endl;
  selectedRoleIndex = -1;
  hoveredElement = -1;
  mousePressed = false;
  playerName = ""; // Clear name on enter
  inputActive = false;

  // Reset to first role
  if (head)
  {
    currentMiddle = head;
  }
}

// Exit from this screen [Implementation]
void RoleSelectionScreen::onExit()
{
  std::cout << "[RoleSelectionScreen] Exited" << std::endl;
}

// Handle events [Implementation]
void RoleSelectionScreen::handleEvents()
{
  sf::RenderWindow *window = game->getWindow();

  while (const std::optional event = window->pollEvent())
  {
    if (event->is<sf::Event::Closed>())
    {
      std::cout << "[RoleSelection] Close event" << std::endl;
      game->quit();
    }

    // Text input for name
    if (const auto *textEntered = event->getIf<sf::Event::TextEntered>())
    {
      if (inputActive)
      {
        char entered = static_cast<char>(textEntered->unicode);

        // Backspace
        if (entered == 8 && !playerName.empty())
        {
          playerName.pop_back();
        }
        // Valid character (letters, numbers, space, hyphen, underscore)
        else if ((entered >= 'a' && entered <= 'z') ||
                 (entered >= 'A' && entered <= 'Z') ||
                 (entered >= '0' && entered <= '9') ||
                 entered == ' ' || entered == '-' || entered == '_')
        {
          if (playerName.length() < 20)
          { // Max 20 characters
            playerName += entered;
          }
        }
      }
    }
    // Mouse clicks
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

// Update logic [Implementation]
void RoleSelectionScreen::update(float deltaTime)
{
  sf::RenderWindow *window = game->getWindow();
  sf::Vector2u windowSize = window->getSize();
  sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
  sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);

  // Update cursor blink
  cursorBlinkTimer += deltaTime;
  if (cursorBlinkTimer >= 0.5f)
  {
    cursorBlinkTimer -= 0.5f;
    cursorVisible = !cursorVisible;
  }

  // Reset hover
  hoveredElement = -1;

  // BACK button (top left)
  float backSize = windowSize.y * 0.06f;
  float backMargin = windowSize.x * 0.02f;
  sf::FloatRect backBounds({backMargin, backMargin}, {backSize * 2.5f, backSize});

  // SETTINGS button (top right)
  float settingsSize = windowSize.y * 0.06f;
  float settingsMargin = windowSize.x * 0.02f;
  float settingsX = windowSize.x - settingsMargin - settingsSize;
  float settingsY = settingsMargin;
  sf::FloatRect settingsBounds({settingsX, settingsY}, {settingsSize, settingsSize});

  // Arrow buttons
  float arrowSize = windowSize.y * 0.08f;
  float arrowY = windowSize.y * 0.50f;
  float leftArrowX = windowSize.x * 0.05f;
  float rightArrowX = windowSize.x * 0.95f - arrowSize;
  sf::FloatRect leftArrowBounds({leftArrowX, arrowY - arrowSize / 2}, {arrowSize, arrowSize});
  sf::FloatRect rightArrowBounds({rightArrowX, arrowY - arrowSize / 2}, {arrowSize, arrowSize});

  // Card layout (3 cards)
  float cardWidth = windowSize.x * 0.22f;
  float cardHeight = windowSize.y * 0.60f;
  float cardGap = windowSize.x * 0.03f;
  float totalWidth = (cardWidth * 3) + (cardGap * 2);
  float startX = (windowSize.x - totalWidth) / 2.0f;
  float cardY = windowSize.y * 0.22f;

  sf::FloatRect leftCardBounds({startX, cardY}, {cardWidth, cardHeight});
  sf::FloatRect midCardBounds({startX + cardWidth + cardGap, cardY}, {cardWidth, cardHeight});
  sf::FloatRect rightCardBounds({startX + (cardWidth + cardGap) * 2, cardY}, {cardWidth, cardHeight});

  // NAME INPUT and START JOURNEY button
  float inputWidth = windowSize.x * 0.40f;
  float inputHeight = windowSize.y * 0.06f;
  float buttonWidth = windowSize.x * 0.25f;
  float buttonGap = windowSize.x * 0.02f;
  float bottomY = windowSize.y * 0.88f;
  float inputX = (windowSize.x - inputWidth - buttonGap - buttonWidth) / 2.0f;
  float buttonX = inputX + inputWidth + buttonGap;

  sf::FloatRect inputBounds({inputX, bottomY}, {inputWidth, inputHeight});
  sf::FloatRect startButtonBounds({buttonX, bottomY}, {buttonWidth, inputHeight});

  // Check hover and clicks
  if (backBounds.contains(mousePos))
  {
    hoveredElement = 5;
    if (mousePressed)
    {
      std::cout << "[RoleSelection] BACK clicked" << std::endl;
      game->changeScreen(Game::ScreenType::MAIN_MENU);
      mousePressed = false;
    }
  }
  else if (settingsBounds.contains(mousePos))
  {
    hoveredElement = 6;
    if (mousePressed)
    {
      std::cout << "[RoleSelection] SETTINGS clicked" << std::endl;
      game->changeScreen(Game::ScreenType::SETTINGS);
      mousePressed = false;
    }
  }
  else if (leftArrowBounds.contains(mousePos))
  {
    hoveredElement = 0;
    if (mousePressed)
    {
      rotateLeft();
      mousePressed = false;
    }
  }
  else if (rightArrowBounds.contains(mousePos))
  {
    hoveredElement = 4;
    if (mousePressed)
    {
      rotateRight();
      mousePressed = false;
    }
  }
  else if (leftCardBounds.contains(mousePos))
  {
    hoveredElement = 1;
    if (mousePressed)
    {
      std::cout << "[RoleSelection] Left card clicked (navigate left)" << std::endl;
      rotateLeft();
      mousePressed = false;
    }
  }
  else if (midCardBounds.contains(mousePos))
  {
    hoveredElement = 2;
  }
  else if (rightCardBounds.contains(mousePos))
  {
    hoveredElement = 3;
    if (mousePressed)
    {
      std::cout << "[RoleSelection] Right card clicked (navigate right)" << std::endl;
      rotateRight();
      mousePressed = false;
    }
  }
  else if (inputBounds.contains(mousePos))
  {
    hoveredElement = 7;
    if (mousePressed)
    {
      inputActive = true;
      mousePressed = false;
    }
  }
  else if (startButtonBounds.contains(mousePos) && !playerName.empty())
  {
    hoveredElement = 8;
    if (mousePressed)
    {
      std::cout << "[RoleSelection] START JOURNEY - Name: " << playerName
                << ", Role: " << currentMiddle->data.name << std::endl;

      Player *newPlayer = nullptr;
      std::string role = currentMiddle->data.name;

      if (role == "FRONTEND DEV")
      {
        newPlayer = new FrontendDev(playerName);
      }
      else if (role == "BACKEND DEV")
      {
        newPlayer = new BackendDev(playerName);
      }
      else if (role == "FULL STACK DEV")
      {
        newPlayer = new FullStackDev(playerName);
      }
      else if (role == "DEVOPS ENGINEER")
      {
        newPlayer = new DevOpsEngineer(playerName);
      }
      else if (role == "DATA SCIENTIST")
      {
        newPlayer = new DataScientist(playerName);
      }

      if (newPlayer)
      {
        game->setPlayer(newPlayer);



        StoryIntroScreen *storyScreen = static_cast<StoryIntroScreen *>(game->getStoryIntroScreen());
        if (storyScreen)
        {
          storyScreen->setPlayerData(playerName, role);
        }

        game->changeScreen(Game::ScreenType::STORY_INTRO);
      }

      mousePressed = false;
    }
  }
  else
  {
    if (mousePressed)
    {
      inputActive = false;
      mousePressed = false;
    }
  }

  for (int i = 0; i < 3; i++)
  {
    bool shouldAnimate = false;

    if (i == 1)
    {
      shouldAnimate = true;
    }

    // Hovered cards animate
    if (hoveredElement == i + 1)
    {
      shouldAnimate = true;
    }

    if (shouldAnimate)
    {
      animationTimer[i] += deltaTime;
      if (animationTimer[i] >= frameTime)
      {
        animationTimer[i] -= frameTime;
        currentFrame[i] = (currentFrame[i] + 1) % 4;
      }
    }
    else
    {
      currentFrame[i] = 0;
      animationTimer[i] = 0.0f;
    }
  }
}

// Render screen [Implementation]
void RoleSelectionScreen::render(sf::RenderWindow &window, sf::Font &font)
{
  window.clear(sf::Color::White);

  const sf::Color COLOR_BLACK = Game::COLOR_BLACK;
  const sf::Color COLOR_OFF_WHITE = Game::COLOR_OFF_WHITE;

  sf::Vector2u windowSize = window.getSize();

  float backSize = windowSize.y * 0.06f;
  float backMargin = windowSize.x * 0.02f;

  sf::RectangleShape backButton({backSize * 2.5f, backSize});
  backButton.setPosition({backMargin, backMargin});
  backButton.setFillColor(hoveredElement == 5 ? COLOR_BLACK : sf::Color::White);
  backButton.setOutlineColor(COLOR_BLACK);
  backButton.setOutlineThickness(3.0f);
  window.draw(backButton);

  float backTextSize = windowSize.y * 0.035f;
  sf::Text backText(font, "< BACK", static_cast<unsigned int>(backTextSize));
  backText.setFillColor(hoveredElement == 5 ? sf::Color::White : COLOR_BLACK);
  backText.setStyle(sf::Text::Bold);
  backText.setPosition({backMargin + backSize * 0.2f, backMargin + backSize * 0.15f});
  window.draw(backText);

  // SETTINGS BUTTON (top right)
  float settingsSize = windowSize.y * 0.06f;
  float settingsMargin = windowSize.x * 0.02f;
  float settingsX = windowSize.x - settingsMargin - settingsSize;
  float settingsY = settingsMargin;

  sf::RectangleShape settingsButton({settingsSize, settingsSize});
  settingsButton.setPosition({settingsX, settingsY});
  settingsButton.setFillColor(hoveredElement == 6 ? COLOR_BLACK : sf::Color::White);
  settingsButton.setOutlineColor(COLOR_BLACK);
  settingsButton.setOutlineThickness(3.0f);
  window.draw(settingsButton);

  sf::Color gearColor = (hoveredElement == 6) ? sf::Color::White : COLOR_BLACK;
  drawGearIcon(window, settingsX + settingsSize / 2, settingsY + settingsSize / 2, settingsSize * 0.35f, gearColor);

  float titleSize = windowSize.y * 0.065f;
  sf::Text titleText(font, "SELECT YOUR ROLE", static_cast<unsigned int>(titleSize));
  titleText.setFillColor(COLOR_BLACK);
  titleText.setStyle(sf::Text::Bold);

  sf::FloatRect titleBounds = titleText.getLocalBounds();
  titleText.setOrigin({titleBounds.size.x / 2.0f, titleBounds.size.y / 2.0f});
  titleText.setPosition({windowSize.x / 2.0f, windowSize.y * 0.10f});
  window.draw(titleText);

  if (!currentMiddle)
    return;

  float cardWidth = windowSize.x * 0.22f;
  float cardHeight = windowSize.y * 0.60f;
  float cardGap = windowSize.x * 0.03f;
  float totalWidth = (cardWidth * 3) + (cardGap * 2);
  float startX = (windowSize.x - totalWidth) / 2.0f;
  float cardY = windowSize.y * 0.22f;

  // Get the 3 roles to display
  RoleNode *leftNode = currentMiddle->prev;
  RoleNode *midNode = currentMiddle;
  RoleNode *rightNode = currentMiddle->next;

  // Get role indices
  int leftIndex = getRoleIndex(leftNode);
  int midIndex = getRoleIndex(midNode);
  int rightIndex = getRoleIndex(rightNode);

  // Draw left card (previous role) - cardPosition = 0
  drawRoleCard(window, font, leftNode->data, leftIndex,startX, cardY, cardWidth, cardHeight,false, hoveredElement == 1, 0);

  // Draw middle card (current/selected role) - cardPosition = 1
  drawRoleCard(window, font, midNode->data, midIndex,startX + cardWidth + cardGap, cardY, cardWidth, cardHeight,true, hoveredElement == 2, 1);

  // Draw right card (next role) - cardPosition = 2
  drawRoleCard(window, font, rightNode->data, rightIndex,startX + (cardWidth + cardGap) * 2, cardY, cardWidth, cardHeight,false, hoveredElement == 3, 2);
  float arrowSize = windowSize.y * 0.08f;
  float arrowY = windowSize.y * 0.50f;

  // LEFT ARROW
  float leftArrowX = windowSize.x * 0.05f;
  sf::Color leftArrowColor = (hoveredElement == 0) ? COLOR_BLACK : COLOR_OFF_WHITE;
  drawArrow(window, leftArrowX + arrowSize / 2, arrowY, arrowSize * 0.6f, true, leftArrowColor);

  // RIGHT ARROW
  float rightArrowX = windowSize.x * 0.95f - arrowSize;
  sf::Color rightArrowColor = (hoveredElement == 4) ? COLOR_BLACK : COLOR_OFF_WHITE;
  drawArrow(window, rightArrowX + arrowSize / 2, arrowY, arrowSize * 0.6f, false, rightArrowColor);

  float inputWidth = windowSize.x * 0.40f;
  float inputHeight = windowSize.y * 0.06f;
  float buttonWidth = windowSize.x * 0.25f;
  float buttonGap = windowSize.x * 0.02f;
  float bottomY = windowSize.y * 0.88f;
  float inputX = (windowSize.x - inputWidth - buttonGap - buttonWidth) / 2.0f;
  float buttonX = inputX + inputWidth + buttonGap;
  sf::RectangleShape inputBox({inputWidth, inputHeight});
  inputBox.setPosition({inputX, bottomY});
  inputBox.setFillColor(inputActive ? COLOR_OFF_WHITE : sf::Color::White);
  inputBox.setOutlineColor(inputActive ? COLOR_BLACK : sf::Color(150, 150, 150));
  inputBox.setOutlineThickness(inputActive ? 3.0f : 2.0f);
  window.draw(inputBox);
  // Input label
  float labelSize = windowSize.y * 0.020f;
  sf::Text inputLabel(font, "ENTER YOUR NAME:", static_cast<unsigned int>(labelSize));
  inputLabel.setFillColor(COLOR_BLACK);
  inputLabel.setStyle(sf::Text::Bold);
  inputLabel.setPosition({inputX, bottomY - windowSize.y * 0.04f});
  window.draw(inputLabel);

  // Input text or placeholder
  float inputTextSize = windowSize.y * 0.028f;
  std::string displayText = playerName.empty() ? "Type your name..." : playerName;
  sf::Text inputText(font, displayText, static_cast<unsigned int>(inputTextSize));
  inputText.setFillColor(playerName.empty() ? sf::Color(150, 150, 150) : COLOR_BLACK);
  inputText.setPosition({inputX + windowSize.x * 0.01f, bottomY + inputHeight * 0.20f});
  window.draw(inputText);

  // Blinking cursor
  if (inputActive && cursorVisible && !playerName.empty())
  {
    sf::FloatRect textBounds = inputText.getLocalBounds();
    float cursorX = inputX + windowSize.x * 0.01f + textBounds.size.x + 5.0f;
    sf::RectangleShape cursor({2.0f, inputHeight * 0.6f});
    cursor.setPosition({cursorX, bottomY + inputHeight * 0.15f});
    cursor.setFillColor(COLOR_BLACK);
    window.draw(cursor);
  }

  bool canStart = !playerName.empty();
  sf::RectangleShape startButton({buttonWidth, inputHeight});
  startButton.setPosition({buttonX, bottomY});

  if (!canStart)
  {
    // Disabled state
    startButton.setFillColor(sf::Color(180, 180, 180));
    startButton.setOutlineColor(sf::Color(120, 120, 120));
  }
  else if (hoveredElement == 8)
  {
    // Hover state
    startButton.setFillColor(COLOR_BLACK);
    startButton.setOutlineColor(COLOR_BLACK);
  }
  else
  {
    // Normal enabled state
    startButton.setFillColor(sf::Color::White);
    startButton.setOutlineColor(COLOR_BLACK);
  }
  startButton.setOutlineThickness(3.0f);
  window.draw(startButton);

  float buttonTextSize = windowSize.y * 0.032f;
  sf::Text buttonText(font, "START JOURNEY", static_cast<unsigned int>(buttonTextSize));

  if (!canStart)
  {
    buttonText.setFillColor(sf::Color(100, 100, 100));
  }
  else if (hoveredElement == 8)
  {
    buttonText.setFillColor(sf::Color::White);
  }
  else
  {
    buttonText.setFillColor(COLOR_BLACK);
  }
  buttonText.setStyle(sf::Text::Bold);

  sf::FloatRect buttonTextBounds = buttonText.getLocalBounds();
  buttonText.setOrigin({buttonTextBounds.size.x / 2.0f, buttonTextBounds.size.y / 2.0f});
  buttonText.setPosition({buttonX + buttonWidth / 2.0f, bottomY + inputHeight / 2.0f});
  window.draw(buttonText);

  float hintSize = windowSize.y * 0.018f;
  sf::Text hintText(font, "Arrows or side cards to browse | Enter name and click Start Journey to begin", static_cast<unsigned int>(hintSize));
  hintText.setFillColor(COLOR_BLACK);

  sf::FloatRect hintBounds = hintText.getLocalBounds();
  hintText.setOrigin({hintBounds.size.x / 2.0f, hintBounds.size.y / 2.0f});
  hintText.setPosition({windowSize.x / 2.0f, windowSize.y * 0.97f});
  window.draw(hintText);
}

// Draw individual role card [Implementation]
void RoleSelectionScreen::drawRoleCard(sf::RenderWindow &window, sf::Font &font, const ProgrammerRole &role, int roleIndex,float cardX, float cardY, float cardWidth, float cardHeight,bool isMiddle, bool isHovered, int cardPosition)
{

  const sf::Color COLOR_BLACK = Game::COLOR_BLACK;
  const sf::Color COLOR_OFF_WHITE = Game::COLOR_OFF_WHITE;
  const sf::Color COLOR_LIGHT_GRAY(200, 200, 200); 
  sf::Vector2u windowSize = window.getSize();

  sf::RectangleShape card({cardWidth, cardHeight});
  card.setPosition({cardX, cardY});

  if (isHovered)
  {
    card.setFillColor(COLOR_LIGHT_GRAY); 
  }
  else if (isMiddle)
  {
    card.setFillColor(COLOR_OFF_WHITE);
  }
  else
  {
    card.setFillColor(sf::Color::White);
  }

  card.setOutlineColor(COLOR_BLACK);
  card.setOutlineThickness(isMiddle ? 4.0f : 2.0f);
  window.draw(card);

  sf::Color textColor = COLOR_BLACK;

  float spriteSize = cardWidth * 0.4f;
  float spriteX = cardX + (cardWidth - spriteSize) / 2.0f; 
  float spriteY = cardY + cardHeight * 0.05f;

  if (roleIndex >= 0 && roleIndex < 5 && spriteSheets[roleIndex].loaded)
  {
    int frame = currentFrame[cardPosition];
    SpriteFrame &frameData = spriteSheets[roleIndex].frames[frame];

    sf::Sprite sprite(spriteSheets[roleIndex].texture);
    sprite.setTextureRect({{frameData.x, frameData.y}, {frameData.width, frameData.height}});
    float scaleX = spriteSize / frameData.width;
    float scaleY = spriteSize / frameData.height;
    sprite.setScale({scaleX, scaleY}); 
    sprite.setPosition({spriteX, spriteY});

    window.draw(sprite);
  }
  else
  {
    sf::RectangleShape spritePlaceholder({spriteSize, spriteSize});
    spritePlaceholder.setPosition({spriteX, spriteY});
    spritePlaceholder.setFillColor(COLOR_OFF_WHITE);
    spritePlaceholder.setOutlineColor(textColor);
    spritePlaceholder.setOutlineThickness(2.0f);
    window.draw(spritePlaceholder);

    float spriteTextSize = windowSize.y * 0.035f;
    sf::Text spriteText(font, "[?]", static_cast<unsigned int>(spriteTextSize));
    spriteText.setFillColor(textColor);
    sf::FloatRect spriteBounds = spriteText.getLocalBounds();
    spriteText.setOrigin({spriteBounds.size.x / 2.0f, spriteBounds.size.y / 2.0f});
    spriteText.setPosition({spriteX + spriteSize / 2.0f, spriteY + spriteSize / 2.0f});
    window.draw(spriteText);
  }

  float nameSize = windowSize.y * 0.028f; 
  sf::Text nameText(font, role.name, static_cast<unsigned int>(nameSize));
  nameText.setFillColor(textColor);
  nameText.setStyle(sf::Text::Bold);

  sf::FloatRect nameBounds = nameText.getLocalBounds();
  nameText.setOrigin({nameBounds.size.x / 2.0f, 0});
  nameText.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight * 0.30f}); 
  window.draw(nameText);

  float descSize = windowSize.y * 0.019f; 
  sf::Text desc1Text(font, role.descriptionLine1, static_cast<unsigned int>(descSize));
  desc1Text.setFillColor(textColor);
  desc1Text.setStyle(sf::Text::Italic);

  sf::FloatRect desc1Bounds = desc1Text.getLocalBounds();
  desc1Text.setOrigin({desc1Bounds.size.x / 2.0f, 0});
  desc1Text.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight * 0.39f}); 
  window.draw(desc1Text);

  // Description line 2
  sf::Text desc2Text(font, role.descriptionLine2, static_cast<unsigned int>(descSize));
  desc2Text.setFillColor(textColor);
  desc2Text.setStyle(sf::Text::Italic);

  sf::FloatRect desc2Bounds = desc2Text.getLocalBounds();
  desc2Text.setOrigin({desc2Bounds.size.x / 2.0f, 0});
  desc2Text.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight * 0.44f}); 
  window.draw(desc2Text);

  float statsSize = windowSize.y * 0.021f;

  sf::Text healthText(font, "HP: " + std::to_string(role.startingHealth), static_cast<unsigned int>(statsSize));
  healthText.setFillColor(textColor);
  healthText.setStyle(sf::Text::Bold);
  healthText.setPosition({cardX + cardWidth * 0.1f, cardY + cardHeight * 0.54f}); // Moved down
  window.draw(healthText);

  sf::Text energyText(font, "EN: " + std::to_string(role.startingEnergy), static_cast<unsigned int>(statsSize));
  energyText.setFillColor(textColor);
  energyText.setStyle(sf::Text::Bold);
  energyText.setPosition({cardX + cardWidth * 0.1f, cardY + cardHeight * 0.60f}); // Moved down
  window.draw(energyText);

  float bonusSize = windowSize.y * 0.018f;
  sf::Text bonusLabel(font, "BONUS:", static_cast<unsigned int>(bonusSize));
  bonusLabel.setFillColor(textColor);
  bonusLabel.setStyle(sf::Text::Bold);
  bonusLabel.setPosition({cardX + cardWidth * 0.1f, cardY + cardHeight * 0.68f}); // Moved down
  window.draw(bonusLabel);

  sf::Text bonusText(font, role.specialBonus, static_cast<unsigned int>(bonusSize));
  bonusText.setFillColor(textColor);
  bonusText.setPosition({cardX + cardWidth * 0.1f, cardY + cardHeight * 0.73f}); // Moved down
  window.draw(bonusText);

  float quoteSize = windowSize.y * 0.016f;
  sf::Text quoteText(font, role.humorQuote, static_cast<unsigned int>(quoteSize));
  quoteText.setFillColor(textColor);
  quoteText.setStyle(sf::Text::Italic);

  sf::FloatRect quoteBounds = quoteText.getLocalBounds();
  quoteText.setOrigin({quoteBounds.size.x / 2.0f, 0});
  quoteText.setPosition({cardX + cardWidth / 2.0f, cardY + cardHeight * 0.88f}); // Moved down
  window.draw(quoteText);
}

// Draw gear icon for settings button [Implementation]
void RoleSelectionScreen::drawGearIcon(sf::RenderWindow &window, float x, float y, float radius, sf::Color color)
{
  const int teeth = 12;
  const float innerRadius = radius * 0.5f;
  const float outerRadius = radius;

  // Outer circle
  sf::CircleShape outerCircle(outerRadius, teeth * 4);
  outerCircle.setOrigin({outerRadius, outerRadius});
  outerCircle.setPosition({x, y});
  outerCircle.setFillColor(color);
  outerCircle.setOutlineColor(color);
  outerCircle.setOutlineThickness(1.0f);
  window.draw(outerCircle);

  // Teeth
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

  // Inner circle
  sf::CircleShape innerCircle(innerRadius);
  innerCircle.setOrigin({innerRadius, innerRadius});
  innerCircle.setPosition({x, y});
  sf::Color holeColor = (color == sf::Color::White) ? Game::COLOR_BLACK : sf::Color::White;
  innerCircle.setFillColor(holeColor);
  window.draw(innerCircle);

  // Center circle
  sf::CircleShape centerCircle(innerRadius * 0.4f);
  centerCircle.setOrigin({innerRadius * 0.4f, innerRadius * 0.4f});
  centerCircle.setPosition({x, y});
  centerCircle.setFillColor(color);
  window.draw(centerCircle);
}

// Draw arrow (left or right) [Implementation]
void RoleSelectionScreen::drawArrow(sf::RenderWindow &window, float x, float y, float size, bool pointsLeft, sf::Color color)
{
  sf::ConvexShape arrow;
  arrow.setPointCount(3);

  if (pointsLeft)
  {
    // Left-pointing arrow: <
    arrow.setPoint(0, {x + size, y - size}); 
    arrow.setPoint(1, {x, y});           
    arrow.setPoint(2, {x + size, y + size}); 
  }
  else
  {
    // Right-pointing arrow: >
    arrow.setPoint(0, {x - size, y - size}); 
    arrow.setPoint(1, {x, y});              
    arrow.setPoint(2, {x - size, y + size}); 
  }

  arrow.setFillColor(color);
  arrow.setOutlineColor(Game::COLOR_BLACK);
  arrow.setOutlineThickness(2.0f);
  window.draw(arrow);
}

// Load sprite sheets for all roles [Implementation]
void RoleSelectionScreen::loadSpriteSheets()
{
  std::cout << "[RoleSelection] Loading sprite sheets..." << std::endl;
  SpriteFrame frontendFrames[4] = {
      {0, 0, 122, 185},
      {132, 0, 118, 185},
      {258, 0, 110, 185},
      {370, 0, 114, 185}};
  // Backend Developer frames
  SpriteFrame backendFrames[4] = {
      {0, 0, 94, 186},
      {148, 0, 85, 186},
      {255, 0, 104, 186},
      {396, 0, 80, 186}};

  // Full Stack Developer frames
  SpriteFrame fullstackFrames[4] = {
      {0, 0, 115, 175},
      {130, 0, 115, 175},
      {260, 0, 115, 175},
      {380, 0, 115, 175}};

  // DevOps Engineer frames
  SpriteFrame devopsFrames[4] = {
      {0, 0, 120, 192},
      {140, 0, 90, 192},
      {258, 0, 106, 192},
      {368, 0, 108, 192}};

  // Data Scientist frames
  SpriteFrame datascientistFrames[4] = {
      {30, 0, 104, 164},
      {132, 0, 107, 164},
      {240, 0, 119, 164},
      {374, 0, 115, 164}};
  // Array of frame data pointers (in order: Frontend, Backend, Fullstack, DevOps, DataScientist)
  SpriteFrame *allFrames[5] = {
      frontendFrames,
      backendFrames,
      fullstackFrames,
      devopsFrames,
      datascientistFrames};

  // Load each role's sprite sheet
  RoleNode *current = head;
  for (int i = 0; i < totalRoles; i++)
  {
    std::cout << "[RoleSelection] Loading: " << current->data.spriteSheetPath << std::endl;

    if (spriteSheets[i].texture.loadFromFile(current->data.spriteSheetPath))
    {
      // Copy the correct frame data for this role
      for (int f = 0; f < 4; f++)
      {
        spriteSheets[i].frames[f] = allFrames[i][f];
      }
      spriteSheets[i].loaded = true;
      std::cout << "[RoleSelection] ? Loaded: " << current->data.name << std::endl;
    }
    else
    {
      std::cout << "[RoleSelection] ? FAILED to load: " << current->data.spriteSheetPath << std::endl;
      spriteSheets[i].loaded = false;
    }

    current = current->next;
  }
}

// Get index of a role node [Implementation]
int RoleSelectionScreen::getRoleIndex(RoleNode *node)
{
  if (!head || !node)
    return -1;

  RoleNode *current = head;
  for (int i = 0; i < totalRoles; i++)
  {
    if (current == node)
      return i;
    current = current->next;
  }
  return -1;
}

// Get the entered player name [Implementation]
std::string RoleSelectionScreen::getPlayerName() const { 
  return playerName; 
}
