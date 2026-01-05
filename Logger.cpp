#include "Logger.h"
#include <iostream>
#include <direct.h>

Logger *Logger::instance = nullptr;

Logger::Logger() : initialized(false), sessionID("") {}

Logger::~Logger()
{
    if (combatLog.is_open())
        combatLog.close();
    if (aiLog.is_open())
        aiLog.close();
}

Logger *Logger::getInstance()
{
    if (instance == nullptr)
    {
        instance = new Logger();
    }
    return instance;
}

void Logger::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

void Logger::initialize(const std::string &logDirectory)
{
    if (initialized)
        return;

    _mkdir(logDirectory.c_str());
    sessionID = generateSessionID();

    std::string combatFile = logDirectory + "/combat_" + sessionID + ".txt";
    std::string aiFile = logDirectory + "/AI_" + sessionID + ".txt";

    combatLog.open(combatFile, std::ios::out | std::ios::app);
    aiLog.open(aiFile, std::ios::out | std::ios::app);

    if (combatLog.is_open())
    {
        combatLog << "========================================" << std::endl;
        combatLog << "Combat Log - Session: " << sessionID << std::endl;
        combatLog << "Started: " << getTimestamp() << std::endl;
        combatLog << "========================================" << std::endl;
    }

    if (aiLog.is_open())
    {
        aiLog << "========================================" << std::endl;
        aiLog << "AI Storyteller Log - Session: " << sessionID << std::endl;
        aiLog << "Started: " << getTimestamp() << std::endl;
        aiLog << "========================================" << std::endl;
    }

    initialized = true;
}

void Logger::logCombatStart(const std::string &enemyName, int enemyHP, int enemyDamage)
{
    std::ostringstream oss;
    oss << "\n=== COMBAT START ===" << std::endl
        << "Enemy: " << enemyName << std::endl
        << "HP: " << enemyHP << " | Damage: " << enemyDamage;
    writeToCombatLog(oss.str());
}

void Logger::logCombatTurn(int turnNumber)
{
    std::ostringstream oss;
    oss << "\n--- Turn " << turnNumber << " ---";
    writeToCombatLog(oss.str());
}

void Logger::logCardPlayed(const std::string &cardName, const std::string &cardType, int energyCost)
{
    std::ostringstream oss;
    oss << "[CARD] " << cardName << " (" << cardType << ") | Cost: " << energyCost;
    writeToCombatLog(oss.str());
}

void Logger::logDamageDealt(const std::string &source, int damage, const std::string &target, bool wasCrit)
{
    std::ostringstream oss;
    oss << "[DMG] " << source << " -> " << target << ": " << damage << (wasCrit ? " (CRIT!)" : "");
    writeToCombatLog(oss.str());
}

void Logger::logBlockGained(const std::string &source, int blockAmount)
{
    std::ostringstream oss;
    oss << "[BLOCK] " << source << " gained " << blockAmount << " block";
    writeToCombatLog(oss.str());
}

void Logger::logHealGained(const std::string &source, int healAmount, int currentHP, int maxHP)
{
    std::ostringstream oss;
    oss << "[HEAL] " << source << " healed " << healAmount << " HP (" << currentHP << "/" << maxHP << ")";
    writeToCombatLog(oss.str());
}

void Logger::logStrengthGained(const std::string &source, int strengthAmount, int totalStrength)
{
    std::ostringstream oss;
    oss << "[POWER] " << source << " gained +" << strengthAmount << " Strength (Total: " << totalStrength << ")";
    writeToCombatLog(oss.str());
}

void Logger::logDexterityGained(const std::string &source, int dexterityAmount, int totalDexterity)
{
    std::ostringstream oss;
    oss << "[POWER] " << source << " gained +" << dexterityAmount << " Dexterity (Total: " << totalDexterity << ")";
    writeToCombatLog(oss.str());
}

void Logger::logDebuffApplied(const std::string &debuffName, int stacks, const std::string &target)
{
    std::ostringstream oss;
    oss << "[DEBUFF] " << debuffName << " x" << stacks << " -> " << target;
    writeToCombatLog(oss.str());
}

void Logger::logEnemyBehavior(const std::string &enemyName, const std::string &behavior, int value)
{
    std::ostringstream oss;
    oss << "[ENEMY] " << enemyName << " | " << behavior;
    if (value > 0)
        oss << " (" << value << ")";
    writeToCombatLog(oss.str());
}

void Logger::logBonusDamage(const std::string &source, int bonusDamage, const std::string &pattern)
{
    std::ostringstream oss;
    oss << "[BONUS] " << source << " +" << bonusDamage << " from " << pattern << " pattern";
    writeToCombatLog(oss.str());
}

void Logger::logCombatEnd(bool victory, int playerHP, int turnsElapsed)
{
    std::ostringstream oss;
    oss << "\n=== COMBAT END ===" << std::endl
        << "Result: " << (victory ? "VICTORY" : "DEFEAT") << std::endl
        << "Player HP: " << playerHP << std::endl
        << "Turns: " << turnsElapsed;
    writeToCombatLog(oss.str());
}

void Logger::logError(const std::string &message)
{
    std::ostringstream oss;
    oss << "[ERROR] " << message;
    writeToCombatLog(oss.str());
}

void Logger::logAIDecision(const std::string &decision, const std::string &reason)
{
    std::ostringstream oss;
    oss << "[AI] Decision: " << decision << " | Reason: " << reason;
    writeToAILog(oss.str());
}

void Logger::logAITensionChange(const std::string &oldTension, const std::string &newTension, float difficultyScore)
{
    std::ostringstream oss;
    oss << "[AI] Tension: " << oldTension << " -> " << newTension
        << " | Difficulty Score: " << difficultyScore;
    writeToAILog(oss.str());
}

void Logger::logAIEventTrigger(const std::string &eventName, const std::string &condition)
{
    std::ostringstream oss;
    oss << "[AI] Event Triggered: " << eventName << " | Condition: " << condition;
    writeToAILog(oss.str());
}

void Logger::logAIInfluence(const std::string &enemyName, int adjustedHP, int adjustedDamage, float multiplier)
{
    std::ostringstream oss;
    oss << "[AI] Enemy Adjusted: " << enemyName
        << " | HP: " << adjustedHP << ", DMG: " << adjustedDamage
        << " | Multiplier: x" << multiplier;
    writeToAILog(oss.str());
}

void Logger::flush()
{
    if (combatLog.is_open())
        combatLog.flush();
    if (aiLog.is_open())
        aiLog.flush();
}

std::string Logger::getTimestamp()
{
    time_t now = time(0);
    struct tm timeInfo;
    localtime_s(&timeInfo, &now);

    std::ostringstream oss;
    oss << std::setfill('0') << std::setw(2) << timeInfo.tm_hour << ":"
        << std::setfill('0') << std::setw(2) << timeInfo.tm_min << ":"
        << std::setfill('0') << std::setw(2) << timeInfo.tm_sec;

    return oss.str();
}

std::string Logger::generateSessionID()
{
    time_t now = time(0);
    struct tm timeInfo;
    localtime_s(&timeInfo, &now);

    std::ostringstream oss;
    oss << (timeInfo.tm_year + 1900)
        << std::setfill('0') << std::setw(2) << (timeInfo.tm_mon + 1)
        << std::setfill('0') << std::setw(2) << timeInfo.tm_mday << "_"
        << std::setfill('0') << std::setw(2) << timeInfo.tm_hour
        << std::setfill('0') << std::setw(2) << timeInfo.tm_min
        << std::setfill('0') << std::setw(2) << timeInfo.tm_sec;

    return oss.str();
}

void Logger::writeToCombatLog(const std::string &message)
{
    if (combatLog.is_open())
    {
        combatLog << "[" << getTimestamp() << "] " << message << std::endl;
        combatLog.flush();
    }
}

void Logger::writeToAILog(const std::string &message)
{
    if (aiLog.is_open())
    {
        aiLog << "[" << getTimestamp() << "] " << message << std::endl;
        aiLog.flush();
    }
}
