#include "CombatLogNode.h"
#include <iostream>

// Constructor [Implementation]
CombatLogList::CombatLogList(int max) : head((CombatLogNode*)0), tail((CombatLogNode*)0), size(0), maxSize(max) {}
// Destructor [Implementation]
CombatLogList::~CombatLogList()
{
    clear();
}
// Add message [Implementation]
void CombatLogList::addMessage(const std::string &message)
{
    CombatLogNode *newNode = new CombatLogNode(message);

    if (!head)
    {
        head = tail = newNode;
    }
    else
    {
        tail->next = newNode;
        tail = newNode;
    }

    size++;
    std::cout << "[CombatLog] " << message << std::endl;
    while (size > maxSize)
    {
        CombatLogNode *temp = head;
        head = head->next;
        delete temp;
        size--;
    }
}
// Get size [Implementation]
int CombatLogList::getSize() const { 
    return size; 
}
// Get head [Implementation]
CombatLogNode *CombatLogList::getHead() const { 
    return head; 
}
// Clear all messages [Implementation]
void CombatLogList::clear()
{
    while (head)
    {
        CombatLogNode *temp = head;
        head = head->next;
        delete temp;
    }
    tail = (CombatLogNode*)0;
    size = 0;
}
