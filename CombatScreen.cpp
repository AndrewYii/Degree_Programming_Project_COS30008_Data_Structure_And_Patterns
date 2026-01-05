#include "CombatScreen.h"
#include "Game.h"
#include "AttackCard.h"
#include "DefenseCard.h"
#include "SkillCard.h"
#include "PowerCard.h"
#include "BinarySearchCard.h"
#include "NeuralNetworkCard.h"
#include "LoadBalancerCard.h"
#include "DrMark.h"
#include "BugMonster.h"
#include "IndexOutOfBoundsBug.h"
#include "MemoryLeakBug.h"
#include "NullPointerBug.h"
#include "StackOverflowBug.h"
#include "SyntaxErrorBug.h"
#include "Logger.h"
#include "AIStoryteller.h"
#include <iostream>
#include <algorithm>
#include <random>
#include <map>

// Constructor [Implementation]
CombatScreen::CombatScreen(Game *gameInstance) : Screen(gameInstance), playerRef(nullptr), currentTurnEntity(nullptr), activeBar(10), undoStack(50), turnNumber(1), hoveredCardIndex(-1), hoveredActiveBarIndex(-1), hoveredButton(-1), mousePressed(false), turnAnimationTimer(0.0f), draggedCard(nullptr), draggedCardIndex(-1), draggedActiveBarCard(nullptr), draggedActiveBarPosition(-1), hoveredSlotIndex(-1), legendPanelOpen(false), legendPanelOffsetX(LEGEND_PANEL_WIDTH),
                                                 showCardDetail(false), selectedCardForDetail(nullptr), cardDetailAlpha(0.0f), playerSprite(nullptr), currentFrame(0), animationTimer(0.0f), enemySprite(nullptr), drMarkFrame(0), drMarkAnimationTimer(0.0f), maxLogEntries(12), logScrollOffset(0), buffScrollOffset(0)
{
    std::cout << "[CombatScreen] Created" << std::endl;
    combatLog = new CombatLogList(20);
    handHashTable = new CardHashTable();
    loadEnemyTextures();
}

// Destructor [Implementation]
CombatScreen::~CombatScreen()
{
    std::cout << "[CombatScreen] Destroyed" << std::endl;
    delete combatLog;
    delete handHashTable;
    Card *current = activeBar.getHead();
    while (current)
    {
        Card *next = current->next;
        delete current;
        current = next;
    }

    if (activeBar.getSingletonCard())
    {
        delete activeBar.getSingletonCard();
    }

    if (activeBar.getObserverCard())
    {
        delete activeBar.getObserverCard();
    }
    for (Card *card : hand)
        delete card;
    for (Card *card : drawPile)
        delete card;
    for (Card *card : discardPile)
        delete card;
    hand.clear();
    drawPile.clear();
    discardPile.clear();
    activeBar.clear();
    delete playerSprite;
    delete enemySprite;
}

// Load enemy textures from files [Implementation]
void CombatScreen::loadEnemyTextures()
{
    std::cout << "[CombatScreen] Loading enemy textures..." << std::endl;
    std::map<std::string, std::string> nameToFile = {
        {"Bug Monster", "BugMonster"},
        {"Syntax Error", "SyntaxError"},
        {"Null Pointer", "NullPointer"},
        {"Memory Leak", "MemoryLeak"},
        {"Stack Overflow", "StackOverflow"},
        {"Index Out of Bounds", "IndexOutOfBounds"},
        {"Dr. Mark", "DrMark"}};

    for (const auto &pair : nameToFile)
    {
        const std::string &displayName = pair.first;
        const std::string &fileName = pair.second;

        sf::Texture texture;
        std::string path = "asset/enemy/" + fileName + ".png";

        if (!texture.loadFromFile(path))
        {
            std::cout << "[CombatScreen] ERROR: Failed to load enemy texture: " << path << std::endl;
        }
        else
        {
            enemyTextures[displayName] = texture; // Store with display name (with spaces)
            std::cout << "[CombatScreen] Loaded enemy texture: " << displayName << " from " << fileName << ".png" << std::endl;
        }
    }
}

// Navigate to this screen override [Implementation]
void CombatScreen::onEnter()
{
    std::cout << "[CombatScreen] Entered" << std::endl;

    playerRef = &(game->getPlayer());

    if (!playerRef)
    {
        std::cout << "[CombatScreen] ERROR: No player!" << std::endl;
        return;
    }

    // Load player character sprite
    loadPlayerSprite();

    // Apply max energy bonus from scenario modifiers
    // Always calculate from the current base (which should be reset in onExit)
    const Player::ScenarioModifiers &mods = playerRef->getScenarioModifiers();

    // Store the base maxBaseCost (before scenario modifiers are applied)
    baseMaxBaseCost = playerRef->getMaxBaseCost();

    if (mods.maxEnergyBonus > 0)
    {
        int newMaxEnergy = baseMaxBaseCost + mods.maxEnergyBonus;
        playerRef->setMaxBaseCost(newMaxEnergy);
        std::cout << "[CombatScreen] Max Energy Bonus Applied: " << mods.maxEnergyBonus
                  << " (Base: " << baseMaxBaseCost << " -> New Max: " << newMaxEnergy << ")" << std::endl;
    }

    // Check for Singleton buff and add special slot
    const Player::RedeemBuffs &buffs = playerRef->getRedeemBuffs();
    if (buffs.singleton)
    {
        activeBar.addSingletonSlot();
        std::cout << "[CombatScreen] Singleton buff active - added green special slot!" << std::endl;
    }

    if (buffs.observer)
    {
        activeBar.addObserverSlot();
        std::cout << "[CombatScreen] Observer buff active - added purple special slot!" << std::endl;
    }

    if (buffs.adapter)
    {
        activeBar.setAdapterBuff(true);
        std::cout << "[CombatScreen] Adapter buff active - bonuses work anywhere!" << std::endl;
    }

    int enemyCount = 1;

    if (rand() % 100 < 20)
    {
        enemyCount = 2;
    }

    // Get current stage level from AIStoryteller
    AIStoryteller *ai = AIStoryteller::getInstance();
    int level = ai->getCurrentStage();

    std::cout << "[CombatScreen] Creating enemies for stage " << level << std::endl;

    createEnemies(enemyCount, level);

    startCombat();
}

// Leave this screen override  [Implementation]
void CombatScreen::onExit()
{
    std::cout << "[CombatScreen] Exited" << std::endl;

    turnQueue.clear();
    currentTurnEntity = nullptr;

    Card *current = activeBar.getHead();
    while (current)
    {
        Card *next = current->next;
        delete current;
        current = next;
    }

    if (activeBar.getSingletonCard())
    {
        delete activeBar.getSingletonCard();
    }

    if (activeBar.getObserverCard())
    {
        delete activeBar.getObserverCard();
    }
    for (Card *card : hand)
        delete card;
    for (Card *card : drawPile)
        delete card;
    for (Card *card : discardPile)
        delete card;

    hand.clear();
    drawPile.clear();
    discardPile.clear();

    activeBar.clear();
    if (handHashTable)
    {
        handHashTable->clear();
    }

    // Clean up enemies
    for (Enemy *e : enemies)
    {
        delete e;
    }
    enemies.clear();

    // Reset player combat stats to prevent carryover between combats
    if (playerRef)
    {
        // Reset maxBaseCost to base value to remove scenario bonuses
        // This prevents energy from accumulating across combats
        playerRef->setMaxBaseCost(baseMaxBaseCost);
        playerRef->refreshBaseCost();

        // Reset all combat-only stats to 0
        playerRef->setBlock(0);
        playerRef->setStrength(0);
        playerRef->addPoison(-playerRef->getPoisonStacks());
        playerRef->addWeak(-playerRef->getWeakStacks());
        playerRef->addVulnerable(-playerRef->getVulnerableStacks());

        std::cout << "[CombatScreen] Player combat stats reset (Energy: " << baseMaxBaseCost << ")" << std::endl;
    }
}

// Create enemies for combat [Implementation]
void CombatScreen::createEnemies(int count, int level)
{
    // Check if this is a boss room
    if (playerRef && playerRef->getIsBossRoom())
    {
        // BOSS ROOM: Spawn only Dr. Mark
        std::cout << "[CombatScreen] BOSS ROOM detected! Spawning Dr. Mark..." << std::endl;

        // Boss stats scale with level
        int bossHP = 150 + (level * 30);   // Higher base HP + scaling
        int bossDamage = 15 + (level * 3); // Higher base damage + scaling

        Enemy *drMark = new DrMark("Dr. Mark", bossHP, bossDamage);

        // AI Storyteller: Influence boss stats
        AIStoryteller *ai = AIStoryteller::getInstance();
        if (drMark)
        {
            ai->influenceEnemyStats(drMark, level);
        }

        enemies.push_back(drMark);

        std::cout << "[CombatScreen] Created BOSS (Lv" << level << "): Dr. Mark (HP: " << drMark->getHP()
                  << ", DMG: " << drMark->getAttackDamage() << ")" << std::endl;
        return; // Exit early, only spawn boss
    }

    // NORMAL ROOM: Spawn regular bug enemies
    const char *bugNames[] = {"Syntax Error", "Null Pointer", "Memory Leak", "Stack Overflow", "Index Out of Bounds", "Bug Monster"};
    const int bugCount = 6;

    for (int i = 0; i < count; i++)
    {
        int bugType = rand() % bugCount;
        std::string name = bugNames[bugType];

        int baseHP = 30 + (rand() % 31);
        int baseDamage = 4 + (rand() % 5);

        int hp = baseHP + (baseHP * level * 20 / 100);
        int damage = baseDamage + (baseDamage * level * 15 / 100);

        Enemy *e = nullptr;

        // Create specialized enemy based on type
        switch (bugType)
        {
        case 0:                                
            e = new SyntaxErrorBug(name, hp, damage); 
            break;
        case 1:                                    
            e = new NullPointerBug(name, hp, damage); 
            break;
        case 2:                                      
            e = new MemoryLeakBug(name, hp, damage, 2); 
            break;
        case 3:                                        
            e = new StackOverflowBug(name, hp, damage, 1);
            break;
        case 4:                                        
            e = new IndexOutOfBoundsBug(name, hp, damage); 
            break;
        case 5:                             
            e = new BugMonster(name, hp, damage); 
            break;
        }

        // AI Storyteller: Influence enemy stats based on tension
        AIStoryteller *ai = AIStoryteller::getInstance();
        if (e)
        {
            ai->influenceEnemyStats(e, level);
        }

        enemies.push_back(e);

        std::cout << "[CombatScreen] Created enemy (Lv" << level << "): " << name << " (HP: " << hp << ", DMG: " << damage << ")" << std::endl;
    }
}

// Start combat sequence [Implementation]
void CombatScreen::startCombat()
{
    std::cout << "[CombatScreen] Combat started!" << std::endl;

    turnNumber = 1;
    addCombatLog("=== COMBAT START ===");

    if (!enemies.empty())
    {
        Logger::getInstance()->logCombatStart(
            enemies[0]->getName(),
            enemies[0]->getHP(),
            enemies[0]->getAttackDamage());
    }

    AIStoryteller::getInstance()->onCombatStart(1, turnNumber);

    createStarterDeck();

    buildEnemyHashTable();
    printHashTableDebug();

    Entity **enemyArray = new Entity *[enemies.size()];
    for (size_t i = 0; i < enemies.size(); i++)
    {
        enemyArray[i] = enemies[i];
    }
    turnQueue.initializeTurnOrder(playerRef, enemyArray, static_cast<int>(enemies.size()));
    delete[] enemyArray;

    startTurn();
}

// Create combat deck from player's master deck [Implementation]
void CombatScreen::createStarterDeck()
{
    // Clear existing cards
    drawPile.clear();
    discardPile.clear();
    hand.clear();
    if (playerRef)
    {
        const std::vector<Card *> &masterDeck = playerRef->getMasterDeck();
        for (Card *card : masterDeck)
        {
            if (card)
            {
                // Clone the card so we don't modify the original
                drawPile.push_back(card->clone());
            }
        }
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(drawPile.begin(), drawPile.end(), g);

    std::cout << "[CombatScreen] Created combat deck from player's master deck: " << drawPile.size() << " cards" << std::endl;
}

// Start a new turn [Implementation]
void CombatScreen::startTurn()
{
    currentTurnEntity = turnQueue.peek();

    if (!currentTurnEntity)
    {
        std::cout << "[CombatScreen] ERROR: No entity in queue!" << std::endl;
        return;
    }

    turnAnimationTimer = 0.0f;

    std::cout << "[CombatScreen] Turn " << turnNumber << " - " << currentTurnEntity->getName() << "'s turn" << std::endl;

    Logger::getInstance()->logCombatTurn(turnNumber);

    if (currentTurnEntity == playerRef)
    {
        playerRef->refreshBaseCost();

        int totalDraw = 5 + playerRef->getBonusDrawNextTurn();
        drawCards(totalDraw);
        playerRef->resetBonusDrawNextTurn();

        const Player::RedeemBuffs &buffs = playerRef->getRedeemBuffs();
        if (buffs.factory && !hand.empty())
        {
            int randomIndex = rand() % hand.size();
            Card *baseCard = hand[randomIndex];
            int copiesAdded = 0;

            for (int i = 0; i < 3; i++)
            {
                Card *duplicate = baseCard->clone();
                if (duplicate)
                {
                    hand.push_back(duplicate);
                    copiesAdded++;
                }
            }

            if (copiesAdded > 0)
            {
                addCombatLog("[FACTORY] Generated " + std::to_string(copiesAdded) + " copies of " + baseCard->getName() + "!");
                std::cout << "[CombatScreen] Factory buff: Generated " << copiesAdded << " copies of " << baseCard->getName() << std::endl;
            }
        }

        std::cout << "[CombatScreen] Hand size after draw: " << hand.size() << std::endl;
    }
}

// End player's turn and process enemy turns [Implementation]
void CombatScreen::endPlayerTurn()
{
    std::cout << "[CombatScreen] Player ended turn" << std::endl;

    // Cards in hand persist across turns
    std::cout << "[CombatScreen] Cards remaining in hand: " << hand.size() << std::endl;

    turnQueue.nextTurn();
    if (checkCombatEnd())
        return;
    while (true)
    {
        currentTurnEntity = turnQueue.peek();

        if (!currentTurnEntity)
            break;

        if (currentTurnEntity == playerRef)
        {
            turnNumber++;
            addCombatLog("--- Turn " + std::to_string(turnNumber) + " ---");
            addCombatLog("Player's turn starts!");
            startTurn();
            break;
        }
        else
        {
            Enemy *enemy = dynamic_cast<Enemy *>(currentTurnEntity);
            if (enemy)
            {
                addCombatLog(enemy->getName() + "'s turn!");
                executeEnemyTurn(enemy);
            }
            turnQueue.nextTurn();
            if (checkCombatEnd())
                return;
        }
    }
}

// Execute enemy's turn [Implementation]
void CombatScreen::executeEnemyTurn(Enemy *enemy)
{
    if (!enemy || !playerRef)
        return;

    std::cout << "[CombatScreen] " << enemy->getName() << " attacks!" << std::endl;

    Logger::getInstance()->logEnemyBehavior(enemy->getName(), "Turn Start", 0);

    enemy->takeTurn();

    // Check if this is DrMark (boss with double attack when enraged)
    DrMark *drMark = dynamic_cast<DrMark *>(enemy);
    int attackCount = (drMark != nullptr) ? drMark->getAttackCount() : 1;

    // Check if this is SyntaxError (may apply debuffs)
    SyntaxErrorBug *syntaxError = dynamic_cast<SyntaxErrorBug *>(enemy);

    // Execute attacks
    for (int i = 0; i < attackCount; i++)
    {
        if (attackCount > 1)
        {
            std::cout << "[CombatScreen] Strike " << (i + 1) << "/" << attackCount << "!" << std::endl;
        }

        int damage = enemy->calculateAttackDamage();

        // Apply damage if any (SyntaxError may return 0)
        if (damage > 0)
        {
            int playerHPBefore = playerRef->getHP();
            playerRef->takeDamage(damage);
            int actualDamage = playerHPBefore - playerRef->getHP();

            Logger::getInstance()->logDamageDealt(enemy->getName(), actualDamage, "Player", false);

            AIStoryteller::getInstance()->onPlayerDamaged(actualDamage);

            std::cout << "[CombatScreen] Player took " << actualDamage << " damage! HP: " << playerRef->getHP() << std::endl;

            if (attackCount > 1)
            {
                addCombatLog(enemy->getName() + " strike " + std::to_string(i + 1) + " for " + std::to_string(actualDamage) + " damage!");
            }
            else
            {
                addCombatLog(enemy->getName() + " attacks for " + std::to_string(actualDamage) + " damage!");
            }
        }
        else
        {
            std::cout << "[CombatScreen] Attack failed (0 damage)!" << std::endl;
            addCombatLog(enemy->getName() + "'s attack failed!");
        }

        // SyntaxError may apply debuffs after attacking
        if (syntaxError != nullptr)
        {
            syntaxError->applyRandomDebuff(playerRef);
        }
    }

    addCombatLog("Player HP: " + std::to_string(playerRef->getHP()) + "/" + std::to_string(playerRef->getMaxHP()));
}

// Draw cards into player's hand [Implementation]
void CombatScreen::drawCards(int count)
{
    int cardsDrawn = 0;

    for (int i = 0; i < count && hand.size() < MAX_HAND_SIZE; i++)
    {
        if (drawPile.empty())
        {
            // Shuffle discard pile back into draw pile
            if (discardPile.empty())
            {
                std::cout << "[CombatScreen] No cards left to draw! Draw pile and discard pile are empty." << std::endl;
                addCombatLog("No more cards to draw! (Draw pile: 0, Discard pile: 0)");
                break;
            }

            std::cout << "[CombatScreen] Shuffling discard pile back into draw pile..." << std::endl;
            addCombatLog("Draw pile empty! Shuffling " + std::to_string(discardPile.size()) + " cards from discard pile...");

            drawPile = discardPile;
            discardPile.clear();

            std::random_device rd;
            std::mt19937 g(rd());
            std::shuffle(drawPile.begin(), drawPile.end(), g);
        }

        // Draw card
        Card *drawnCard = drawPile.back();
        drawPile.pop_back();
        hand.push_back(drawnCard);

        // Add to custom hash table for O(1) lookup!
        handHashTable->insert(drawnCard->getName(), drawnCard);

        std::cout << "[CombatScreen] Drew: " << drawnCard->getName() << std::endl;
        cardsDrawn++;
    }

    if (cardsDrawn > 0)
    {
        addCombatLog("Drew " + std::to_string(cardsDrawn) + " card(s). Hand: " + std::to_string(hand.size()) + "/" + std::to_string(MAX_HAND_SIZE));
    }

    if (hand.size() >= MAX_HAND_SIZE)
    {
        std::cout << "[CombatScreen] Hand is full! (" << hand.size() << "/" << MAX_HAND_SIZE << ")" << std::endl;
    }

    checkForCombos();
}

// Play a card from hand [Implementation]
void CombatScreen::playCard(int cardIndex)
{
    if (cardIndex < 0 || cardIndex >= hand.size())
        return;
    if (!playerRef || enemies.empty())
        return;

    Card *card = hand[cardIndex];

    // Check energy cost (use buff system!)
    int cost = playerRef->calculateEnergyCost(card->getEnergyCost());

    if (playerRef->getCurrentBaseCost() < cost)
    {
        std::cout << "[CombatScreen] Not enough energy! Need " << cost << std::endl;
        return;
    }

    std::cout << "[CombatScreen] Playing: " << card->getName() << " (cost: " << cost << ")" << std::endl;

    std::string cardTypeName = "UNKNOWN";
    switch (card->getType())
    {
    case Card::CardType::ATTACK:
        cardTypeName = "ATTACK";
        break;
    case Card::CardType::DEFENSE:
        cardTypeName = "DEFENSE";
        break;
    case Card::CardType::POWER:
        cardTypeName = "POWER";
        break;
    case Card::CardType::SKILL:
        cardTypeName = "SKILL";
        break;
    }
    Logger::getInstance()->logCardPlayed(card->getName(), cardTypeName, cost);

    Enemy *target = nullptr;
    for (Enemy *e : enemies)
    {
        if (e->getHP() > 0)
        {
            target = e;
            break;
        }
    }

    if (!target)
        return;

    // Apply ActiveBar bonus if available
    ActiveCardMetadata *metadata = activeBar.getCardMetadata(card);
    int activeBonus = (metadata && metadata->canPlayWithBonus) ? metadata->bonusDamage : 0;

    if (activeBonus > 0)
    {
        if (card->getType() == Card::CardType::ATTACK)
        {
            AttackCard *attackCard = dynamic_cast<AttackCard *>(card);
            if (attackCard)
            {
                attackCard->setBaseDamage(attackCard->getBaseDamage() + activeBonus);
                std::cout << "[ActiveBar] Bonus +" << activeBonus << " damage applied to " << card->getName() << "!" << std::endl;
            }
        }
        else if (card->getType() == Card::CardType::DEFENSE)
        {
            DefenseCard *defenseCard = dynamic_cast<DefenseCard *>(card);
            if (defenseCard)
            {
                defenseCard->setBlockAmount(defenseCard->getBlockAmount() + activeBonus);
                std::cout << "[ActiveBar] Bonus +" << activeBonus << " block applied to " << card->getName() << "!" << std::endl;
            }
        }
        else if (card->getType() == Card::CardType::POWER)
        {
            PowerCard *powerCard = dynamic_cast<PowerCard *>(card);
            if (powerCard)
            {
                // Apply bonus to both strength and dexterity gains
                if (powerCard->getStrengthGain() > 0)
                {
                    powerCard->setStrengthGain(powerCard->getStrengthGain() + activeBonus);
                }
                if (powerCard->getDexterityGain() > 0)
                {
                    powerCard->setDexterityGain(powerCard->getDexterityGain() + activeBonus);
                }
                std::cout << "[ActiveBar] Bonus +" << activeBonus << " power applied to " << card->getName() << "!" << std::endl;
            }
        }
        else if (card->getType() == Card::CardType::SKILL)
        {
            // For skill cards, apply bonus to their specific damage values
            BinarySearchCard *binarySearch = dynamic_cast<BinarySearchCard *>(card);
            if (binarySearch)
            {
                binarySearch->setCritDamage(binarySearch->getCritDamage() + activeBonus);
                std::cout << "[ActiveBar] Bonus +" << activeBonus << " crit damage applied to " << card->getName() << "!" << std::endl;
            }
            else
            {
                // Generic bonus for other skill cards
                std::cout << "[ActiveBar] Bonus +" << activeBonus << " applied to skill " << card->getName() << "!" << std::endl;
            }
        }
    }

    // Play card
    card->play(playerRef, target);

    // Track damage for AI
    int damageDealt = 0;
    if (card->getType() == Card::CardType::ATTACK)
    {
        AttackCard *attackCard = dynamic_cast<AttackCard *>(card);
        if (attackCard)
        {
            damageDealt = attackCard->getBaseDamage();
        }
    }

    // AI Storyteller: Track card played
    AIStoryteller::getInstance()->onCardPlayed(damageDealt);

    // Spend energy
    playerRef->modifyBaseCost(-cost);

    // Move to discard
    hand.erase(hand.begin() + cardIndex);
    discardPile.push_back(card);

    // Remove from hash table
    if (handHashTable)
    {
        handHashTable->remove(card->getName(), card);
    }

    // Check if enemy died
    if (checkCombatEnd())
        return;
}

// Discard all cards in hand [Implementation]
void CombatScreen::discardHand()
{
    for (Card *card : hand)
    {
        discardPile.push_back(card);
    }
    hand.clear();

    // Clear hash table since hand is empty
    if (handHashTable)
    {
        handHashTable->clear();
    }
}

// Check if combat has ended [Implementation]
bool CombatScreen::checkCombatEnd()
{
    if (!playerRef)
        return true;

    // Remove dead entities from queue
    turnQueue.removeDeadEntities();

    // Check victory
    if (turnQueue.onlyPlayerRemains())
    {
        std::cout << "[CombatScreen] VICTORY! All enemies defeated!" << std::endl;

        Logger::getInstance()->logCombatEnd(true, playerRef->getHP(), turnNumber);

        float combatDuration = turnNumber * 20.0f;
        AIStoryteller::getInstance()->onCombatEnd(true, combatDuration);
        AIStoryteller::getInstance()->onRoomCleared();

        // Check if this was a boss room
        if (playerRef->getIsBossRoom())
        {
            std::cout << "[CombatScreen] BOSS DEFEATED! Game completed! Going to credits..." << std::endl;
            game->changeScreen(Game::ScreenType::CREDITS);
        }
        else
        {
            // Normal victory - go to victory screen
            game->changeScreen(Game::ScreenType::VICTORY);
        }
        return true;
    }

    if (turnQueue.onlyEnemiesRemain() || playerRef->getHP() <= 0)
    {
        std::cout << "[CombatScreen] DEFEAT! Player died!" << std::endl;

        Logger::getInstance()->logCombatEnd(false, playerRef->getHP(), turnNumber);

        float combatDuration = turnNumber * 20.0f;
        AIStoryteller::getInstance()->onCombatEnd(false, combatDuration);
        AIStoryteller::getInstance()->onPlayerDeath();
        game->changeScreen(Game::ScreenType::DEATH);
        return true;
    }

    return false;
}

// Handle input events override [Implementation]
void CombatScreen::handleEvents()
{
    sf::RenderWindow *window = game->getWindow();

    while (const std::optional event = window->pollEvent())
    {
        if (event->is<sf::Event::Closed>())
        {
            game->quit();
        }

        // Mouse wheel scrolling for combat log
        if (const auto *mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>())
        {
            int totalMessages = combatLog->getSize();
            int maxScroll = std::max(0, totalMessages - maxLogEntries);

            if (mouseWheel->delta > 0)
            {
                // Scroll up (show older messages)
                logScrollOffset = std::min(logScrollOffset + 1, maxScroll);
            }
            else if (mouseWheel->delta < 0)
            {
                // Scroll down (show newer messages)
                logScrollOffset = std::max(logScrollOffset - 1, 0);
            }
        }

        if (const auto *mousePress = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mousePress->button == sf::Mouse::Button::Left)
            {
                mousePressed = true;

                sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
                sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);
                sf::Vector2u windowSize = window->getSize();

                float barY = 520.0f;
                float btnWidth = 120.0f;
                float btnHeight = 50.0f;
                float btnSpacing = 10.0f;
                float btnX = windowSize.x - 40.0f - btnWidth - 10.0f;
                float undoY = barY + 10.0f;
                float confirmY = undoY + btnHeight + btnSpacing;

                sf::FloatRect undoBounds({btnX, undoY}, {btnWidth, btnHeight});
                sf::FloatRect confirmBounds({btnX, confirmY}, {btnWidth, btnHeight});

                if (undoBounds.contains(mousePos))
                {
                    undoLastCard();
                    mousePressed = false;
                    return;
                }
                else if (confirmBounds.contains(mousePos))
                {
                    executeActiveBar();
                    mousePressed = false;
                    return;
                }

                if (currentTurnEntity == playerRef && !draggedActiveBarCard)
                {
                    float activeBarY = 520.0f;
                    float startX = 100.0f;
                    float cardW = 80.0f;
                    float cardH = 100.0f;
                    float spacing = 90.0f;

                    Card *current = activeBar.getHead();
                    int position = 0;

                    while (current)
                    {
                        float cardX = startX + (position * spacing);
                        float cardY = activeBarY + 10;
                        sf::FloatRect cardBounds({cardX, cardY}, {cardW, cardH});

                        if (cardBounds.contains(mousePos))
                        {
                            draggedActiveBarCard = current;
                            draggedActiveBarPosition = position;
                            dragOffset = {mousePos.x - cardX, mousePos.y - cardY};
                            break;
                        }

                        current = current->next;
                        position++;
                    }

                    // If not dragging normal card, check special slots
                    if (!draggedActiveBarCard)
                    {
                        // Check singleton slot
                        int singletonSlot = activeBar.getSingletonSlotIndex();
                        if (singletonSlot >= 0 && activeBar.getSingletonCard())
                        {
                            float cardX = startX + (singletonSlot * spacing);
                            float cardY = barY + 10;
                            sf::FloatRect cardBounds({cardX, cardY}, {cardW, cardH});

                            if (cardBounds.contains(mousePos))
                            {
                                draggedActiveBarCard = activeBar.getSingletonCard();
                                draggedActiveBarPosition = singletonSlot;
                                dragOffset = {mousePos.x - cardX, mousePos.y - cardY};
                            }
                        }

                        // Check observer slot
                        if (!draggedActiveBarCard)
                        {
                            int observerSlot = activeBar.getObserverSlotIndex();
                            if (observerSlot >= 0 && activeBar.getObserverCard())
                            {
                                float cardX = startX + (observerSlot * spacing);
                                float cardY = barY + 10;
                                sf::FloatRect cardBounds({cardX, cardY}, {cardW, cardH});

                                if (cardBounds.contains(mousePos))
                                {
                                    draggedActiveBarCard = activeBar.getObserverCard();
                                    draggedActiveBarPosition = observerSlot;
                                    dragOffset = {mousePos.x - cardX, mousePos.y - cardY};
                                }
                            }
                        }
                    }
                }

                // Check if clicking on a card in hand (only if not dragging Active Bar card)
                if (currentTurnEntity == playerRef && !draggedCard && !draggedActiveBarCard)
                {
                    sf::Vector2u handWindowSize = window->getSize();
                    float cardAreaY = handWindowSize.y - CARD_HEIGHT - 20.0f;
                    float totalWidth = hand.size() * (CARD_WIDTH + CARD_SPACING) - CARD_SPACING;
                    float startX = (handWindowSize.x - totalWidth) / 2.0f;

                    for (size_t i = 0; i < hand.size(); i++)
                    {
                        float cardX = startX + i * (CARD_WIDTH + CARD_SPACING);
                        sf::FloatRect cardBounds({cardX, cardAreaY}, {CARD_WIDTH, CARD_HEIGHT});

                        if (cardBounds.contains(mousePos))
                        {
                            draggedCard = hand[i];
                            draggedCardIndex = (int)i;
                            dragOffset = {mousePos.x - cardX, mousePos.y - cardAreaY};
                            break;
                        }
                    }
                }
            }
        }

        if (const auto *mouseRelease = event->getIf<sf::Event::MouseButtonReleased>())
        {
            if (mouseRelease->button == sf::Mouse::Button::Left)
            {
                mousePressed = false;

                // Drop Active Bar card (reordering or removing)
                if (draggedActiveBarCard)
                {
                    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
                    sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);

                    // Check if dropped outside Active Bar area (remove card)
                    sf::Vector2u activeBarWindowSize = window->getSize();
                    float barY = 520.0f;
                    sf::FloatRect activeBarArea({20, barY}, {activeBarWindowSize.x - 40.0f, 200.0f});

                    if (!activeBarArea.contains(mousePos))
                    {
                        // Remove card from Active Bar and return to hand
                        removeCardFromActiveBar(draggedActiveBarCard);
                    }
                    else
                    {
                        // Reordering within Active Bar (only for normal DLL cards)
                        bool isSpecialSlot = (draggedActiveBarCard == activeBar.getSingletonCard() ||
                                              draggedActiveBarCard == activeBar.getObserverCard());

                        if (!isSpecialSlot && activeBar.getCardCount() > 0)
                        {
                            // Calculate which position to drop it at
                            float startX = 100.0f;
                            float spacing = 90.0f;

                            // Find closest position
                            int newPosition = static_cast<int>((mousePos.x - startX + spacing / 2.0f) / spacing);
                            newPosition = std::max(0, std::min(newPosition, activeBar.getCardCount() - 1));

                            // Reorder if position changed
                            if (newPosition != draggedActiveBarPosition)
                            {
                                std::cout << "[ActiveBar] Reordering card from " << draggedActiveBarPosition << " to " << newPosition << std::endl;

                                activeBar.moveCard(draggedActiveBarPosition, newPosition);
                            }
                        }
                    }

                    // Reset drag state
                    draggedActiveBarCard = nullptr;
                    draggedActiveBarPosition = -1;
                }

                // Drop card from hand
                if (draggedCard)
                {
                    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
                    sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);

                    sf::Vector2u dropWindowSize = window->getSize();
                    float barY = 480.0f;      // Start higher for easier detection
                    float barHeight = 240.0f; // Make taller for easier targeting
                    sf::FloatRect activeBarArea({20, barY}, {dropWindowSize.x - 40.0f, barHeight});

                    if (activeBarArea.contains(mousePos))
                    {
                        float startX = 100.0f;
                        float cardW = 80.0f;
                        float cardH = 100.0f;
                        float spacing = 90.0f;
                        float cardY = 520.0f + 10;

                        int singletonSlot = activeBar.getSingletonSlotIndex();
                        int observerSlot = activeBar.getObserverSlotIndex();

                        bool droppedOnSpecialSlot = false;

                        // Check if dropped on SINGLETON slot (exact position required)
                        if (singletonSlot >= 0)
                        {
                            float slotX = startX + (singletonSlot * spacing);
                            sf::FloatRect singletonBounds({slotX, cardY}, {cardW, cardH});

                            if (singletonBounds.contains(mousePos))
                            {
                                std::cout << "[ActiveBar] Dropped on SINGLETON slot!" << std::endl;
                                addCardToActiveBar(draggedCardIndex, singletonSlot);
                                droppedOnSpecialSlot = true;
                            }
                        }

                        // Check if dropped on OBSERVER slot (exact position required)
                        if (!droppedOnSpecialSlot && observerSlot >= 0)
                        {
                            float slotX = startX + (observerSlot * spacing);
                            sf::FloatRect observerBounds({slotX, cardY}, {cardW, cardH});

                            if (observerBounds.contains(mousePos))
                            {
                                std::cout << "[ActiveBar] Dropped on OBSERVER slot!" << std::endl;
                                addCardToActiveBar(draggedCardIndex, observerSlot);
                                droppedOnSpecialSlot = true;
                            }
                        }

                        // If NOT dropped on special slot, add to NORMAL slots (easy mode!)
                        if (!droppedOnSpecialSlot)
                        {
                            // Add to next available normal slot (no exact position needed!)
                            std::cout << "[ActiveBar] Dropped in normal area - adding to next available slot" << std::endl;
                            addCardToActiveBar(draggedCardIndex, -1); // -1 means add to tail/next available
                        }
                    }
                    else
                    {
                        std::cout << "[ActiveBar] Dropped outside Active Bar area" << std::endl;
                    }

                    // Reset drag state
                    draggedCard = nullptr;
                    draggedCardIndex = -1;
                }

                // Handle panel interactions (only if not dragging)
                if (!draggedCard && !draggedActiveBarCard)
                {
                    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
                    sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);
                    sf::Vector2u panelWindowSize = window->getSize();

                    // Check if clicking card detail popup close (click outside popup)
                    if (showCardDetail)
                    {
                        // Popup bounds
                        float popupW = 500.0f;
                        float popupH = 600.0f;
                        float popupX = (panelWindowSize.x - popupW) / 2.0f;
                        float popupY = (panelWindowSize.y - popupH) / 2.0f;
                        sf::FloatRect popupBounds({popupX, popupY}, {popupW, popupH});

                        // If clicking outside popup, close it
                        if (!popupBounds.contains(mousePos))
                        {
                            showCardDetail = false;
                            selectedCardForDetail = nullptr;
                        }
                    }
                    else // Only check other clicks if detail popup is closed
                    {
                        // Check legend panel arrow button
                        float panelX = panelWindowSize.x - LEGEND_PANEL_WIDTH + legendPanelOffsetX;
                        float panelY = 100.0f;
                        float panelH = panelWindowSize.y - 200.0f;

                        // Arrow button is at left edge of panel
                        float arrowX = panelX - 30.0f;
                        float arrowY = panelY + panelH / 2.0f - 40.0f;
                        sf::FloatRect arrowBounds({arrowX, arrowY}, {30.0f, 80.0f});

                        if (arrowBounds.contains(mousePos))
                        {
                            legendPanelOpen = !legendPanelOpen;
                            std::cout << "[Legend Panel] Toggled: " << (legendPanelOpen ? "OPEN" : "CLOSED") << std::endl;
                        }
                        else
                        {
                            // Check if clicking on a card in hand (for detail view)
                            float cardAreaY = panelWindowSize.y - CARD_HEIGHT - 20.0f;
                            float totalWidth = hand.size() * (CARD_WIDTH + CARD_SPACING) - CARD_SPACING;
                            float startX = (panelWindowSize.x - totalWidth) / 2.0f;

                            for (size_t i = 0; i < hand.size(); i++)
                            {
                                float cardX = startX + i * (CARD_WIDTH + CARD_SPACING);
                                sf::FloatRect cardBounds({cardX, cardAreaY}, {CARD_WIDTH, CARD_HEIGHT});

                                if (cardBounds.contains(mousePos))
                                {
                                    selectedCardForDetail = hand[i];
                                    showCardDetail = true;
                                    std::cout << "[Card Detail] Opened for: " << hand[i]->getName() << std::endl;
                                    break;
                                }
                            }

                            // If not clicking hand card, check Active Bar cards
                            if (!showCardDetail)
                            {
                                float barY = 520.0f;
                                float startX_bar = 100.0f;
                                float cardW = 80.0f;
                                float cardH = 100.0f;
                                float spacing = 90.0f;

                                Card *current = activeBar.getHead();
                                int position = 0;

                                while (current)
                                {
                                    float cardX = startX_bar + (position * spacing);
                                    float cardY = barY + 10;
                                    sf::FloatRect cardBounds({cardX, cardY}, {cardW, cardH});

                                    if (cardBounds.contains(mousePos))
                                    {
                                        selectedCardForDetail = current;
                                        showCardDetail = true;
                                        std::cout << "[Card Detail] Opened for Active Bar card: " << current->getName() << std::endl;
                                        break;
                                    }

                                    current = current->next;
                                    position++;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

// Update logic override [Implementation]
void CombatScreen::update(float deltaTime)
{
    // Update animation timer
    turnAnimationTimer += deltaTime;

    // Update character animation (always animate)
    animationTimer += deltaTime;
    if (animationTimer >= FRAME_TIME)
    {
        animationTimer -= FRAME_TIME;
        currentFrame = (currentFrame + 1) % 4; // Cycle through 4 frames (0-3)
    }

    // Update Dr. Mark animation (if Dr. Mark is in combat)
    bool drMarkPresent = false;
    for (auto *enemy : enemies)
    {
        if (enemy && enemy->getName() == "Dr. Mark")
        {
            drMarkPresent = true;
            break;
        }
    }

    if (drMarkPresent)
    {
        drMarkAnimationTimer += deltaTime;
        if (drMarkAnimationTimer >= 0.2f) // 0.2 seconds per frame (faster than player)
        {
            drMarkAnimationTimer -= 0.2f;
            drMarkFrame = (drMarkFrame + 1) % 4; // Cycle through 4 frames (0-3)
        }
    }

    // Check if it's player's turn
    bool isPlayerTurn = (currentTurnEntity == playerRef);

    if (!isPlayerTurn || !playerRef)
        return;

    sf::RenderWindow *window = game->getWindow();
    sf::Vector2u windowSize = window->getSize();
    sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*window);
    sf::Vector2f mousePos = window->mapPixelToCoords(mousePixelPos);

    // Calculate card positions
    float cardAreaY = windowSize.y - CARD_HEIGHT - 20.0f;
    float totalWidth = hand.size() * (CARD_WIDTH + CARD_SPACING) - CARD_SPACING;
    float startX = (windowSize.x - totalWidth) / 2.0f;

    // Check hover (but don't play cards directly if we're using drag system)
    hoveredCardIndex = -1;

    for (size_t i = 0; i < hand.size(); i++)
    {
        float cardX = startX + i * (CARD_WIDTH + CARD_SPACING);
        sf::FloatRect cardBounds({cardX, cardAreaY}, {CARD_WIDTH, CARD_HEIGHT});

        if (cardBounds.contains(mousePos))
        {
            hoveredCardIndex = (int)i;
        }
    }

    // Active Bar button positions (top-right corner)
    float barY = 520.0f;
    float btnWidth = 120.0f;
    float btnHeight = 50.0f;
    float btnSpacing = 10.0f;
    float btnX = windowSize.x - 40.0f - btnWidth - 10.0f;

    // UNDO button (top)
    float undoY = barY + 10.0f;
    sf::FloatRect undoBounds({btnX, undoY}, {btnWidth, btnHeight});

    // CONFIRM button (below UNDO)
    float confirmY = undoY + btnHeight + btnSpacing;
    sf::FloatRect confirmBounds({btnX, confirmY}, {btnWidth, btnHeight});

    // Check hover state (do this BEFORE click detection)
    hoveredButton = -1; // Reset
    if (undoBounds.contains(mousePos))
    {
        hoveredButton = 0;
    }
    else if (confirmBounds.contains(mousePos))
    {
        hoveredButton = 1;
    }

    // Animate legend panel slide (ONLY when state changes)
    if (legendPanelOpen && legendPanelOffsetX > 0.1f)
    {
        legendPanelOffsetX -= LEGEND_SLIDE_SPEED * deltaTime;
        if (legendPanelOffsetX < 0)
            legendPanelOffsetX = 0;
    }
    else if (!legendPanelOpen && legendPanelOffsetX < LEGEND_PANEL_WIDTH - 0.1f)
    {
        legendPanelOffsetX += LEGEND_SLIDE_SPEED * deltaTime;
        if (legendPanelOffsetX > LEGEND_PANEL_WIDTH)
            legendPanelOffsetX = LEGEND_PANEL_WIDTH;
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
}

// Render combat screen override [Implementation]
void CombatScreen::render(sf::RenderWindow &window, sf::Font &font)
{
    window.clear(sf::Color(240, 240, 230)); // Off-white background

    if (!playerRef)
        return;

    sf::Vector2u windowSize = window.getSize();
    const sf::Color COLOR_BLACK = Game::COLOR_BLACK;

    float playerX = windowSize.x * 0.15f;
    float playerY = windowSize.y * 0.30f;
    bool isPlayerActive = (currentTurnEntity == playerRef);

    // Draw player character sprite with animation
    if (playerSprite && playerTexture.getNativeHandle())
    {
        sf::Vector2u textureSize = playerTexture.getSize();
        int frameWidth = textureSize.x / 4; // 4 frames
        int frameHeight = textureSize.y;

        sf::IntRect frameRect({currentFrame * frameWidth, 0}, {frameWidth, frameHeight});
        playerSprite->setTextureRect(frameRect);

        // Scale to reasonable size (100x100 pixels)
        float scale = 100.0f / frameWidth;
        playerSprite->setScale({scale, scale});

        playerSprite->setPosition({playerX - 50, playerY - 50});
        window.draw(*playerSprite);
    }

    // Player name
    sf::Text playerNameText(font, playerRef->getName(), 32);
    playerNameText.setFillColor(isPlayerActive ? sf::Color(0, 200, 0) : COLOR_BLACK);
    playerNameText.setStyle(sf::Text::Bold);
    playerNameText.setPosition({playerX - 50, playerY - 100});
    window.draw(playerNameText);

    // Player role
    sf::Text roleText(font, playerRef->getRoleName(), 18);
    roleText.setFillColor(sf::Color(100, 100, 100));
    roleText.setPosition({playerX - 50, playerY - 70});
    window.draw(roleText);

    // Player health bar (MEMORY themed)
    drawHealthBar(window, font, playerX - 100, playerY + 100, 200.0f,
                  playerRef->getHP(), playerRef->getMaxHP(), "MEMORY");

    // Player defense bar (always show, even at 0)
    drawDefenseBar(window, font, playerX - 100, playerY + 135, 200.0f,
                   playerRef->getBlock());

    float startEnemyX = windowSize.x * 0.60f;
    float enemyY = windowSize.y * 0.30f;
    float enemySpacing = 200.0f;

    for (size_t i = 0; i < enemies.size(); i++)
    {
        Enemy *enemy = enemies[i];
        if (!enemy || enemy->getHP() <= 0)
            continue;

        float enemyX = startEnemyX + (i * enemySpacing);
        bool isActive = (currentTurnEntity == enemy);

        std::string enemyName = enemy->getName();

        // Find texture - fallback to BugMonster if enemy-specific texture not found
        std::string textureName = enemyName;
        if (enemyTextures.find(textureName) == enemyTextures.end())
        {
            std::cout << "[CombatScreen] Texture not found for " << enemyName << ", using BugMonster fallback" << std::endl;
            textureName = "Bug Monster"; // Fallback to generic bug
        }

        if (enemyTextures.find(textureName) != enemyTextures.end())
        {
            delete enemySprite;
            enemySprite = new sf::Sprite(enemyTextures[textureName]);

            // For Dr. Mark, use animation frames
            if (textureName == "Dr. Mark")
            {
                // Set the appropriate frame rectangle
                switch (drMarkFrame)
                {
                case 0:
                    enemySprite->setTextureRect(sf::IntRect({0, 0}, {85, 158}));
                    break;
                case 1:
                    enemySprite->setTextureRect(sf::IntRect({97, 0}, {84, 158}));
                    break;
                case 2:
                    enemySprite->setTextureRect(sf::IntRect({199, 0}, {87, 158}));
                    break;
                case 3:
                    enemySprite->setTextureRect(sf::IntRect({287, 0}, {94, 158}));
                    break;
                }
            }
            else
            {
                // For regular enemies, use full texture (500x500)
                sf::Vector2u texSize = enemyTextures[textureName].getSize();
                enemySprite->setTextureRect(sf::IntRect({0, 0}, {(int)texSize.x, (int)texSize.y}));
            }

            // Scale sprite to fit 150x150 box
            sf::FloatRect bounds = enemySprite->getLocalBounds();
            float maxDim = std::max(bounds.size.x, bounds.size.y);
            float scale = 150.0f / maxDim;
            enemySprite->setScale({scale, scale});

            // Center the sprite
            enemySprite->setPosition({enemyX - 75, enemyY - 75});
            window.draw(*enemySprite);
        }

        // Enemy name (ABOVE sprite - consistent with player)
        sf::Text enemyNameText(font, enemy->getName(), 24);
        enemyNameText.setFillColor(isActive ? sf::Color(200, 0, 0) : COLOR_BLACK);
        enemyNameText.setStyle(sf::Text::Bold);
        enemyNameText.setPosition({enemyX - 70, enemyY - 90}); // Above sprite
        window.draw(enemyNameText);

        // Enemy health bar (MEMORY themed)
        drawHealthBar(window, font, enemyX - 80, enemyY + 100, 160.0f,
                      enemy->getHP(), enemy->getMaxHP(), "MEMORY");

        // Enemy defense bar (always show, even at 0)
        drawDefenseBar(window, font, enemyX - 80, enemyY + 135, 160.0f,
                       enemy->getBlock());
    }

    drawEnergyDisplay(window, font);

    drawActiveBar(window, font);

    bool isPlayerTurn = (currentTurnEntity == playerRef);

    if (isPlayerTurn && !hand.empty())
    {
        float cardAreaY = windowSize.y - CARD_HEIGHT - 20.0f;
        float totalWidth = hand.size() * (CARD_WIDTH + CARD_SPACING) - CARD_SPACING;
        float startX = (windowSize.x - totalWidth) / 2.0f;

        for (size_t i = 0; i < hand.size(); i++)
        {
            // Skip drawing the card being dragged
            if (draggedCard && (int)i == draggedCardIndex)
                continue;

            float cardX = startX + i * (CARD_WIDTH + CARD_SPACING);
            bool isHovered = ((int)i == hoveredCardIndex);

            drawCard(window, font, hand[i], cardX, cardAreaY, isHovered);
        }
    }

    if (draggedCard && isPlayerTurn)
    {
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);
        float dragX = mousePos.x - dragOffset.x;
        float dragY = mousePos.y - dragOffset.y;

        // Draw semi-transparent card at mouse position
        drawCard(window, font, draggedCard, dragX, dragY, true);

        // Add transparency overlay
        sf::RectangleShape overlay({CARD_WIDTH, CARD_HEIGHT});
        overlay.setPosition({dragX, dragY});
        overlay.setFillColor(sf::Color(255, 255, 255, 100));
        window.draw(overlay);
    }

    drawCombatLog(window, font);
    drawLegendPanel(window, font);
    if (cardDetailAlpha > 0)
    {
        drawCardDetailPopup(window, font);
    }
}

// Draw a single card at specified position [Implementation]
void CombatScreen::drawCard(sf::RenderWindow &window, sf::Font &font, Card *card,
                            float x, float y, bool isHovered)
{
    if (!card || !playerRef)
        return;

    const sf::Color COLOR_BLACK = Game::COLOR_BLACK;

    // Lift card up when hovered
    float liftY = isHovered ? -30.0f : 0.0f;

    // Card background (light gray)
    sf::RectangleShape cardRect({CARD_WIDTH, CARD_HEIGHT});
    cardRect.setPosition({x, y + liftY});
    cardRect.setFillColor(sf::Color(240, 240, 240));

    // Border color by card type
    sf::Color borderColor;
    switch (card->getType())
    {
    case Card::CardType::ATTACK:
        borderColor = sf::Color(255, 255, 0); // Yellow
        break;
    case Card::CardType::DEFENSE:
        borderColor = sf::Color(50, 150, 255); // Blue
        break;
    case Card::CardType::SKILL:
        borderColor = sf::Color(100, 255, 100); // Green
        break;
    case Card::CardType::POWER:
        borderColor = sf::Color(255, 0, 255); // Purple
        break;
    default:
        borderColor = COLOR_BLACK;
    }

    cardRect.setOutlineColor(isHovered ? sf::Color(255, 215, 0) : borderColor);
    cardRect.setOutlineThickness(isHovered ? 4.0f : 3.0f);
    window.draw(cardRect);

    // Draw icon sprite if available
    sf::Sprite *iconSprite = card->getIconSprite();
    if (iconSprite)
    {
        // Scale icon to fit card (leave space for cost circle)
        sf::Vector2f iconSize = {CARD_WIDTH - 20.0f, CARD_HEIGHT - 20.0f};
        const sf::Texture &iconTexture = iconSprite->getTexture();
        sf::Vector2u texSize = iconTexture.getSize();

        // Safety check: ensure texture has valid dimensions
        if (texSize.x > 0 && texSize.y > 0)
        {
            float scaleX = iconSize.x / texSize.x;
            float scaleY = iconSize.y / texSize.y;
            float scale = std::min(scaleX, scaleY);

            iconSprite->setScale({scale, scale});

            // Center icon in card
            sf::Vector2f scaledSize = {texSize.x * scale, texSize.y * scale};
            float iconX = x + (CARD_WIDTH - scaledSize.x) / 2.0f;
            float iconY = y + liftY + (CARD_HEIGHT - scaledSize.y) / 2.0f;
            iconSprite->setPosition({iconX, iconY});

            window.draw(*iconSprite);
        }
    }

    if (card->isUpgraded())
    {
        sf::CircleShape starCircle(12.0f, 5);
        starCircle.setPosition({x + CARD_WIDTH - 30, y + liftY + 5});
        starCircle.setFillColor(sf::Color(255, 215, 0));
        starCircle.setOutlineColor(sf::Color(255, 165, 0));
        starCircle.setOutlineThickness(2.0f);
        window.draw(starCircle);

        sf::Text starText(font, "+", 20);
        starText.setFillColor(sf::Color::Black);
        starText.setStyle(sf::Text::Bold);
        starText.setPosition({x + CARD_WIDTH - 25, y + liftY + 5});
        window.draw(starText);
    }

    int cost = playerRef->calculateEnergyCost(card->getEnergyCost());

    sf::Text costText(font, std::to_string(cost), 28);
    costText.setFillColor(COLOR_BLACK);
    costText.setStyle(sf::Text::Bold);
    costText.setPosition({x + 10, y + liftY + 10});
    window.draw(costText);

    AttackCard *attackCard = dynamic_cast<AttackCard *>(card);
    DefenseCard *defenseCard = dynamic_cast<DefenseCard *>(card);

    if (attackCard)
    {
        int damage = attackCard->getBaseDamage();
        sf::RectangleShape powerBox({40.0f, 30.0f});
        powerBox.setPosition({x + CARD_WIDTH - 50, y + liftY + CARD_HEIGHT - 40});
        powerBox.setFillColor(sf::Color(255, 100, 100));
        powerBox.setOutlineColor(sf::Color::Black);
        powerBox.setOutlineThickness(2.0f);
        window.draw(powerBox);

        sf::Text powerText(font, std::to_string(damage), 20);
        powerText.setFillColor(sf::Color::White);
        powerText.setStyle(sf::Text::Bold);
        sf::FloatRect powerBounds = powerText.getLocalBounds();
        powerText.setPosition({x + CARD_WIDTH - 30 - powerBounds.size.x / 2.0f, y + liftY + CARD_HEIGHT - 35});
        window.draw(powerText);
    }
    else if (defenseCard)
    {
        int block = defenseCard->getBlockAmount();
        sf::RectangleShape powerBox({40.0f, 30.0f});
        powerBox.setPosition({x + CARD_WIDTH - 50, y + liftY + CARD_HEIGHT - 40});
        powerBox.setFillColor(sf::Color(100, 200, 255));
        powerBox.setOutlineColor(sf::Color::Black);
        powerBox.setOutlineThickness(2.0f);
        window.draw(powerBox);

        sf::Text powerText(font, std::to_string(block), 20);
        powerText.setFillColor(sf::Color::White);
        powerText.setStyle(sf::Text::Bold);
        sf::FloatRect powerBounds = powerText.getLocalBounds();
        powerText.setPosition({x + CARD_WIDTH - 30 - powerBounds.size.x / 2.0f, y + liftY + CARD_HEIGHT - 35});
        window.draw(powerText);
    }
}

// Draw health bar [Implementation]
void CombatScreen::drawHealthBar(sf::RenderWindow &window, sf::Font &font, float x, float y, float width, int current, int max, const std::string &label)
{
    const sf::Color COLOR_BLACK = Game::COLOR_BLACK;

    // Background
    sf::RectangleShape bgBar({width, 30.0f});
    bgBar.setPosition({x, y});
    bgBar.setFillColor(sf::Color(100, 100, 100));
    bgBar.setOutlineColor(COLOR_BLACK);
    bgBar.setOutlineThickness(2.0f);
    window.draw(bgBar);

    // Health fill
    float fillWidth = (static_cast<float>(current) / max) * width;
    sf::RectangleShape fillBar({fillWidth, 30.0f});
    fillBar.setPosition({x, y});
    fillBar.setFillColor(sf::Color(200, 50, 50));
    window.draw(fillBar);

    // Text
    std::string hpText = label + ": " + std::to_string(current) + "/" + std::to_string(max);
    sf::Text text(font, hpText, 18);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
    text.setPosition({x + width / 2.0f, y + 15});
    window.draw(text);
}

// Draw defense bar [Implementation]
void CombatScreen::drawDefenseBar(sf::RenderWindow &window, sf::Font &font, float x, float y, float width, int blockAmount)
{
    const sf::Color COLOR_BLACK = Game::COLOR_BLACK;

    // Background
    sf::RectangleShape bgBar({width, 25.0f});
    bgBar.setPosition({x, y});
    bgBar.setFillColor(sf::Color(60, 60, 80));
    bgBar.setOutlineColor(COLOR_BLACK);
    bgBar.setOutlineThickness(2.0f);
    window.draw(bgBar);

    // Defense fill (cyan/blue gradient)
    sf::RectangleShape fillBar({width, 25.0f});
    fillBar.setPosition({x, y});
    fillBar.setFillColor(sf::Color(100, 200, 255)); // Cyan blue
    window.draw(fillBar);

    // Shield icon (left side)
    sf::Text shieldIcon(font, "[S]", 18);
    shieldIcon.setFillColor(sf::Color::White);
    shieldIcon.setStyle(sf::Text::Bold);
    shieldIcon.setPosition({x + 5, y + 3});
    window.draw(shieldIcon);

    // Defense text (right side)
    std::string defenseText = "DEF: " + std::to_string(blockAmount);
    sf::Text text(font, defenseText, 18);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
    text.setPosition({x + width / 2.0f + 15, y + 12});
    window.draw(text);
}

// Draw energy display [Implementation]
void CombatScreen::drawEnergyDisplay(sf::RenderWindow &window, sf::Font &font)
{
    if (!playerRef)
        return;

    sf::Vector2u windowSize = window.getSize();
    const sf::Color COLOR_BLACK = Game::COLOR_BLACK;

    float energyX = windowSize.x - 150.0f;
    float energyY = 80.0f;

    // Calculate preview energy (current - active bar cost)
    int currentEnergy = playerRef->getCurrentBaseCost();
    int activeBarCost = activeBar.getTotalEnergyCost(playerRef);
    int previewEnergy = currentEnergy - activeBarCost;

    // Energy circle - change color if not enough energy
    sf::CircleShape energyCircle(40.0f);
    energyCircle.setPosition({energyX, energyY});

    if (previewEnergy < 0)
    {
        // Red if not enough energy
        energyCircle.setFillColor(sf::Color(255, 100, 100));
    }
    else if (activeBarCost > 0)
    {
        // Yellow if preview active (cards in bar)
        energyCircle.setFillColor(sf::Color(255, 200, 100));
    }
    else
    {
        // Blue if normal
        energyCircle.setFillColor(sf::Color(100, 200, 255));
    }

    energyCircle.setOutlineColor(COLOR_BLACK);
    energyCircle.setOutlineThickness(3.0f);
    window.draw(energyCircle);
    std::string energyText;
    if (activeBarCost > 0)
    {
        energyText = std::to_string(previewEnergy) + "/" + std::to_string(playerRef->getMaxBaseCost());
    }
    else
    {
        energyText = std::to_string(currentEnergy) + "/" + std::to_string(playerRef->getMaxBaseCost());
    }

    sf::Text text(font, energyText, 28);
    text.setFillColor(sf::Color::White);
    text.setStyle(sf::Text::Bold);
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setOrigin({textBounds.size.x / 2.0f, textBounds.size.y / 2.0f});
    text.setPosition({energyX + 40, energyY + 40});
    window.draw(text);

    sf::Text labelText(font, "CPU", 18);
    labelText.setFillColor(COLOR_BLACK);
    labelText.setPosition({energyX + 10, energyY + 90});
    window.draw(labelText);
}

// Draw turn order bar [Implementation]
void CombatScreen::drawTurnOrderBar(sf::RenderWindow &window, sf::Font &font)
{
    if (turnQueue.isEmpty())
        return;

    sf::Vector2u windowSize = window.getSize();
    const sf::Color COLOR_BLACK = Game::COLOR_BLACK;

    // Vertical stack on RIGHT side
    float startX = windowSize.x - 150.0f;
    float startY = 50.0f;
    float rectWidth = 120.0f;
    float rectHeight = 50.0f;
    float rectSpacing = 5.0f;

    // Draw "ACTION QUEUE" label
    sf::Text label(font, "ACTION QUEUE", 16);
    label.setFillColor(COLOR_BLACK);
    label.setStyle(sf::Text::Bold);
    label.setPosition({startX, startY - 25});
    window.draw(label);

    // Traverse queue and draw each entity as vertical rectangles
    Entity *current = turnQueue.peek();
    int position = 0;

    while (current && position < 12)
    { // Max 12 entities shown
        float y = startY + (position * (rectHeight + rectSpacing));

        // Draw rectangle background
        sf::RectangleShape rect({rectWidth, rectHeight});
        rect.setPosition({startX, y});

        // Color based on type
        if (current == playerRef)
        {
            rect.setFillColor(sf::Color(120, 200, 120));
        }
        else
        {
            rect.setFillColor(sf::Color(220, 100, 100));
        }

        // Highlight current turn with gold border
        if (current == currentTurnEntity)
        {
            rect.setOutlineColor(sf::Color(255, 215, 0));
            rect.setOutlineThickness(4.0f);
        }
        else
        {
            rect.setOutlineColor(COLOR_BLACK);
            rect.setOutlineThickness(2.0f);
        }

        window.draw(rect);
        std::string name = current->getName();
        if (name.length() > 12)
            name = name.substr(0, 10) + "..";

        sf::Text nameText(font, name, 14);
        nameText.setFillColor(COLOR_BLACK);
        nameText.setStyle(sf::Text::Bold);
        nameText.setPosition({startX + 5, y + 5});
        window.draw(nameText);

        std::string hpStr = std::to_string(current->getHP()) + " HP";
        sf::Text hpText(font, hpStr, 12);
        hpText.setFillColor(COLOR_BLACK);
        hpText.setPosition({startX + 5, y + 28});
        window.draw(hpText);

        // Draw position number (turn order)
        sf::Text posText(font, "#" + std::to_string(position + 1), 12);
        posText.setFillColor(COLOR_BLACK);
        posText.setStyle(sf::Text::Bold);
        posText.setPosition({startX + rectWidth - 25, y + 18});
        window.draw(posText);

        // Move to next in queue
        current = current->next;
        position++;
    }
}

// Load player sprite based on role [Implementation]
void CombatScreen::loadPlayerSprite()
{
    if (!playerRef)
        return;

    std::string roleName = playerRef->getRoleName();
    std::string spritePath = "asset/character/";

    if (roleName == "FRONTEND DEV")
        spritePath += "Frontenddeveloper.png";
    else if (roleName == "BACKEND DEV")
        spritePath += "BackendDeveloper.png";
    else if (roleName == "DATA SCIENTIST")
        spritePath += "Datascientist.png";
    else if (roleName == "DEVOPS ENGINEER")
        spritePath += "DevOsDeveloper.png";
    else if (roleName == "FULL STACK DEV")
        spritePath += "Fullstackdeveloper.png";
    else
        spritePath += "Frontenddeveloper.png";

    if (!playerTexture.loadFromFile(spritePath))
    {
        std::cout << "[CombatScreen] Failed to load player sprite: " << spritePath << std::endl;
        return;
    }

    delete playerSprite;
    playerSprite = new sf::Sprite(playerTexture);
    std::cout << "[CombatScreen] Loaded player sprite: " << spritePath << std::endl;
}

// Draw Active Bar [Implementation]
void CombatScreen::drawActiveBar(sf::RenderWindow &window, sf::Font &font)
{
    const sf::Color COLOR_BLACK = Game::COLOR_BLACK;
    sf::Vector2u windowSize = window.getSize();

    // Active Bar background (even taller and moved down)
    float barY = 520.0f;
    float barHeight = 200.0f;

    // Check if currently dragging a card from hand over the Active Bar
    bool isDraggingOverBar = false;
    if (draggedCard)
    {
        sf::RenderWindow *win = game->getWindow();
        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(*win);
        sf::Vector2f mousePos = win->mapPixelToCoords(mousePixelPos);

        // Use expanded hit area for detection (same as drop detection)
        float detectBarY = 480.0f;
        float detectBarHeight = 240.0f;
        sf::FloatRect activeBarArea({20, detectBarY}, {windowSize.x - 40.0f, detectBarHeight});
        isDraggingOverBar = activeBarArea.contains(mousePos);
    }

    sf::RectangleShape barBg({windowSize.x - 40.0f, barHeight});
    barBg.setPosition({20, barY});

    // Highlight when dragging over
    if (isDraggingOverBar)
    {
        barBg.setFillColor(sf::Color(200, 255, 200, 220));
        barBg.setOutlineColor(sf::Color(0, 255, 0));
        barBg.setOutlineThickness(5.0f);
    }
    else
    {
        barBg.setFillColor(sf::Color(180, 180, 200, 200));
        barBg.setOutlineColor(COLOR_BLACK);
        barBg.setOutlineThickness(3.0f);
    }
    window.draw(barBg);

    // Title
    sf::Text title(font, "ACTIVE BAR (Arrange Cards Here)", 20);
    title.setFillColor(COLOR_BLACK);
    title.setStyle(sf::Text::Bold);
    title.setPosition({30, barY - 25});
    window.draw(title);

    // Draw cards in Active Bar (from HEAD to TAIL)
    float startX = 100.0f;
    float cardY = barY + 10;
    float cardW = 80.0f;
    float cardH = 100.0f;
    float spacing = 90.0f;

    // First, draw empty slots (including special slots)
    int totalSlots = activeBar.getTotalSlotCount();
    int singletonSlot = activeBar.getSingletonSlotIndex();
    int observerSlot = activeBar.getObserverSlotIndex();

    for (int i = 0; i < totalSlots; i++)
    {
        float slotX = startX + (i * spacing);

        // Draw slot background
        sf::RectangleShape slotRect({cardW, cardH});
        slotRect.setPosition({slotX, cardY});

        // Special slot (Singleton) - GREEN
        if (i == singletonSlot)
        {
            slotRect.setFillColor(sf::Color(100, 255, 100, 100));
            slotRect.setOutlineColor(sf::Color(0, 200, 0));
            slotRect.setOutlineThickness(3.0f);
            window.draw(slotRect);

            // Label
            sf::Text label(font, "FREE", 12);
            label.setFillColor(sf::Color(0, 150, 0));
            label.setStyle(sf::Text::Bold);
            label.setPosition({slotX + 10, cardY + 40});
            window.draw(label);
        }
        // Special slot (Observer) - PURPLE
        else if (i == observerSlot)
        {
            slotRect.setFillColor(sf::Color(200, 100, 255, 100));
            slotRect.setOutlineColor(sf::Color(150, 0, 255));
            slotRect.setOutlineThickness(3.0f);
            window.draw(slotRect);

            // Label
            sf::Text label(font, "3x", 12);
            label.setFillColor(sf::Color(150, 0, 255));
            label.setStyle(sf::Text::Bold);
            label.setPosition({slotX + 25, cardY + 40});
            window.draw(label);
        }
        // Normal slot - GRAY
        else
        {
            slotRect.setFillColor(sf::Color(150, 150, 150, 80));
            slotRect.setOutlineColor(sf::Color(100, 100, 100));
            slotRect.setOutlineThickness(1.0f);
            window.draw(slotRect);
        }
    }

    auto drawCardInSlot = [&](Card *card, int slotIndex)
    {
        if (!card)
            return;

        float cardX = startX + (slotIndex * spacing);

        bool patternValid = (activeBar.calculatePatternBonus(card) > 0);

        sf::RectangleShape cardRect({cardW, cardH});
        cardRect.setPosition({cardX, cardY});
        cardRect.setFillColor(sf::Color(240, 240, 240));

        sf::Color borderColor = COLOR_BLACK;
        if (patternValid)
        {
            borderColor = sf::Color(0, 255, 0);
        }
        cardRect.setOutlineColor(borderColor);
        cardRect.setOutlineThickness(patternValid ? 3.0f : 2.0f);
        window.draw(cardRect);

        // Draw icon sprite if available
        sf::Sprite *iconSprite = card->getIconSprite();
        if (iconSprite)
        {
            const sf::Texture &iconTexture = iconSprite->getTexture();
            sf::Vector2u texSize = iconTexture.getSize();

            // Scale icon to fit within card
            float scaleX = (cardW - 10.0f) / texSize.x;
            float scaleY = (cardH - 30.0f) / texSize.y;
            float scale = std::min(scaleX, scaleY);

            iconSprite->setScale({scale, scale});

            // Center icon in card
            float iconWidth = texSize.x * scale;
            float iconHeight = texSize.y * scale;
            float iconX = cardX + (cardW - iconWidth) / 2.0f;
            float iconY = cardY + (cardH - iconHeight - 20.0f) / 2.0f;

            iconSprite->setPosition({iconX, iconY});
            window.draw(*iconSprite);
        }

        // Pattern bonus indicator
        if (patternValid)
        {
            int bonus = activeBar.calculatePatternBonus(card);
            sf::Text bonusText(font, "+" + std::to_string(bonus), 14);
            bonusText.setFillColor(sf::Color(0, 200, 0));
            bonusText.setStyle(sf::Text::Bold);
            bonusText.setPosition({cardX + 5, cardY + 80});
            window.draw(bonusText);
        }
    };

    // Draw cards in normal slots (DLL)
    Card *current = activeBar.getHead();
    int position = 0;
    while (current)
    {
        drawCardInSlot(current, position);
        current = current->next;
        position++;
    }

    // Draw card in singleton slot (if any)
    if (singletonSlot >= 0 && activeBar.getSingletonCard())
    {
        drawCardInSlot(activeBar.getSingletonCard(), singletonSlot);
    }

    // Draw card in observer slot (if any)
    if (observerSlot >= 0 && activeBar.getObserverCard())
    {
        drawCardInSlot(activeBar.getObserverCard(), observerSlot);
    }

    // Buttons in top-right corner of Active Bar
    float btnWidth = 120.0f;
    float btnHeight = 50.0f;
    float btnSpacing = 10.0f;
    float btnX = windowSize.x - 40.0f - btnWidth - 10.0f; // Right side of Active Bar

    // UNDO button (top)
    float undoY = barY + 10.0f;
    sf::RectangleShape undoBtn({btnWidth, btnHeight});
    undoBtn.setPosition({btnX, undoY});
    // Change color on hover
    if (hoveredButton == 0) // Hovering UNDO
    {
        undoBtn.setFillColor(sf::Color(230, 130, 30));
        undoBtn.setOutlineThickness(3.0f);
    }
    else
    {
        undoBtn.setFillColor(sf::Color(200, 100, 0)); // Normal orange
        undoBtn.setOutlineThickness(2.0f);
    }
    undoBtn.setOutlineColor(COLOR_BLACK);
    window.draw(undoBtn);

    sf::Text undoText(font, "UNDO", 20);
    undoText.setFillColor(COLOR_BLACK);
    undoText.setStyle(sf::Text::Bold);
    sf::FloatRect undoBounds = undoText.getLocalBounds();
    undoText.setOrigin({undoBounds.size.x / 2.0f, undoBounds.size.y / 2.0f});
    undoText.setPosition({btnX + btnWidth / 2.0f, undoY + btnHeight / 2.0f});
    window.draw(undoText);

    // CONFIRM button (below UNDO)
    float confirmY = undoY + btnHeight + btnSpacing;
    sf::RectangleShape confirmBtn({btnWidth, btnHeight});
    confirmBtn.setPosition({btnX, confirmY});
    // Change color on hover
    if (hoveredButton == 1) // Hovering CONFIRM
    {
        confirmBtn.setFillColor(sf::Color(30, 230, 30)); // Lighter green
        confirmBtn.setOutlineThickness(3.0f);            // Thicker outline
    }
    else
    {
        confirmBtn.setFillColor(sf::Color(0, 200, 0)); // Normal green
        confirmBtn.setOutlineThickness(2.0f);
    }
    confirmBtn.setOutlineColor(COLOR_BLACK);
    window.draw(confirmBtn);

    sf::Text confirmText(font, "CONFIRM", 20);
    confirmText.setFillColor(COLOR_BLACK);
    confirmText.setStyle(sf::Text::Bold);
    sf::FloatRect confirmBounds = confirmText.getLocalBounds();
    confirmText.setOrigin({confirmBounds.size.x / 2.0f, confirmBounds.size.y / 2.0f});
    confirmText.setPosition({btnX + btnWidth / 2.0f, confirmY + btnHeight / 2.0f});
    window.draw(confirmText);
}

// Add card to Active Bar [Implementation]
void CombatScreen::addCardToActiveBar(int cardIndex, int slotPosition)
{
    if (cardIndex < 0 || cardIndex >= hand.size())
        return;

    Card *card = hand[cardIndex];

    // Check if trying to add to special slots
    bool isAddingToSingleton = (slotPosition == activeBar.getSingletonSlotIndex() && slotPosition >= 0);
    bool isAddingToObserver = (slotPosition == activeBar.getObserverSlotIndex() && slotPosition >= 0);

    if (!isAddingToSingleton)
    {
        // Check if adding this card would exceed available energy
        int currentEnergy = playerRef->getCurrentBaseCost();
        int activeBarCost = activeBar.getTotalEnergyCost(playerRef);
        int newCost = activeBarCost + playerRef->calculateEnergyCost(card->getEnergyCost());

        if (newCost > currentEnergy)
        {
            std::cout << "[ActiveBar] Not enough energy! Need " << newCost << ", have " << currentEnergy << std::endl;
            return;
        }
    }

    bool success = false;
    if (slotPosition >= 0)
    {
        // Add to specific slot (including special slots)
        success = activeBar.addCardToSlot(card, slotPosition);
    }
    else
    {
        // Add to tail of normal slots (default behavior)
        success = activeBar.addCard(card);
    }

    if (!success)
    {
        std::cout << "[ActiveBar] Failed to add card to slot " << slotPosition << std::endl;
        return;
    }

    // Push to undo stack (store position)
    UndoAction action(card, slotPosition >= 0 ? slotPosition : activeBar.getCardCount() - 1);
    undoStack.push(action);

    // Remove from hand
    hand.erase(hand.begin() + cardIndex);

    // Remove from hash table
    if (handHashTable)
    {
        handHashTable->remove(card->getName(), card);
    }

    int currentEnergy = playerRef->getCurrentBaseCost();
    int activeBarCost = activeBar.getTotalEnergyCost(playerRef);

    if (isAddingToSingleton)
    {
        std::cout << "[ActiveBar] Added card: " << card->getName() << " to SINGLETON slot (FREE!)" << std::endl;
        addCombatLog("Added " + card->getName() + " to SINGLETON slot (FREE!)");
    }
    else if (isAddingToObserver)
    {
        std::cout << "[ActiveBar] Added card: " << card->getName() << " to OBSERVER slot (3x!)" << std::endl;
        addCombatLog("Added " + card->getName() + " to OBSERVER slot (3x activation!)");
    }
    else
    {
        std::cout << "[ActiveBar] Added card: " << card->getName() << " (Cost: " << card->getEnergyCost() << ", Total: " << activeBarCost << "/" << currentEnergy << ")" << std::endl;
    }
}

// Remove card from Active Bar [Implementation]
void CombatScreen::removeCardFromActiveBar(Card *card)
{
    if (!card)
        return;

    // Check what type of slot this card is in
    bool wasSingleton = (card == activeBar.getSingletonCard());
    bool wasObserver = (card == activeBar.getObserverCard());

    activeBar.removeCard(card);

    // Add back to hand
    hand.push_back(card);

    // Add back to hash table
    if (handHashTable)
    {
        handHashTable->insert(card->getName(), card);
    }

    if (wasSingleton)
    {
        std::cout << "[ActiveBar] Removed card from SINGLETON slot: " << card->getName() << std::endl;
        addCombatLog("Removed from Singleton: " + card->getName());
    }
    else if (wasObserver)
    {
        std::cout << "[ActiveBar] Removed card from OBSERVER slot: " << card->getName() << std::endl;
        addCombatLog("Removed from Observer: " + card->getName());
    }
    else
    {
        std::cout << "[ActiveBar] Removed card: " << card->getName() << std::endl;
    }
}

// Execute cards in Active Bar [Implementation]
void CombatScreen::executeActiveBar()
{
    // Check if there are any cards to execute (including special slots)
    bool hasCards = (activeBar.getCardCount() > 0 ||
                     activeBar.getSingletonCard() != nullptr ||
                     activeBar.getObserverCard() != nullptr);

    if (!hasCards)
    {
        std::cout << "[ActiveBar] No cards to execute!" << std::endl;
        return;
    }

    // Calculate total energy cost
    int totalCost = activeBar.getTotalEnergyCost(playerRef);
    int currentEnergy = playerRef->getCurrentBaseCost();

    // Check if enough energy (double-check)
    if (totalCost > currentEnergy)
    {
        std::cout << "[ActiveBar] ERROR: Not enough energy! Need " << totalCost << ", have " << currentEnergy << std::endl;
        return;
    }

    // Spend energy NOW (on confirm)
    playerRef->modifyBaseCost(-totalCost);
    std::cout << "[ActiveBar] Spent " << totalCost << " energy. Remaining: " << playerRef->getCurrentBaseCost() << std::endl;
    addCombatLog("Spent " + std::to_string(totalCost) + " energy (Remaining: " + std::to_string(playerRef->getCurrentBaseCost()) + ")");

    // Find first alive enemy
    Enemy *target = nullptr;
    for (Enemy *e : enemies)
    {
        if (e->getHP() > 0)
        {
            target = e;
            break;
        }
    }

    if (!target)
    {
        std::cout << "[ActiveBar] No valid target!" << std::endl;
        return;
    }

    std::cout << "\n[ActiveBar] ========== EXECUTING ACTIVE BAR ==========" << std::endl;
    std::cout << "[ActiveBar] Target: " << target->getName() << " (HP: " << target->getHP() << ")" << std::endl;

    // Helper lambda to execute a card
    auto executeCard = [&](Card *card, int pos, const std::string &slotLabel = "")
    {
        if (!card)
            return;

        std::cout << "[ActiveBar] Card " << pos << slotLabel << ": " << card->getName() << std::endl;

        // Calculate pattern bonus
        int patternBonus = activeBar.calculatePatternBonus(card);
        std::cout << "[ActiveBar] Pattern Bonus: +" << patternBonus << std::endl;

        // Get role-specific bonus based on dataType
        int roleBonus = 0;
        std::string roleName = playerRef->getRoleName();
        Card::CardDataType dataType = card->getDataType();

        if (roleName == "FRONTEND DEV")
        {
            // Frontend loves Trees and Nodes (visual structures)
            if (dataType == Card::CardDataType::TREE)
                roleBonus = 3;
            else if (dataType == Card::CardDataType::NODE)
                roleBonus = 2;
        }
        else if (roleName == "BACKEND DEV")
        {
            // Backend loves Hash Tables and DLLs (data structures)
            if (dataType == Card::CardDataType::HASH_TABLE)
                roleBonus = 3;
            else if (dataType == Card::CardDataType::DLL)
                roleBonus = 2;
        }
        else if (roleName == "DATA SCIENTIST")
        {
            // Data Scientist loves Trees (decision trees) and Nodes (neural networks)
            if (dataType == Card::CardDataType::TREE)
                roleBonus = 3;
            else if (dataType == Card::CardDataType::NODE)
                roleBonus = 3;
        }
        else if (roleName == "DEVOPS ENGINEER")
        {
            // DevOps loves Queues and Stacks (pipeline structures)
            if (dataType == Card::CardDataType::QUEUE)
                roleBonus = 3;
            else if (dataType == Card::CardDataType::STACK)
                roleBonus = 2;
        }
        else if (roleName == "FULL STACK DEV")
        {
            // Full Stack gets small bonus for everything
            roleBonus = 1;
        }

        std::cout << "[ActiveBar] Role Bonus (" << roleName << "): +" << roleBonus << std::endl;

        int totalBonus = patternBonus + roleBonus;
        std::cout << "[ActiveBar] Total Bonus: +" << totalBonus << std::endl;

        if (totalBonus > 0)
        {
            addCombatLog("Bonus: +" + std::to_string(totalBonus) + " damage!");
        }

        // Apply bonuses through Entity's strength system
        int originalStrength = playerRef->getStrength();
        playerRef->setStrength(originalStrength + totalBonus);

        std::cout << "[ActiveBar] Strength: " << originalStrength << " -> " << playerRef->getStrength() << std::endl;

        addCombatLog("Playing: " + card->getName() + slotLabel);

        int targetHPBefore = target ? target->getHP() : 0;
        int playerHPBefore = playerRef->getHP();
        int playerBlockBefore = playerRef->getBlock();
        int playerStrengthBefore = playerRef->getStrength();
        int playerDexterityBefore = playerRef->getDexterity();

        card->play(playerRef, target);

        int targetHPAfter = target ? target->getHP() : 0;
        int playerHPAfter = playerRef->getHP();
        int playerBlockAfter = playerRef->getBlock();
        int playerStrengthAfter = playerRef->getStrength();
        int playerDexterityAfter = playerRef->getDexterity();

        if (card->getType() == Card::CardType::ATTACK)
        {
            int damageDealt = targetHPBefore - targetHPAfter;
            if (damageDealt > 0)
            {
                addCombatLog("Dealt " + std::to_string(damageDealt) + " damage to " + target->getName());
                Logger::getInstance()->logDamageDealt(card->getName(), damageDealt, target->getName());
            }
        }
        else if (card->getType() == Card::CardType::DEFENSE)
        {
            int blockGained = playerBlockAfter - playerBlockBefore;
            if (blockGained > 0)
            {
                addCombatLog("Gained " + std::to_string(blockGained) + " block (Total: " + std::to_string(playerBlockAfter) + ")");
                Logger::getInstance()->logBlockGained(card->getName(), blockGained);
            }
        }
        else if (card->getType() == Card::CardType::SKILL)
        {
            int damageDealt = targetHPBefore - targetHPAfter;
            int blockGained = playerBlockAfter - playerBlockBefore;
            int healGained = playerHPAfter - playerHPBefore;

            if (damageDealt > 0)
            {
                addCombatLog("Dealt " + std::to_string(damageDealt) + " damage to " + target->getName());
                Logger::getInstance()->logDamageDealt(card->getName(), damageDealt, target->getName());
            }
            if (blockGained > 0)
            {
                addCombatLog("Gained " + std::to_string(blockGained) + " block (Total: " + std::to_string(playerBlockAfter) + ")");
                Logger::getInstance()->logBlockGained(card->getName(), blockGained);
            }
            if (healGained > 0)
            {
                addCombatLog("Healed " + std::to_string(healGained) + " HP (HP: " + std::to_string(playerHPAfter) + "/" + std::to_string(playerRef->getMaxHP()) + ")");
                Logger::getInstance()->logHealGained(card->getName(), healGained, playerHPAfter, playerRef->getMaxHP());
            }
        }
        else if (card->getType() == Card::CardType::POWER)
        {
            int strengthGained = playerStrengthAfter - playerStrengthBefore;
            int dexterityGained = playerDexterityAfter - playerDexterityBefore;

            if (strengthGained > 0)
            {
                addCombatLog("Gained +" + std::to_string(strengthGained) + " Strength (Total: " + std::to_string(playerStrengthAfter) + ")");
                Logger::getInstance()->logStrengthGained(card->getName(), strengthGained, playerStrengthAfter);
            }
            if (dexterityGained > 0)
            {
                addCombatLog("Gained +" + std::to_string(dexterityGained) + " Dexterity (Total: " + std::to_string(playerDexterityAfter) + ")");
                Logger::getInstance()->logDexterityGained(card->getName(), dexterityGained, playerDexterityAfter);
            }
        }

        playerRef->setStrength(originalStrength);

        std::cout << "[ActiveBar] Executed " << card->getName() << " (Bonus: +" << totalBonus << ")" << std::endl;

        // Check if target died, switch to next enemy
        if (target && target->getHP() <= 0)
        {
            std::cout << "[ActiveBar] Target defeated! Switching to next enemy..." << std::endl;
            addCombatLog(target->getName() + " defeated!");
            target = nullptr;
            for (Enemy *e : enemies)
            {
                if (e->getHP() > 0)
                {
                    target = e;
                    std::cout << "[ActiveBar] New target: " << target->getName() << std::endl;
                    break;
                }
            }
            if (!target)
            {
                std::cout << "[ActiveBar] All enemies defeated!" << std::endl;
            }
        }
    };

    // Execute normal slot cards (DLL order)
    Card *current = activeBar.getHead();
    int position = 0;

    while (current && target)
    {
        executeCard(current, position);
        Card *next = current->next;
        current = next;
        position++;
    }

    // Execute singleton slot card (if any) - FREE!
    if (activeBar.getSingletonCard() && target)
    {
        addCombatLog("[SINGLETON SLOT - FREE ENERGY!]");
        executeCard(activeBar.getSingletonCard(), -1, " [SINGLETON]");
    }

    // Execute observer slot card (if any) - 3x activation!
    if (activeBar.getObserverCard() && target)
    {
        addCombatLog("[OBSERVER SLOT - 3x ACTIVATION!]");
        for (int i = 0; i < 3 && target; i++)
        {
            executeCard(activeBar.getObserverCard(), -1, " [OBSERVER x" + std::to_string(i + 1) + "]");
        }
    }

    std::cout << "[ActiveBar] ========== EXECUTION COMPLETE ==========" << std::endl;

    // Move normal slot cards to discard pile
    current = activeBar.getHead();
    while (current)
    {
        Card *next = current->next;
        discardPile.push_back(current);
        current = next;
    }

    // OBSERVER SLOT: Move to discard pile (card is consumed)
    if (activeBar.getObserverCard())
    {
        discardPile.push_back(activeBar.getObserverCard());
    }

    // SINGLETON SLOT: Keep the card (it stays for next turn!)
    // Do NOT move to discard pile
    if (activeBar.getSingletonCard())
    {
        addCombatLog("Singleton card remains: " + activeBar.getSingletonCard()->getName());
    }

    // Clear Active Bar for next turn (keeps singleton, clears observer + normal)
    activeBar.clearForNextTurn();

    undoStack.clear();

    // End player's turn after executing (this will check combat end during turn processing)
    endPlayerTurn();
}

// Undo last added card in Active Bar [Implementation]
void CombatScreen::undoLastCard()
{
    if (undoStack.isEmpty())
    {
        std::cout << "[ActiveBar] Nothing to undo!" << std::endl;
        return;
    }

    UndoAction action = undoStack.pop();
    removeCardFromActiveBar(action.card);

    std::cout << "[ActiveBar] Undone: " << action.card->getName() << std::endl;
}

// Draw Legend/Instruction Panel [Implementation]
void CombatScreen::drawLegendPanel(sf::RenderWindow &window, sf::Font &font)
{
    sf::Vector2u windowSize = window.getSize();

    float panelX = windowSize.x - LEGEND_PANEL_WIDTH + legendPanelOffsetX;
    float panelY = 0;
    float panelHeight = static_cast<float>(windowSize.y);

    sf::RectangleShape panel({LEGEND_PANEL_WIDTH, panelHeight});
    panel.setPosition({panelX, panelY});
    panel.setFillColor(sf::Color(40, 40, 50, 230));
    panel.setOutlineColor(sf::Color(255, 215, 0)); // Gold
    panel.setOutlineThickness(3.0f);
    window.draw(panel);

    // Arrow button (on left edge of panel)
    float arrowX = panelX - 30;
    float arrowY = windowSize.y / 2.0f - 40;
    sf::RectangleShape arrowButton({30, 80});
    arrowButton.setPosition({arrowX, arrowY});
    arrowButton.setFillColor(sf::Color(255, 215, 0)); // Gold
    window.draw(arrowButton);

    // Arrow text (REVERSED: open=>, closed=<)
    sf::Text arrowText(font, legendPanelOpen ? ">" : "<", 30);
    arrowText.setFillColor(sf::Color::Black);
    arrowText.setPosition({arrowX + 5, arrowY + 20});
    window.draw(arrowText);

    // Title
    sf::Text title(font, "INSTRUCTION", 32);
    title.setFillColor(sf::Color(255, 215, 0));
    title.setStyle(sf::Text::Bold);
    title.setPosition({panelX + 20, 20});
    window.draw(title);

    float yPos = 80;

    // Color meanings
    sf::Text colorTitle(font, "CARD BORDERS:", 20);
    colorTitle.setFillColor(sf::Color::White);
    colorTitle.setStyle(sf::Text::Bold);
    colorTitle.setPosition({panelX + 20, yPos});
    window.draw(colorTitle);
    yPos += 35;

    // Yellow - Attack
    sf::RectangleShape yellowBox({20, 20});
    yellowBox.setPosition({panelX + 20, yPos});
    yellowBox.setFillColor(sf::Color(255, 255, 0));
    window.draw(yellowBox);

    sf::Text yellowText(font, "Attack Card", 18);
    yellowText.setFillColor(sf::Color::White);
    yellowText.setPosition({panelX + 50, yPos});
    window.draw(yellowText);
    yPos += 30;

    // Blue - Defense
    sf::RectangleShape blueBox({20, 20});
    blueBox.setPosition({panelX + 20, yPos});
    blueBox.setFillColor(sf::Color(0, 150, 255));
    window.draw(blueBox);

    sf::Text blueText(font, "Defense Card", 18);
    blueText.setFillColor(sf::Color::White);
    blueText.setPosition({panelX + 50, yPos});
    window.draw(blueText);
    yPos += 30;

    // Green - Skill
    sf::RectangleShape greenBox({20, 20});
    greenBox.setPosition({panelX + 20, yPos});
    greenBox.setFillColor(sf::Color(0, 255, 100));
    window.draw(greenBox);

    sf::Text greenText(font, "Skill Card", 18);
    greenText.setFillColor(sf::Color::White);
    greenText.setPosition({panelX + 50, yPos});
    window.draw(greenText);
    yPos += 30;

    // Purple - Power
    sf::RectangleShape purpleBox({20, 20});
    purpleBox.setPosition({panelX + 20, yPos});
    purpleBox.setFillColor(sf::Color(255, 0, 255));
    window.draw(purpleBox);

    sf::Text purpleText(font, "Power Card", 18);
    purpleText.setFillColor(sf::Color::White);
    purpleText.setPosition({panelX + 50, yPos});
    window.draw(purpleText);
    yPos += 50;

    // Glow meanings
    sf::Text glowTitle(font, "CARD GLOW:", 20);
    glowTitle.setFillColor(sf::Color::White);
    glowTitle.setStyle(sf::Text::Bold);
    glowTitle.setPosition({panelX + 20, yPos});
    window.draw(glowTitle);
    yPos += 35;

    // Green glow
    sf::CircleShape greenGlow(10);
    greenGlow.setPosition({panelX + 20, yPos});
    greenGlow.setFillColor(sf::Color(0, 255, 0));
    window.draw(greenGlow);

    sf::Text greenGlowText(font, "Can play with\nbonus", 16);
    greenGlowText.setFillColor(sf::Color::White);
    greenGlowText.setPosition({panelX + 50, yPos - 5});
    window.draw(greenGlowText);
    yPos += 60;

    // Pattern bonuses
    sf::Text patternTitle(font, "PATTERN BONUSES:", 20);
    patternTitle.setFillColor(sf::Color::White);
    patternTitle.setStyle(sf::Text::Bold);
    patternTitle.setPosition({panelX + 20, yPos});
    window.draw(patternTitle);
    yPos += 35;

    std::vector<std::string> patterns = {
        "STACK: MUST play\nnewest first (+3)",
        "QUEUE: MUST play\noldest first (+2)",
        "TREE: Parent BEFORE\nchildren (+3)",
        "HASH: Group SAME\ntype together (+5)",
        "DLL: Forward/Back\norder OK (+2)",
        "SLL/NODE: Left to\nright only (+2)"};

    for (const auto &pattern : patterns)
    {
        sf::Text patternText(font, pattern, 14);
        patternText.setFillColor(sf::Color(200, 200, 200));
        patternText.setPosition({panelX + 20, yPos});
        window.draw(patternText);
        yPos += 45;
    }
}

// Draw Card Detail Popup [Implementation]
void CombatScreen::drawCardDetailPopup(sf::RenderWindow &window, sf::Font &font)
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

        // Reset color
        iconColor.a = 255;
        iconSprite->setColor(iconColor);
    }

    float yPos = popupY + 310;

    sf::Text costText(font, "Cost: " + std::to_string(playerRef->calculateEnergyCost(selectedCardForDetail->getEnergyCost())) + " Energy", 24);
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
        rarityColor = sf::Color(120, 120, 120); // Gray
        break;
    case Card::CardRarity::UNCOMMON:
        rarityStr = "Uncommon";
        rarityColor = sf::Color(0, 180, 0); // Green
        break;
    case Card::CardRarity::RARE:
        rarityStr = "Rare";
        rarityColor = sf::Color(0, 100, 255); // Blue
        break;
    case Card::CardRarity::EPIC:
        rarityStr = "Epic";
        rarityColor = sf::Color(160, 0, 200); // Purple
        break;
    case Card::CardRarity::LEGENDARY:
        rarityStr = "LEGENDARY";
        rarityColor = sf::Color(255, 165, 0); // Orange/Gold
        break;
    }

    sf::Text rarityText(font, "Rarity: " + rarityStr, 24);
    rarityColor.a = static_cast<std::uint8_t>(cardDetailAlpha);
    rarityText.setFillColor(rarityColor);
    rarityText.setStyle(sf::Text::Bold);
    rarityText.setPosition({popupX + 50, yPos});
    window.draw(rarityText);
    yPos += 40;

    // Data Structure
    std::string dataTypeStr = "";
    switch (selectedCardForDetail->getDataType())
    {
    case Card::CardDataType::STACK:
        dataTypeStr = "Stack";
        break;
    case Card::CardDataType::QUEUE:
        dataTypeStr = "Queue";
        break;
    case Card::CardDataType::TREE:
        dataTypeStr = "Tree";
        break;
    case Card::CardDataType::HASH_TABLE:
        dataTypeStr = "Hash Table";
        break;
    case Card::CardDataType::DLL:
        dataTypeStr = "Doubly Linked List";
        break;
    case Card::CardDataType::NODE:
    case Card::CardDataType::SLL:
        dataTypeStr = "Singly Linked List";
        break;
    }

    sf::Text dataText(font, "Structure: " + dataTypeStr, 24);
    dataText.setFillColor(sf::Color(0, 0, 0, static_cast<std::uint8_t>(cardDetailAlpha)));
    dataText.setPosition({popupX + 50, yPos});
    window.draw(dataText);
    yPos += 60;

    // Description
    sf::Text descTitle(font, "Description:", 20);
    descTitle.setFillColor(sf::Color(80, 80, 80, static_cast<std::uint8_t>(cardDetailAlpha)));
    descTitle.setStyle(sf::Text::Bold);
    descTitle.setPosition({popupX + 50, yPos});
    window.draw(descTitle);
    yPos += 35;

    sf::Text descText(font, selectedCardForDetail->getDescription(), 18);
    descText.setFillColor(sf::Color(60, 60, 60, static_cast<std::uint8_t>(cardDetailAlpha)));
    descText.setPosition({popupX + 50, yPos});
    window.draw(descText);
    yPos += 80;

    // Close instruction
    sf::Text closeText(font, "Click anywhere to close", 16);
    closeText.setFillColor(sf::Color(120, 120, 120, static_cast<std::uint8_t>(cardDetailAlpha)));
    sf::FloatRect closeBounds = closeText.getLocalBounds();
    closeText.setOrigin({closeBounds.size.x / 2.0f, 0});
    closeText.setPosition({popupX + popupWidth / 2.0f, popupY + popupHeight - 40});
    window.draw(closeText);
}

// Build enemy hash table for quick lookup [Implementation]
void CombatScreen::buildEnemyHashTable()
{
    enemyHashTable.clear();

    // Hash each enemy by their name (type)
    for (Enemy *enemy : enemies)
    {
        if (enemy && enemy->isAlive())
        {
            std::string enemyType = enemy->getName();
            enemyHashTable[enemyType].push_back(enemy);
        }
    }

    std::cout << "[HashTable] Built hash table with " << enemyHashTable.size()
              << " unique enemy types" << std::endl;
}

// Retrieve enemies by name using hash table [Implementation]
std::vector<Enemy *> CombatScreen::getEnemiesByName(const std::string &name)
{
    // Hash table lookup - O(1) average case
    if (enemyHashTable.find(name) != enemyHashTable.end())
    {
        return enemyHashTable[name];
    }

    return std::vector<Enemy *>(); // Empty vector if not found
}

// Debug function to print hash table contents [Implementation]
void CombatScreen::printHashTableDebug() const
{
    std::cout << "[HashTable] Enemy Hash Table Contents:" << std::endl;
    std::cout << "========================================" << std::endl;

    for (const auto &pair : enemyHashTable)
    {
        std::cout << "  Hash Key: \"" << pair.first << "\" -> "
                  << pair.second.size() << " enemies" << std::endl;

        for (const Enemy *enemy : pair.second)
        {
            std::cout << "    - " << enemy->getName()
                      << " (HP: " << enemy->getHP() << ")" << std::endl;
        }
    }

    std::cout << "========================================" << std::endl;
}

// Check for special card combos in hand [Implementation]
void CombatScreen::checkForCombos()
{
    if (!handHashTable)
        return;

    // COMBO 1: "Stack Overflow" - 2+ Stack cards
    int stackCount = handHashTable->countByName("Stack");
    if (stackCount >= 2)
    {
        addCombatLog("[COMBO!] Stack Overflow: 2x Stack cards! +5 damage this turn!");
        std::cout << "[COMBO] Stack Overflow triggered! " << stackCount << " Stack cards detected.\n";
    }

    // COMBO 2: "Linked List" - Node + Debug card
    bool hasNode = handHashTable->contains("Node");
    bool hasDebug = handHashTable->contains("Debug");
    if (hasNode && hasDebug)
    {
        addCombatLog("[COMBO!] Linked List: Node + Debug! Draw +1 card!");
        std::cout << "[COMBO] Linked List triggered!\n";
    }

    // COMBO 3: "Hash Attack" - Multiple Hash cards
    int hashCount = handHashTable->countByName("Hash Attack");
    if (hashCount >= 2)
    {
        addCombatLog("[COMBO!] Hash Collision: " + std::to_string(hashCount) + "x Hash! Double damage!");
        std::cout << "[COMBO] Hash Collision! " << hashCount << " Hash Attack cards.\n";
    }

    // COMBO 4: "Queue System" - 3+ Queue cards
    int queueCount = handHashTable->countByName("Queue");
    if (queueCount >= 3)
    {
        addCombatLog("[COMBO!] Queue System: 3x Queue! Energy cost -1!");
        std::cout << "[COMBO] Queue System activated!\n";
    }

    // COMBO 5: "Tree Traversal" - Tree + Binary Search
    bool hasTree = handHashTable->contains("Tree");
    bool hasBinarySearch = handHashTable->contains("Binary Search");
    if (hasTree && hasBinarySearch)
    {
        addCombatLog("[COMBO!] Tree Traversal: Tree + Binary Search! +10 block!");
        std::cout << "[COMBO] Tree Traversal combo!\n";
    }
}

// Add message to combat log [Implementation]
void CombatScreen::addCombatLog(const std::string &message)
{
    combatLog->addMessage(message);
}

// Draw combat log panel [Implementation]
void CombatScreen::drawCombatLog(sf::RenderWindow &window, sf::Font &font)
{
    if (combatLog->getSize() == 0)
        return;

    sf::Vector2u windowSize = window.getSize();

    // Log panel (bottom-left corner)
    float logWidth = 450.0f;
    float logHeight = 280.0f; // Bigger height
    float logX = 10.0f;
    float logY = windowSize.y - logHeight - 10.0f;

    // Background
    sf::RectangleShape logPanel({logWidth, logHeight});
    logPanel.setPosition({logX, logY});
    logPanel.setFillColor(sf::Color(0, 0, 0, 180));
    logPanel.setOutlineColor(sf::Color(200, 200, 200));
    logPanel.setOutlineThickness(2.0f);
    window.draw(logPanel);

    // Title
    sf::Text titleText(font, "COMBAT LOG", 18);
    titleText.setFillColor(sf::Color::Yellow);
    titleText.setStyle(sf::Text::Bold);
    titleText.setPosition({logX + 8, logY + 5});
    window.draw(titleText);

    // Scroll hint (if there are more messages)
    int totalMessages = combatLog->getSize();
    if (totalMessages > maxLogEntries)
    {
        sf::Text scrollHint(font, "[Scroll: Mouse Wheel]", 11);
        scrollHint.setFillColor(sf::Color(150, 150, 150));
        scrollHint.setPosition({logX + logWidth - 160, logY + 7});
        window.draw(scrollHint);
    }

    // Calculate skip count with scroll offset
    int skipCount = std::max(0, std::min(logScrollOffset, totalMessages - maxLogEntries));

    CombatLogNode *current = combatLog->getHead();
    int index = 0;

    // Skip to the scroll position
    while (current && index < skipCount)
    {
        current = current->next;
        index++;
    }

    // Draw the visible messages - Bigger text
    float yPos = logY + 30;
    int drawn = 0;
    while (current && drawn < maxLogEntries)
    {
        sf::Text logText(font, current->message, 13); // Bigger font size
        logText.setFillColor(sf::Color::White);
        logText.setPosition({logX + 10, yPos});
        window.draw(logText);
        yPos += 18; // More spacing for bigger text
        current = current->next;
        drawn++;
    }
}
