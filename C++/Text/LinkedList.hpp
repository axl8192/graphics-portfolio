#pragma once
#include <iostream>

//### Char Linked List ###//
struct LLNode{
    LLNode* previous;
    char character;
    LLNode* next;
};

void LLCreate(LLNode** entryNode, LLNode** endingNode){
    (*entryNode) = new LLNode;
    (*endingNode) = new LLNode;

    (*entryNode)->previous = nullptr;
    (*entryNode)->next = (*endingNode);
    (*entryNode)->character = '\0';

    (*endingNode)->previous = (*entryNode);
    (*endingNode)->next = nullptr;
    (*endingNode)->character = '\0';
}

LLNode* LLInsertNode(LLNode* previousNode, char character){
    
    LLNode* newNode  = new LLNode;

    newNode->previous = previousNode;
    newNode->next = previousNode->next;
    newNode->character = character;
    
    if(newNode->next != nullptr){
        newNode->next->previous = newNode;
    }

    previousNode->next = newNode;

    return newNode;
}

void LLRemoveNode(LLNode* node){
    assert(node->previous != nullptr && node->next != nullptr);

    node->previous->next = node->next;
    node->next->previous = node->previous;

    delete node;
}

void LLPrintDbg(LLNode* baseNode){
    LLNode* current = baseNode;
    while(current != nullptr){
        std::cout  << "Char: " << current->character << " | This: " << current  << " | Prev: " << current->previous << " | Next: " << current->next << std::endl;
        current = current->next;
    }
}

void LLInsertString(LLNode* previousNode, const char* text){
    LLNode* currentNode = previousNode;
    for(int i = 0; i< strlen(text); i++){
        LLInsertNode(currentNode, text[i]);
        currentNode = currentNode->next;
    }
}



