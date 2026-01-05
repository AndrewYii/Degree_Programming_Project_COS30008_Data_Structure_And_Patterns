#include "StoryIntroScreen.h"
#include "Game.h"
#include <iostream>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <algorithm>

StoryIntroScreen::StoryIntroScreen(Game *gameInstance) : Screen(gameInstance), head(nullptr), tail(nullptr), displayedCharCount(0), typewriterTimer(0.0f), charDelay(0.03f), currentCharIndex(0), storyComplete(false), inputActive(false), shakeAnimation(false), shakeTimer(0.0f), shakeDuration(0.5f), shakeIntensity(10.0f), cursorBlinkTimer(0.0f), cursorVisible(true), lastCharPlayed(-1), mousePressed(false)
{

    std::cout << "[StoryIntroScreen] Created" << std::endl;

    if (typingSoundBuffer.loadFromFile("audio/typing.wav"))
    {
        std::cout << "[StoryIntroScreen] ? Loaded typing.wav" << std::endl;
    }
    else if (typingSoundBuffer.loadFromFile("audio/typing.mp3"))
    {
        std::cout << "[StoryIntroScreen] ? Loaded typing.mp3 (consider using .wav for better performance)" << std::endl;
    }
    else
    {
        std::cout << "[StoryIntroScreen] ? WARNING: Failed to load typing sound (tried .wav and .mp3)" << std::endl;
    }

    if (typingSoundBuffer.getDuration().asSeconds() > 0)
    {
        typingSound = std::make_unique<sf::Sound>(typingSoundBuffer);
        typingSound->setVolume(game->getSoundVolume());
        float duration = typingSoundBuffer.getDuration().asSeconds();
        std::cout << "[StoryIntroScreen] Typing sound duration: " << duration << "s";
        if (duration > 0.2f)
        {
            std::cout << " ?? TOO LONG! Should be < 0.1s for typewriter effect!";
        }
        std::cout << std::endl;
    }

    if (errorSoundBuffer.loadFromFile("audio/error.wav"))
    {
        std::cout << "[StoryIntroScreen] ? Loaded error.wav" << std::endl;
    }
    else if (errorSoundBuffer.loadFromFile("audio/error.mp3"))
    {
        std::cout << "[StoryIntroScreen] ? Loaded error.mp3" << std::endl;
    }
    else
    {
        std::cout << "[StoryIntroScreen] ? WARNING: Failed to load error sound" << std::endl;
    }

    if (errorSoundBuffer.getDuration().asSeconds() > 0)
    {
        errorSound = std::make_unique<sf::Sound>(errorSoundBuffer);
        errorSound->setVolume(game->getSoundVolume());
    }

    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

StoryIntroScreen::~StoryIntroScreen()
{
    std::cout << "[StoryIntroScreen] Destroyed" << std::endl;
    clearList();
}

void StoryIntroScreen::appendCharacter(char c)
{
    CharNode *newNode = new CharNode(c);

    if (!head)
    {
        // First character
        head = newNode;
        tail = newNode;
    }
    else
    {
        // Append to end
        tail->next = newNode;
        tail = newNode;
    }

    displayedCharCount++;
}

std::string StoryIntroScreen::getDisplayedText()
{
    std::string result;
    result.reserve(displayedCharCount);

    CharNode *current = head;
    while (current)
    {
        result += current->character;
        current = current->next;
    }

    return result;
}

void StoryIntroScreen::clearList()
{
    CharNode *current = head;
    while (current)
    {
        CharNode *next = current->next;
        delete current;
        current = next;
    }

    head = nullptr;
    tail = nullptr;
    displayedCharCount = 0;
}

std::string StoryIntroScreen::generateStory(const std::string &pName, const std::string &roleName)
{
    std::string scenario = getRandomScenario(roleName);

    std::string story = "Hello, " + pName + ".\n\n";
    story += "You are a " + roleName + " " + scenario + "\n\n";
    story += "Your journey begins now.\n\n";
    story += "But first, prove you can code...\n";

    return story;
}

std::string StoryIntroScreen::getRandomScenario(const std::string &roleName)
{
    std::vector<std::string> scenarios;

    if (roleName == "FRONTEND DEV")
    {
        scenarios = {
            "working on a potato laptop.\nCard costs -20% | Damage taken +10%",
            "with a 4K monitor setup.\nDamage +30% | Card costs +40%",
            "debugging CSS at 3 AM.\nMax Energy +1 | Damage taken +25%"};
    }
    else if (roleName == "BACKEND DEV")
    {
        scenarios = {
            "maintaining legacy code.\nDamage +30% | Gold rewards +50%",
            "with a beast workstation.\nDamage +40% | Card costs x2",
            "optimizing database queries.\nCard costs -30% | Damage taken +20%"};
    }
    else if (roleName == "FULL STACK DEV")
    {
        scenarios = {
            "juggling frontend and backend.\nGold rewards +50% | Damage +20%",
            "with unlimited coffee supply.\nMax Energy +1 | Damage taken +10%",
            "on a tight deadline.\nGold rewards x2 | Damage taken +25%"};
    }
    else if (roleName == "DEVOPS ENGINEER")
    {
        scenarios = {
            "managing deployment pipelines.\nDamage +50% | Damage taken +30%",
            "fixing production issues.\nDamage taken +40% | Gold rewards +50%",
            "working remotely.\nDamage taken -50% | Card costs +20%"};
    }
    else if (roleName == "DATA SCIENTIST")
    {
        scenarios = {
            "analyzing massive datasets.\nGold rewards +60% | Damage -30%",
            "training ML models overnight.\nGold rewards +50% | Card costs +40%",
            "presenting to stakeholders.\nGold rewards +40% | Damage -20%"};
    }
    else
    {
        scenarios = {
            "starting your coding journey.\nAll stats balanced, no modifiers."};
    }

    int index = std::rand() % scenarios.size();

    // NEW: Apply scenario modifiers to player
    Player &player = game->getPlayer();
    player.setScenarioDescription(scenarios[index]);
    applyScenarioModifiers(roleName, index);

    return scenarios[index];
}

void StoryIntroScreen::applyScenarioModifiers(const std::string &roleName, int scenarioIndex)
{
    Player &player = game->getPlayer();

    float cardCost = 0.0f;
    float speed = 0.0f;
    int maxEnergyBonus = 0; // +0 or +1 max energy
    float damage = 0.0f;
    float reward = 0.0f;

    if (roleName == "FRONTEND DEV")
    {
        if (scenarioIndex == 0)
        {                      // Potato laptop
            cardCost = -0.20f; // -20% card costs
            speed = -0.10f;    // -10% speed (lag)
        }
        else if (scenarioIndex == 1)
        {                     // 4K monitor
            speed = 0.30f;    // +30% speed
            cardCost = 0.40f; // Card costs +40%
        }
        else if (scenarioIndex == 2)
        {                       // CSS at 3 AM
            maxEnergyBonus = 1; // +1 max energy (coffee power!)
            damage = 0.25f;
        }
    }
    else if (roleName == "BACKEND DEV")
    {
        if (scenarioIndex == 0)
        {                   // Legacy code
            damage = 0.30f; // +30% bug damage
            reward = 0.50f; // +50% experience
        }
        else if (scenarioIndex == 1)
        {                     // Beast workstation
            speed = 0.40f;    // +40% speed
            cardCost = 1.00f; // Card costs doubled
        }
        else if (scenarioIndex == 2)
        {                      // Database optimization
            cardCost = -0.30f; // -30% card costs
            damage = 0.20f;    // +20% stress damage
        }
    }
    else if (roleName == "FULL STACK DEV")
    {
        if (scenarioIndex == 0)
        {                   // Juggling both
            reward = 0.50f; // +50% versatility
            speed = -0.20f; // -20% focus
        }
        else if (scenarioIndex == 1)
        {                       // Unlimited coffee
            maxEnergyBonus = 1; // +1 max energy (coffee!)
            damage = 0.10f;     // +10% jitter damage
        }
        else if (scenarioIndex == 2)
        {                   // Tight deadline
            reward = 1.00f; // Rewards doubled
            damage = 0.25f; // +25% health drain
        }
    }
    else if (roleName == "DEVOPS ENGINEER")
    {
        if (scenarioIndex == 0)
        {                   // Deployment pipelines
            speed = 0.50f;  // +50% automation
            damage = 0.30f; // +30% stress
        }
        else if (scenarioIndex == 1)
        {                   // Production issues
            damage = 0.40f; // +40% pressure
            reward = 0.50f; // Hero bonus
        }
        else if (scenarioIndex == 2)
        {                     // Remote work
            damage = -0.50f;  // -50% distraction damage
            cardCost = 0.20f; // +20% card costs (isolation)
        }
    }
    else if (roleName == "DATA SCIENTIST")
    {
        if (scenarioIndex == 0)
        {                   // Massive datasets
            reward = 0.60f; // +60% insight
            speed = -0.30f; // -30% processing time
        }
        else if (scenarioIndex == 1)
        {                     // ML models overnight
            reward = 0.50f;   // +50% power
            cardCost = 0.40f; // +40% costs (sleep deprivation)
        }
        else if (scenarioIndex == 2)
        {                   // Stakeholder presentation
            reward = 0.40f; // +40% communication
            damage = 0.20f; // Imposter syndrome
        }
    }

    // Apply modifiers to player
    player.setScenarioModifiers(cardCost, speed, maxEnergyBonus, damage, reward);

    // Initialize starter deck with random cards
    player.initializeStarterDeck();

    std::cout << "[StoryIntro] Applied scenario modifiers for " << roleName
              << " scenario " << scenarioIndex << std::endl;
}

void StoryIntroScreen::setPlayerData(const std::string &name, const std::string &role)
{
    playerName = name;
    selectedRole = role;
    std::cout << "[StoryIntro] Player data set: " << name << " as " << role << std::endl;
}

void StoryIntroScreen::onEnter()
{
    std::cout << "[StoryIntroScreen] Entered" << std::endl;

    // Reset state
    clearList();
    userInput = "";
    inputActive = false;
    storyComplete = false;
    shakeAnimation = false;
    currentCharIndex = 0;
    typewriterTimer = 0.0f;

    // Generate story
    fullStoryText = generateStory(playerName, selectedRole);
    std::cout << "[StoryIntro] Story generated: " << fullStoryText.length() << " characters" << std::endl;

    // DEBUG: Check if typing sound is ready
    if (typingSound)
    {
        std::cout << "[StoryIntro] Typing sound is READY. Duration: "
                  << typingSoundBuffer.getDuration().asSeconds() << "s" << std::endl;
        if (typingSoundBuffer.getDuration().asSeconds() > 0.5f)
        {
            std::cout << "[StoryIntro] WARNING: Typing sound is too long! Should be < 0.1s for typewriter effect" << std::endl;
        }
    }
    else
    {
        std::cout << "[StoryIntro] ERROR: Typing sound is NULL!" << std::endl;
    }
}

void StoryIntroScreen::onExit()
{
    std::cout << "[StoryIntroScreen] Exited" << std::endl;
    if (typingSound)
    {
        typingSound->stop();
    }
    if (errorSound)
    {
        errorSound->stop();
    }
}

void StoryIntroScreen::handleEvents()
{
    sf::RenderWindow *window = game->getWindow();

    while (const std::optional event = window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            std::cout << "[StoryIntro] Close event" << std::endl;
            game->quit();
        }

        // Text input for challenge
        if (const auto *textEntered = event->getIf<sf::Event::TextEntered>())
        {
            if (storyComplete && inputActive)
            {
                char entered = static_cast<char>(textEntered->unicode);

                // Backspace
                if (entered == 8 && !userInput.empty())
                {
                    userInput.pop_back();
                }
                else if ((entered >= 32 && entered <= 126))
                { // Printable ASCII
                    if (userInput.length() < 50)
                    {
                        userInput += entered;
                    }
                }
            }
        }

        // Enter key to submit
        if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            if (keyPressed->code == sf::Keyboard::Key::Enter && storyComplete)
            {
                // Case-insensitive comparison
                std::string userInputLower = userInput;
                std::string correctAnswerLower = correctAnswer;

                // Convert both to lowercase for comparison
                std::transform(userInputLower.begin(), userInputLower.end(), userInputLower.begin(),
                               [](unsigned char c)
                               { return static_cast<char>(std::tolower(c)); });
                std::transform(correctAnswerLower.begin(), correctAnswerLower.end(), correctAnswerLower.begin(),
                               [](unsigned char c)
                               { return static_cast<char>(std::tolower(c)); });

                if (userInputLower == correctAnswerLower)
                {
                    std::cout << "[StoryIntro] ? Correct answer! Going to gameplay..." << std::endl;

                    std::cout << "\n=== PLAYER SUMMARY ===" << std::endl;
                    std::cout << game->getPlayer().getSummary() << std::endl;
                    std::cout << "=====================\n"
                              << std::endl;

                    game->changeScreen(Game::ScreenType::GAMEPLAY);
                }
                else
                {
                    std::cout << "[StoryIntro] ? Wrong answer: \"" << userInput << "\"" << std::endl;
                    shakeAnimation = true;
                    shakeTimer = 0.0f;
                    userInput = "";

                    // Play error sound
                    if (errorSound)
                    {
                        errorSound->setVolume(game->getSoundVolume());
                        errorSound->play();
                        std::cout << "[StoryIntro] Playing error sound" << std::endl;
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

void StoryIntroScreen::update(float deltaTime)
{
    sf::RenderWindow *window = game->getWindow();
    sf::Vector2u windowSize = window->getSize();
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
    sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);

    if (!storyComplete)
    {
        typewriterTimer += deltaTime;

        if (typewriterTimer >= charDelay)
        {
            if (currentCharIndex < static_cast<int>(fullStoryText.length()))
            {
                char nextChar = fullStoryText[currentCharIndex];

                appendCharacter(nextChar);

                if (typingSound && nextChar != ' ' && nextChar != '\n' && nextChar != '\t')
                {
                    if (typingSound->getStatus() == sf::SoundSource::Status::Playing)
                    {
                        typingSound->stop();
                    }

                    typingSound->setVolume(game->getSoundVolume());
                    typingSound->play();
                    std::cout << "[TYPING] '" << nextChar << "' vol:" << game->getSoundVolume() << "% status:"
                              << (typingSound->getStatus() == sf::SoundSource::Status::Playing ? "PLAYING" : "NOT_PLAYING") << std::endl;
                }

                currentCharIndex++;
                typewriterTimer = 0.0f;
            }
            else
            {
                storyComplete = true;
                inputActive = true;
                std::cout << "[StoryIntro] Story complete. Challenge active." << std::endl;
            }
        }
    }

    // Cursor blink
    cursorBlinkTimer += deltaTime;
    if (cursorBlinkTimer >= 0.5f)
    {
        cursorBlinkTimer -= 0.5f;
        cursorVisible = !cursorVisible;
    }

    // Shake animation
    if (shakeAnimation)
    {
        shakeTimer += deltaTime;
        if (shakeTimer >= shakeDuration)
        {
            shakeAnimation = false;
            shakeTimer = 0.0f;
        }
    }

    // Check input box click
    if (storyComplete && mousePressed)
    {
        float inputY = windowSize.y * 0.75f;
        float inputWidth = windowSize.x * 0.70f;
        float inputHeight = windowSize.y * 0.06f;
        float inputX = (windowSize.x - inputWidth) / 2.0f;

        sf::FloatRect inputBounds({inputX, inputY}, {inputWidth, inputHeight});
        if (inputBounds.contains(mousePos))
        {
            inputActive = true;
            mousePressed = false;
        }
    }
}

void StoryIntroScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    const sf::Color COLOR_BLACK = Game::COLOR_BLACK;
    const sf::Color COLOR_OFF_WHITE = Game::COLOR_OFF_WHITE;

    // Black background
    window.clear(sf::Color::Black);

    sf::Vector2u windowSize = window.getSize();

    std::string displayText = getDisplayedText();

    float storyTextSize = windowSize.y * 0.028f;
    sf::Text storyText(font, displayText, static_cast<unsigned int>(storyTextSize));
    storyText.setFillColor(COLOR_OFF_WHITE);
    storyText.setPosition({windowSize.x * 0.10f, windowSize.y * 0.20f});
    window.draw(storyText);

    if (storyComplete)
    {
        // Challenge prompt
        float promptSize = windowSize.y * 0.025f;
        sf::Text promptText(font, "TO PROCEED, PROVE YOU CAN CODE:", static_cast<unsigned int>(promptSize));
        promptText.setFillColor(COLOR_OFF_WHITE);
        promptText.setStyle(sf::Text::Bold);

        sf::FloatRect promptBounds = promptText.getLocalBounds();
        promptText.setOrigin({promptBounds.size.x / 2.0f, 0});
        promptText.setPosition({windowSize.x / 2.0f, windowSize.y * 0.65f});
        window.draw(promptText);

        // Input box
        float inputWidth = windowSize.x * 0.70f;
        float inputHeight = windowSize.y * 0.06f;
        float inputX = (windowSize.x - inputWidth) / 2.0f;
        float inputY = windowSize.y * 0.75f;

        // Apply shake animation
        float shakeOffsetX = 0.0f;
        if (shakeAnimation)
        {
            shakeOffsetX = std::sin(shakeTimer * 30.0f) * shakeIntensity;
        }

        sf::RectangleShape inputBox({inputWidth, inputHeight});
        inputBox.setPosition({inputX + shakeOffsetX, inputY});
        inputBox.setFillColor(inputActive ? sf::Color(30, 30, 30) : sf::Color(20, 20, 20));
        inputBox.setOutlineColor(shakeAnimation ? sf::Color::Red : COLOR_OFF_WHITE);
        inputBox.setOutlineThickness(inputActive ? 3.0f : 2.0f);
        window.draw(inputBox);

        // Input text or placeholder
        float inputTextSize = windowSize.y * 0.025f;
        std::string displayInput = userInput.empty() ? "Type: cout << \"HELLO WORLD\";" : userInput;
        sf::Text inputText(font, displayInput, static_cast<unsigned int>(inputTextSize));
        inputText.setFillColor(userInput.empty() ? sf::Color(100, 100, 100) : COLOR_OFF_WHITE);
        inputText.setPosition({inputX + shakeOffsetX + windowSize.x * 0.02f, inputY + inputHeight * 0.25f});
        window.draw(inputText);

        // Blinking cursor
        if (inputActive && cursorVisible && !userInput.empty())
        {
            sf::FloatRect textBounds = inputText.getLocalBounds();
            float cursorX = inputX + shakeOffsetX + windowSize.x * 0.02f + textBounds.size.x + 5.0f;
            sf::RectangleShape cursor({2.0f, inputHeight * 0.5f});
            cursor.setPosition({cursorX, inputY + inputHeight * 0.25f});
            cursor.setFillColor(COLOR_OFF_WHITE);
            window.draw(cursor);
        }

        // Hint
        float hintSize = windowSize.y * 0.018f;
        sf::Text hintText(font, "Look Carefully: Space, Press ENTER to submit", static_cast<unsigned int>(hintSize));
        hintText.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect hintBounds = hintText.getLocalBounds();
        hintText.setOrigin({hintBounds.size.x / 2.0f, 0});
        hintText.setPosition({windowSize.x / 2.0f, windowSize.y * 0.85f});
        window.draw(hintText);
    }
}
