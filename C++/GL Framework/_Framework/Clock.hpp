#pragma once
#include "Common/Common.hpp"
#include <unordered_map>

class Clock
{
private:
    static string baseCaption;
    static GLFWwindow* window;
    
    static double frameTime;
    static double timeFromUpdate;
    static unsigned int framesFromUpdate;
    static double initialFrameTime;
    
    static std::unordered_map<string, float> timers;

    static void updateTimers(){
        for(auto iter = timers.begin(); iter != timers.end(); iter++){
            iter->second += 1*Clock::getFrameTime();
        }
    }

public:
    //## Base Functions ##//
    static void init(GLFWwindow* window, string baseCaption){
        Clock::window  = window;
        Clock::baseCaption  = baseCaption;
    }

    static void updateFrameTime(bool fpsCaption = false, bool frameTimeCaption = false, float updateRate = 0.1){
        Clock::frameTime = Clock::getTime()-Clock::initialFrameTime;
        if (fpsCaption || frameTimeCaption){
            Clock::timeFromUpdate += Clock::frameTime;
            Clock::framesFromUpdate += 1;
            
            if (Clock::timeFromUpdate >= updateRate){
                std::ostringstream newCaption;
                newCaption << Clock::baseCaption;
                if (fpsCaption) newCaption << " - FPS: " << int(Clock::framesFromUpdate/Clock::timeFromUpdate);
                if (frameTimeCaption) newCaption << " - Frame Time: " << 1000*Clock::frameTime << " ms";
                
                glfwSetWindowTitle(Clock::window, newCaption.str().c_str());

                Clock::timeFromUpdate = 0;
                Clock::framesFromUpdate = 0;
            }
        }

        Clock::updateTimers();
        Clock::initialFrameTime = glfwGetTime();
    }
    
    static double getFrameTime(){ return frameTime; }
    static double getTime(){ return glfwGetTime(); }

    //## Timer Functions ##//
    static void setTimer(string name){
        timers[name] = 0.0f;
    }
    static float getTimer(string name){
        if(timers.find(name) != timers.end()){
            return timers[name];
        }else{
            return 0.0f;
        }
    }

    static void deleteTimer(string name){
        if(timers.find(name) != timers.end()){
            timers.erase(name);
        }
    }

    static void deleteAllTimers(){
        timers.clear();
    }
    
};

double Clock::frameTime = 0.0;
double Clock::timeFromUpdate = 0.0;
unsigned int Clock::framesFromUpdate = 0;
double Clock::initialFrameTime = 0;
std::unordered_map<string, float> Clock::timers;

string Clock::baseCaption = "Window";
GLFWwindow* Clock::window;