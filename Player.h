#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>
#include "Entity.h"
#include "Card.h"

class Player : public Entity
{
public:
  struct RoleBonuses
  {
    float domTreeSpeed;
    float hashMapLookup;
    float stackOverflow;
    float pipelineSpeed;
    float priorityQueue;

    RoleBonuses() : domTreeSpeed(0.0f), hashMapLookup(0.0f), stackOverflow(0.0f), pipelineSpeed(0.0f), priorityQueue(0.0f) {}
  };

  struct ScenarioModifiers
  {
    float cardCostModifier;
    float speedModifier;
    int maxEnergyBonus;
    float damageModifier;
    float rewardModifier;

    ScenarioModifiers() : cardCostModifier(0.0f), speedModifier(0.0f), maxEnergyBonus(0), damageModifier(0.0f), rewardModifier(0.0f) {}
  };

  struct RedeemBuffs
  {
    bool onePunchMan;
    bool iterator;
    bool singleton;
    bool factory;
    bool adapter;
    bool observer;

    RedeemBuffs() : onePunchMan(false), iterator(false), singleton(false), factory(false), adapter(false), observer(false) {}
  };

private:
  std::string roleName;
  std::string scenarioDescription;
  int maxBaseCost;
  int currentBaseCost;
  int gold;
  RoleBonuses roleBonuses;
  ScenarioModifiers scenarioModifiers;
  RedeemBuffs redeemBuffs;
  sf::Texture spriteTexture;
  std::unique_ptr<sf::Sprite> sprite;
  bool spriteLoaded;
  int currentFrame;
  sf::IntRect frameRects[4];
  bool hasFrames;
  std::vector<Card *> masterDeck;
  int bonusDrawNextTurn;
  bool isBossRoom;

public:
  Player();
  Player(const std::string &playerName, const std::string &role, int health, int energy);
  ~Player() override;
  void update(float deltaTime) override;
  void render(sf::RenderWindow &window) override;
  void takeTurn() override;
  void setRoleName(const std::string &role);
  std::string getRoleName() const;
  std::string getScenarioDescription() const;
  int getMaxBaseCost() const;
  int getCurrentBaseCost() const;
  const RoleBonuses &getRoleBonuses() const;
  const ScenarioModifiers &getScenarioModifiers() const;
  RedeemBuffs &getRedeemBuffs();
  const RedeemBuffs &getRedeemBuffs() const;
  void setName(const std::string &playerName);
  void setScenarioDescription(const std::string &desc);
  void setMaxHealth(int health);
  void setMaxBaseCost(int cost);
  void modifyBaseCost(int amount);
  void refreshBaseCost();
  int getGold() const;
  void addGold(int amount);
  void spendGold(int amount);
  void setGold(int amount);
  std::vector<Card *> &getMasterDeck();
  const std::vector<Card *> &getMasterDeck() const;
  int getCurrentFrame() const;
  int getDeckSize() const;
  void setRoleBonuses(const std::string &role);
  void setScenarioModifiers(float cardCost, float speed, int maxEnergyBonus, float damage, float reward);
  bool loadSprite(const std::string &texturePath);
  void setSprite(const sf::Texture &texture);
  void setCurrentFrame(int frame);
  void setFrameRect(int frameIndex, int x, int y, int width, int height);
  void reset();
  float getTotalCardCostModifier() const;
  float getTotalSpeedModifier() const;
  std::string getSummary() const;
  int calculateDamage(int baseDamage) const;
  int calculateBlock(int baseBlock) const;
  int calculateCardDraw(int baseDraw) const;
  int calculateEnergyCost(int baseCost) const;
  void initializeStarterDeck();
  void addCardToDeck(Card *card);
  void removeCardFromDeck(Card *card);
  void clearDeck();
  void addBonusDrawNextTurn(int amount);
  int getBonusDrawNextTurn() const;
  void resetBonusDrawNextTurn();
  void setIsBossRoom(bool value);
  bool getIsBossRoom() const;
};