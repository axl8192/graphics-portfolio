#pragma once
#include "Common/Common.hpp"
#include <string.h>

class Input
{
private:

    //### Variables ###//
    static GLFWwindow* window;

    static bool keyIsActive[GLFW_KEY_LAST+1]; // If a key is currently down or not
    static bool keyEvents[2*(GLFW_KEY_LAST+1)]; // If a key was pressed or released

    static bool mouseButtonIsActive[GLFW_MOUSE_BUTTON_LAST+1]; // If a mouse button is currently down or not
    static bool mouseButtonEvents[2*(GLFW_MOUSE_BUTTON_LAST+1)]; // If a mouse button was pressed or released

    static double mousePrevPos[2];
    static double mousePos[2];

    static double scrollY;

    //### Callbacks ###//
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
        if (key != GLFW_KEY_UNKNOWN && action != GLFW_REPEAT){
            keyIsActive[key] = (bool) action;
            keyEvents[2*key+action] = true;
        }
    }
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods){
        mouseButtonIsActive[button] = (bool) action;
        mouseButtonEvents[2*button+action] = true;
    }
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
        Input::scrollY = yoffset;
    }

public:

    //### Init, utilities and options ###//
    static void init(GLFWwindow* window){
        Input::window = window;
        glfwSetKeyCallback(window, Input::keyCallback);
        glfwSetMouseButtonCallback(window, Input::mouseButtonCallback);
        glfwSetScrollCallback(window, Input::scrollCallback);
    }

    static void resetEvents(){
        // Empty event lists
        memset(keyEvents, 0, 2*(GLFW_KEY_LAST+1)*sizeof(bool));
        memset(mouseButtonEvents, 0, 2*(GLFW_MOUSE_BUTTON_LAST+1)*sizeof(bool));

        // Reset scrollY
        Input::scrollY = 0;
        
        // Set mousePrevPos and mousePos
        memcpy(Input::mousePrevPos, Input::mousePos, 2*sizeof(double));
        glfwGetCursorPos(window, &Input::mousePos[0], &Input::mousePos[1]);

        // Call the callbacks
        glfwPollEvents(); 
    }

    static void setCursorMode(int mode){
        glfwSetInputMode(window, GLFW_CURSOR, mode);
    }

    static void setRawMouseMotion(bool rawMotion){
        if (glfwRawMouseMotionSupported()) glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, rawMotion);
        else std::cout << "Raw mouse motion not supported. Sorry XD." << std::endl;
    }

    //### Keyboard Input ###//
    static bool getKey(int key){
        return keyIsActive[key];
    }

    static bool getKeyDown(int key){
        return keyEvents[2*key+GLFW_PRESS];
    }

    static bool getKeyUp(int key){
        return keyEvents[2*key+GLFW_RELEASE];
    }

    //### Mouse Input ###//
    static bool getMouseButton(int button){
        return mouseButtonIsActive[button];
    }

    static bool getMouseButtonDown(int button){
        return mouseButtonEvents[2*button+GLFW_PRESS];
    }

    static bool getMouseButtonUp(int button){
        return mouseButtonEvents[2*button+GLFW_RELEASE];
    }

    static double getMouseRel(int coord){
        return (Input::mousePos[coord]-Input::mousePrevPos[coord]);
    }

    static double getMouseScroll(){
        return Input::scrollY;
    }

    static double getMousePos(int index){
        return mousePos[index];
    }

};

GLFWwindow* Input::window;

bool Input::keyIsActive[GLFW_KEY_LAST+1];
bool Input::keyEvents[2*(GLFW_KEY_LAST+1)];

bool Input::mouseButtonIsActive[GLFW_MOUSE_BUTTON_LAST+1];
bool Input::mouseButtonEvents[2*(GLFW_MOUSE_BUTTON_LAST+1)];

double Input::scrollY = 0;

double Input::mousePrevPos[2];
double Input::mousePos[2];
