#pragma once

#include "Screen.h"
#include "Card.h"
#include <vector>
#include <string>

// Forward declarations
class Game;
class Player;

class ShopScreen : public Screen
{
private:
    // Buff shop item structure
    struct BuffItem
    {
        std::string name;
        std::string description;
        int price;
        bool *buffPtr; // Pointer to the buff in player's RedeemBuffs
        bool purchased;
        std::string iconPath; // Path to buff icon image

        BuffItem(const std::string &n, const std::string &desc, int p, bool *ptr, const std::string &icon)
            : name(n), description(desc), price(p), buffPtr(ptr), purchased(false), iconPath(icon) {}
    };

    Game *game;
    Player *playerRef;
    std::vector<Card *> shopCards;
    std::vector<int> cardPrices;
    std::vector<BuffItem> buffItems;
    std::unordered_map<std::string, sf::Texture> buffTextures; // Store buff icon textures
    int hoveredCardIndex;
    int hoveredBuffIndex;
    bool hoveredLeave;
    bool hoveredGacha;
    bool showCardDetail;
    Card *selectedCardForDetail;
    bool showBuffDetail;
    int selectedBuffIndex;
    float cardDetailAlpha;
    float buffDetailAlpha;
    bool hoveredConfirmButton;
    bool hoveredBuffConfirmButton;
    float fadeAlpha;
    int gachaPrice;
    bool showGachaResult;
    Card *gachaResultCard;
    float gachaResultTimer;

public:
    ShopScreen(Game *g);
    ~ShopScreen();
    void handleEvents() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow &window, sf::Font &font) override;
    void onEnter() override;
    void onExit() override;
    void generateShop();
    void buyCard(int cardIndex);
    void buyBuff(int buffIndex);
    void performGacha();
    int calculateCardPrice(Card *card);
    void drawCardDetailPopup(sf::RenderWindow &window, sf::Font &font);
    void drawBuffDetailPopup(sf::RenderWindow &window, sf::Font &font);
    void drawGachaResultPopup(sf::RenderWindow &window, sf::Font &font);
    void initializeBuffShop();
    void loadBuffTextures();
};
