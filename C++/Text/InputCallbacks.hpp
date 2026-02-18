#pragma once
#include "Globals.hpp"
#include "Input.hpp"

//### Input Asociations ###//
struct InputAsociation{
    void (*func)();
    int mods;
    int key;
};

std::vector<InputAsociation> inputAsociations = {
    InputAsociation{moveTextCursorRight, 0, GLFW_KEY_RIGHT},
    InputAsociation{moveTextCursorLeft, 0, GLFW_KEY_LEFT},
    InputAsociation{moveTextCursorDown, 0, GLFW_KEY_DOWN},
    InputAsociation{moveTextCursorUp, 0, GLFW_KEY_UP},

    InputAsociation{insertNewLine, 0, GLFW_KEY_ENTER},
    InputAsociation{insertNewTab, 0, GLFW_KEY_TAB},

    InputAsociation{removePreviousChar, 0, GLFW_KEY_BACKSPACE},
    InputAsociation{removeCurrentChar, 0, GLFW_KEY_DELETE},
    InputAsociation{insertNewLineAfterThisLine, GLFW_MOD_CONTROL, GLFW_KEY_ENTER},
    
    InputAsociation{saveCurrentFile, GLFW_MOD_CONTROL, GLFW_KEY_S},

    InputAsociation{nextEditor, 0, GLFW_KEY_PAGE_DOWN},
    InputAsociation{previousEditor, 0, GLFW_KEY_PAGE_UP},

    InputAsociation{closeCurrentEditor, 0, GLFW_KEY_ESCAPE},
};

//### Input callbacks ###//
void character_callback(GLFWwindow* window, unsigned int codepoint){
    if(codepoint <= 127){
        LLInsertNode(SFTE->cursorNode->previous, (char)codepoint);
        SFTE->practicalCursorColumn = calcCursorColum();
        SFTE->cursorColumn = calcCursorColum();
        adjustFirstShowingLineToCursor();
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if(action == GLFW_PRESS || action == GLFW_REPEAT){
        for(InputAsociation action: inputAsociations){
            if(action.mods == mods && action.key == key){
                action.func();
                return;
            }
        }
    }
}

//### Input framewise ###//
double timeFromDown = 0;
double timeFromUp = 0;
double timeFromRight = 0;
double timeFromLeft = 0;
double upOrDownTime = 0.03;

void getInput(){
    if(glfwGetKey(mainWindow, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(mainWindow, GLFW_KEY_LEFT_ALT) && glfwGetKey(mainWindow, GLFW_KEY_DOWN)){
        if(timeFromDown > upOrDownTime){
            firstShowedLinePlus();
            timeFromDown = 0;
        }else{
            timeFromDown += Clock::getFrameTime();
        }
    }else{
        timeFromDown = 0;
    }
    
    if(glfwGetKey(mainWindow, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(mainWindow, GLFW_KEY_LEFT_ALT) && glfwGetKey(mainWindow, GLFW_KEY_UP)){
        if(timeFromUp > upOrDownTime){
            firstShowedLineMinus();
            timeFromUp = 0;
        }else{
            timeFromUp += Clock::getFrameTime();
        }
    }else{
        timeFromUp = 0;
    }

    if(glfwGetKey(mainWindow, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(mainWindow, GLFW_KEY_LEFT_ALT) && glfwGetKey(mainWindow, GLFW_KEY_RIGHT)){
        if(timeFromRight > upOrDownTime){
            firstColumnShowedPlus();
            timeFromRight = 0;
        }else{
            timeFromRight += Clock::getFrameTime();
        }
    }else{
        timeFromRight = 0;
    }
    
    if(glfwGetKey(mainWindow, GLFW_KEY_LEFT_CONTROL) && glfwGetKey(mainWindow, GLFW_KEY_LEFT_ALT) && glfwGetKey(mainWindow, GLFW_KEY_LEFT)){
        if(timeFromLeft > upOrDownTime){
            firstColumnShowedMinus();
            timeFromLeft = 0;
        }else{
            timeFromLeft += Clock::getFrameTime();
        }
    }else{
        timeFromLeft = 0;
    }
}