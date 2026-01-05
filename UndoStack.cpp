#include "UndoStack.h"
#include <iostream>

// Constructor
UndoStack::UndoStack(int maxCapacity): top(nullptr), size(0), maxSize(maxCapacity)
{
}

// Destructor
UndoStack::~UndoStack()
{
    clear();
}

// Push a new action onto the stack
void UndoStack::push(const UndoAction &action)
{
    if (isFull())
    {
        std::cout << "[UndoStack] Stack is full! Cannot push more actions." << std::endl;
        return;
    }

    StackNode *newNode = new StackNode(action);
    newNode->next = top;
    top = newNode;
    size++;
}

// Pop the top action from the stack
UndoAction UndoStack::pop()
{
    if (isEmpty())
    {
        std::cout << "[UndoStack] Stack is empty! Cannot pop." << std::endl;
        return UndoAction(nullptr, -1);
    }

    StackNode *temp = top;
    UndoAction action = temp->action;
    top = top->next;
    delete temp;
    size--;

    return action;
}

// Peek at the top action without removing it
UndoAction UndoStack::peek() const
{
    if (isEmpty())
    {
        return UndoAction(nullptr, -1);
    }

    return top->action;
}

// Clear the entire stack
void UndoStack::clear()
{
    while (!isEmpty())
    {
        pop();
    }
}
