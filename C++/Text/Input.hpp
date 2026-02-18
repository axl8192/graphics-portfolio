#pragma once
#include "Helpers/Helpers.hpp"
#include "Globals.hpp"
#include "FileTextEditor.hpp"

//### Calculate cursor column ###//
unsigned int calcCursorColum(){
    LLNode* currentNode = SFTE->cursorNode;
    unsigned int columCount = 1;
    while(true){
        if(currentNode->previous->previous != nullptr) {
            currentNode = currentNode->previous;
            if(currentNode->character == '\n'){ break; }
        }else{ break; }

        if(currentNode->character == '\t'){
            columCount += tabWidth;
        }else{
            columCount += 1;
        }
    }
    return columCount;
}

//### Camera ###//
void firstShowedLinePlus(){
    LLNode* currentNode = SFTE->firstShowingNode;

    while(true){
        currentNode = currentNode->next;
        
        if(currentNode->character == '\n'){
            SFTE->firstShowingNode = currentNode;
            SFTE->firstLineShowing += 1;
            break;
        }else if(currentNode->next == nullptr){
            break;
        }
    }
}

void firstShowedLineMinus(){
    LLNode* currentNode = SFTE->firstShowingNode;
    int newLineCount = 0;

    while(true){
        if(currentNode->character == '\n'){
            if(newLineCount == 0){
                newLineCount = 1;
            }else{
                SFTE->firstShowingNode = currentNode; 
                SFTE->firstLineShowing -= 1;
                break;
            }
        }else if(currentNode->previous == nullptr){
            if(newLineCount == 1){ SFTE->firstLineShowing -= 1; }
            SFTE->firstShowingNode = currentNode;
            break;
        }

        currentNode = currentNode->previous;
    }
}

void firstColumnShowedPlus(){
    SFTE->firstColumnShowing += 1;
}

void firstColumnShowedMinus(){
    if(SFTE->firstColumnShowing > 1){
        SFTE->firstColumnShowing -= 1;
    }
}

//### Helpers ###//
void cursorLineMinus(){
    SFTE->cursorLine -= 1;
    if(SFTE->cursorLine < SFTE->firstLineShowing){
        firstShowedLineMinus();
    }
}

void cursorLinePlus(){
    SFTE->cursorLine += 1;
    if(SFTE->cursorLine > SFTE->firstLineShowing+SFTE->maxLinesToShow - 2){
        firstShowedLinePlus();
    }
}

void adjustFirstShowingLineToCursor(){
    if(SFTE->cursorLine < SFTE->firstLineShowing){
        while(SFTE->cursorLine < SFTE->firstLineShowing){
            firstShowedLineMinus();
        }
    }else if(SFTE->cursorLine > SFTE->firstLineShowing+SFTE->maxLinesToShow - 1){
        while(SFTE->cursorLine > SFTE->firstLineShowing+SFTE->maxLinesToShow - 1){
            firstShowedLinePlus();
        }
    }

    if(SFTE->cursorColumn < SFTE->firstColumnShowing){
        while(SFTE->cursorColumn < SFTE->firstColumnShowing){
            firstColumnShowedMinus();
        }
    }else if(SFTE->cursorColumn > SFTE->firstColumnShowing+SFTE->maxColumnsToShow - 2){
        while(SFTE->cursorColumn > SFTE->firstColumnShowing+SFTE->maxColumnsToShow - 2){
            firstColumnShowedPlus();
        }
    }
}

//### Move cursor ###//
void moveTextCursorRight(){
    if(SFTE->cursorNode->next != nullptr){
        if(SFTE->cursorNode->character == '\n'){
            cursorLinePlus();
        }
        
        SFTE->cursorNode = SFTE->cursorNode->next;
        SFTE->practicalCursorColumn = calcCursorColum(); 
    }
    SFTE->cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

void moveTextCursorLeft(){
    if(SFTE->cursorNode->previous->previous != nullptr){
        SFTE->cursorNode = SFTE->cursorNode->previous;
        SFTE->practicalCursorColumn = calcCursorColum();

        if(SFTE->cursorNode->character == '\n'){
            cursorLineMinus();
        }
    }
    SFTE->cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

void moveTextCursorDown(){
    LLNode* currentNode = SFTE->cursorNode;
    unsigned int lineSkipsCount = 0;
    unsigned int columnCount = 0;
    while(true){
        if(lineSkipsCount == 1){
            if(currentNode->character == '\t'){columnCount += tabWidth;}
            else{columnCount += 1;}
        }
        if(columnCount >= SFTE->practicalCursorColumn){ cursorLinePlus(); break; }

        if(currentNode->character == '\n'){
            lineSkipsCount += 1;
            if(lineSkipsCount == 2){ cursorLinePlus(); break; }
        }else if(currentNode->next == nullptr){
            if(lineSkipsCount == 0){ currentNode = SFTE->cursorNode; }
            else{ cursorLinePlus(); }
            break;
        }
        currentNode = currentNode->next;
    }

    if(currentNode->character ==  '\t'){
        currentNode = currentNode->next;
    }
    SFTE->cursorNode = currentNode;

    SFTE->cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

void moveTextCursorUp(){
    LLNode* currentNode = SFTE->cursorNode;
    unsigned int lineSkipsCount = 0;
    while(true){
        currentNode = currentNode->previous;
        
        if(currentNode->character == '\n'){
            lineSkipsCount += 1;
            if(lineSkipsCount == 2){ cursorLineMinus(); break; }
        }else if(currentNode->previous == nullptr){ if(SFTE->cursorLine > 1){cursorLineMinus();} break; }
    }

    unsigned int columnCount = 0;
    bool passedInitialTabs = false;
    while(true){
        if(columnCount > 0 && currentNode->character != '\t'){ passedInitialTabs = true; }
        if(columnCount>=SFTE->practicalCursorColumn && passedInitialTabs){ break; }

        if(currentNode->character == '\t'){ columnCount += tabWidth; }
        else{ columnCount += 1; }
        currentNode = currentNode->next;
        
        if(currentNode->character == '\n' || currentNode->next == nullptr){ break; }
    }
    SFTE->cursorNode = currentNode;

    SFTE->cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

//### Enter special chars ###//
void insertNewLine(){
    LLInsertNode(SFTE->cursorNode->previous, '\n');

    LLNode* currentNode = SFTE->cursorNode->previous;
    while(currentNode->previous->character != '\n' && currentNode->previous->previous != nullptr){
        currentNode = currentNode->previous;
    }

    int tabCount = 0;
    while(currentNode->character == '\t'){
        tabCount += 1;
        currentNode = currentNode->next;
    }

    for(int i = 0; i<tabCount; i++){
        LLInsertNode(SFTE->cursorNode->previous, '\t');
    }
    SFTE->practicalCursorColumn = calcCursorColum();

    cursorLinePlus();
    SFTE->cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();

    SFTE->textLL->nLines += 1;
}

void insertNewTab(){
    LLInsertNode(SFTE->cursorNode->previous, '\t');
    SFTE->practicalCursorColumn = calcCursorColum();
    SFTE->cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

//### Remove char ###//
void removePreviousChar(){
    if(SFTE->cursorNode->previous->previous != nullptr){
        if(SFTE->cursorNode->previous->character == '\n'){
            cursorLineMinus();
        }

        if(SFTE->cursorNode->previous->character == '\n'){
            SFTE->textLL->nLines -= 1;
        }
        
        LLRemoveNode(SFTE->cursorNode->previous);
        SFTE->practicalCursorColumn = calcCursorColum();
    }

    SFTE->cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

void removeCurrentChar(){
    if(SFTE->cursorNode->next != nullptr){
        if(SFTE->cursorNode->character == '\n'){
            SFTE->textLL->nLines -= 1;
        }

        SFTE->cursorNode = SFTE->cursorNode->next;
        LLRemoveNode(SFTE->cursorNode->previous);
        SFTE->practicalCursorColumn = calcCursorColum();
    }

    SFTE->cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

//### Ctr other things ###//
void insertNewLineAfterThisLine(){
    LLNode* currentNode = SFTE->cursorNode;
    while(currentNode->previous->character != '\n' && currentNode->previous->previous != nullptr){
        currentNode = currentNode->previous;
    }

    int tabCount = 0;
    while(currentNode->character == '\t'){
        tabCount += 1;
        currentNode = currentNode->next;
    }
    
    while(currentNode->character != '\n' && currentNode->next != nullptr){
        currentNode = currentNode->next;
    }
    SFTE->cursorNode = currentNode;

    LLInsertNode(currentNode->previous, '\n');
    for(int i = 0; i<tabCount; i++){
        LLInsertNode(currentNode->previous, '\t');
    }
    SFTE->practicalCursorColumn = calcCursorColum();
    cursorLinePlus();
    SFTE->cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
    
    SFTE->textLL->nLines += 1;
}

//### Others? ###//
void saveCurrentFile(){
    FILE* fileStream = fopen(SFTE->textLL->filePath, "wb+");
    if(fileStream == nullptr){cout << "Error opening " << SFTE->textLL->filePath << endl; exit(-1);}
    
    LLNode* currentNode = SFTE->textLL->entry->next;
    while(currentNode != SFTE->textLL->ending){
        fputc((int)currentNode->character, fileStream);
        currentNode = currentNode->next;
    }

    cout << "Saved " << SFTE->textLL->filePath << endl;
    fclose(fileStream);
}

void nextEditor(){
    currentEditorNode = currentEditorNode->next;

    if(currentEditorNode == fileEditorLLEntry){
        currentEditorNode = currentEditorNode->next;
    }
    SFTE = currentEditorNode->editor;
}

void previousEditor(){
    FileEditorNode* nodeShadow = currentEditorNode;

    while(currentEditorNode->next != nodeShadow){
        currentEditorNode = currentEditorNode->next;
    }

    if(currentEditorNode == fileEditorLLEntry){
        while(currentEditorNode->next != fileEditorLLEntry){
            currentEditorNode = currentEditorNode->next;
        }
    }

    SFTE = currentEditorNode->editor;
}

void closeCurrentEditor(){
    // hack
    {
        if(fileEditorLLEntry->next->next == fileEditorLLEntry){
            cout << "Not allowed yet to close all the files" << endl;
            return;
        }
    }

    // Close current editor and go to next one
    FileEditorNode* currentNode = currentEditorNode->next;
    
    if(currentNode == fileEditorLLEntry){ currentNode = currentNode->next; }
    
    FileEditorNode* nodeShadow = currentEditorNode;
    FileEditorNode* prevEditor = currentEditorNode->next;
    while(prevEditor->next != nodeShadow){
        prevEditor = prevEditor->next;
    }

    prevEditor->next = currentEditorNode->next;

    delete currentEditorNode->editor;
    delete currentEditorNode;
    
    currentEditorNode = currentNode;
    SFTE = currentEditorNode->editor;
}
