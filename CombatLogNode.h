#pragma once

#include <string>


class CombatLogNode
{
public:
    // Message content
    std::string message;
    // Pointer to next node
    CombatLogNode *next;
    // Constructor
    CombatLogNode(const std::string &msg) : message(msg), next((CombatLogNode*)0) {}
};

class CombatLogList
{
private:
    // Head in the combat log list
    CombatLogNode *head;
    // Tail in the combat log list
    CombatLogNode *tail;
    // Current size
    int size;
    // Maximum size
    int maxSize;
public:
    // Constructor[Header]
    CombatLogList(int max = 20);
    // Destructor [Header]
    ~CombatLogList();
    // Add message [Header]
    void addMessage(const std::string &message);
    // Get size [Header]
    int getSize() const;
    // Get head [Header]
    CombatLogNode *getHead() const;
    // Clear all messages [Header]
    void clear();
};

