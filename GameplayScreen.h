#pragma once

#include "Screen.h"
#include "Map.h"
#include "Player.h"
#include <SFML/Graphics.hpp>
#include <map>

class GameplayScreen : public Screen
{
private:
    Map *gameMap;
    Player *playerRef;
    bool mousePressed;
    int hoveredElement;
    Room *hoveredRoom;
    bool viewingDeck;
    float deckScrollOffset;
    bool showCardDetail;
    Card *selectedCardForDetail;
    float cardDetailAlpha;
    const float FADE_SPEED = 800.0f;
    bool showStatsDetail;
    float statsDetailAlpha;
    bool showScenarioDetail;
    float scenarioDetailAlpha;
    sf::Texture deckIconTexture;
    sf::Sprite *deckIconSprite;
    sf::Texture redeemIconTexture;
    sf::Sprite *redeemIconSprite;
    bool showRedeemOverlay;
    std::string redeemCodeInput;
    float redeemOverlayAlpha;
    bool redeemInputActive;
    bool showRedeemResult;
    std::string redeemResultMessage;
    bool redeemSuccess;
    float redeemResultAlpha;
    bool showBuffDetail;
    float buffDetailAlpha;
    float buffScrollOffset;
    std::string selectedBuffName;
    std::map<std::string, sf::Texture> buffTextures;
    sf::View gameView;
    sf::Vector2f cameraOffset;
    int currentFrame;
    float animationTimer;
    float frameTime;
    bool wasGenerating;

public:
    // Constructor [Header]
    GameplayScreen(Game *gameInstance);
    // Destructor [Header]
    ~GameplayScreen() override;
    // Handle events, update, render [Header]
    void handleEvents() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window, sf::Font &font) override;
    // Navigate to this screen [Header]
    void onEnter() override;
    // Exit from this screen [Header]
    void onExit() override;

private:
    // UI Drawing Helpers
    void drawSettingsButton(sf::RenderWindow &window);
    void drawViewDeckButton(sf::RenderWindow &window, sf::Font &font);
    void drawRedeemCodeButton(sf::RenderWindow &window, sf::Font &font);
    void drawDeckOverlay(sf::RenderWindow &window, sf::Font &font);
    void drawPlayerStats(sf::RenderWindow &window, sf::Font &font);
    void drawCharacterSprite(sf::RenderWindow &window, sf::Font &font);
    void drawRoomInfo(sf::RenderWindow &window, sf::Font &font, Room *room);
    void drawGearIcon(sf::RenderWindow &window, float x, float y, float radius, sf::Color color);
    void drawCardDetailPopup(sf::RenderWindow &window, sf::Font &font);
    void drawStatsDetailPopup(sf::RenderWindow &window, sf::Font &font);
    void drawScenarioDetailPopup(sf::RenderWindow &window, sf::Font &font);
    void drawRedeemOverlay(sf::RenderWindow &window, sf::Font &font);
    void drawRedeemResultPopup(sf::RenderWindow &window, sf::Font &font);
    void drawBuffDetailPopup(sf::RenderWindow &window, sf::Font &font);
    void processRedeemCode(const std::string &code);
    void loadBuffTextures();
};
