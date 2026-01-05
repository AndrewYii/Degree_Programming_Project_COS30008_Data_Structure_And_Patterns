#pragma once

#include "Card.h"


struct UndoAction
{
    Card *card;
    int position; 
    UndoAction(Card *c, int pos) : card(c), position(pos) {}
};

class UndoStack
{
private:
    struct StackNode
    {
        UndoAction action;
        StackNode *next;

        StackNode(const UndoAction &act) : action(act), next(nullptr) {}
    };

    StackNode *top;
    int size;
    int maxSize;

public:
    UndoStack(int maxCapacity = 50);
    ~UndoStack();
    void push(const UndoAction &action);
    UndoAction pop();
    UndoAction peek() const;
    bool isEmpty() const { return top == nullptr; }
    bool isFull() const { return size >= maxSize; }
    int getSize() const { return size; }

    void clear();
};
