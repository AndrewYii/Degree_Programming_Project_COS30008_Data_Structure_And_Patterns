#pragma once

#include <string>
#include <fstream>
#include <ctime>
#include <sstream>
#include <iomanip>

class Logger
{
private:
    static Logger *instance;
    std::ofstream combatLog;
    std::ofstream aiLog;
    bool initialized;
    std::string sessionID;

    Logger();
    Logger(const Logger &) = delete;
    Logger &operator=(const Logger &) = delete;

public:
    static Logger *getInstance();
    static void destroy();
    ~Logger();

    void initialize(const std::string &logDirectory = "logs");

    void logCombatStart(const std::string &enemyName, int enemyHP, int enemyDamage);
    void logCombatTurn(int turnNumber);
    void logCardPlayed(const std::string &cardName, const std::string &cardType, int energyCost);
    void logDamageDealt(const std::string &source, int damage, const std::string &target, bool wasCrit = false);
    void logBlockGained(const std::string &source, int blockAmount);
    void logHealGained(const std::string &source, int healAmount, int currentHP, int maxHP);
    void logStrengthGained(const std::string &source, int strengthAmount, int totalStrength);
    void logDexterityGained(const std::string &source, int dexterityAmount, int totalDexterity);
    void logDebuffApplied(const std::string &debuffName, int stacks, const std::string &target);
    void logEnemyBehavior(const std::string &enemyName, const std::string &behavior, int value = 0);
    void logBonusDamage(const std::string &source, int bonusDamage, const std::string &pattern);
    void logCombatEnd(bool victory, int playerHP, int turnsElapsed);
    void logError(const std::string &message);

    void logAIDecision(const std::string &decision, const std::string &reason);
    void logAITensionChange(const std::string &oldTension, const std::string &newTension, float difficultyScore);
    void logAIEventTrigger(const std::string &eventName, const std::string &condition);
    void logAIInfluence(const std::string &enemyName, int adjustedHP, int adjustedDamage, float multiplier);

    void flush();

private:
    std::string getTimestamp();
    std::string generateSessionID();
    void writeToCombatLog(const std::string &message);
    void writeToAILog(const std::string &message);
};
