#include "../_Framework/GLFramework.hpp"
#include "Axes.hpp"
#include "3DFunctionDrawer.hpp"
#include "OrbitCamera.hpp"
#include "GUI.hpp"
#include "Screenshot.hpp"

void processInput(OrbitCamera& camera, Function3D& funcDrawer, FuncGUI& funcGUI){
    float& keyRotVel = camera.keyRotVel;
    float& keyZoomVel = camera.keyZoomVel;
    float& dragRotVel = camera.dragRotVel;
    float& wheelZoomVel = camera.wheelZoomVel;
    float& pivotVel = camera.pivotVel;
    
    float& r_raw = camera.r_raw;
    float& angH = camera.angH;
    float& angV = camera.angV;
    float& pivotX = camera.pivotX;
    float& pivotY = camera.pivotY;
    
    float frametime = Clock::getFrameTime();
    int shiftMultiplier = 3*Input::getKey(GLFW_KEY_LEFT_SHIFT)+1;

    // Function Parameters
    if(Input::getKey(GLFW_KEY_LEFT)){ funcDrawer.a -= shiftMultiplier*frametime; }
    if(Input::getKey(GLFW_KEY_RIGHT)){ funcDrawer.a += shiftMultiplier*frametime; }
    if(Input::getKey(GLFW_KEY_DOWN)){ funcDrawer.b -= shiftMultiplier*frametime; }
    if(Input::getKey(GLFW_KEY_UP)){ funcDrawer.b += shiftMultiplier*frametime; }

    if(Input::getKeyUp(GLFW_KEY_Q)){ funcDrawer.changeCount(-shiftMultiplier*10); }
    if(Input::getKeyUp(GLFW_KEY_E)){ funcDrawer.changeCount(shiftMultiplier*10); }

    if(Input::getKey(GLFW_KEY_N)){ funcDrawer.changeSize(-2*shiftMultiplier*frametime); }
    if(Input::getKey(GLFW_KEY_M)){ funcDrawer.changeSize(2*shiftMultiplier*frametime); }

    // Show/Hide GUI
    if(Input::getKeyDown(GLFW_KEY_SPACE)){funcGUI.show = !funcGUI.show;}

    // Camera Keyboard Rotation-Zoom
    if(Input::getKey(GLFW_KEY_J)){ angH += keyRotVel*frametime *pi/180; }
    if(Input::getKey(GLFW_KEY_L)){ angH -= keyRotVel*frametime *pi/180; }

    if(Input::getKey(GLFW_KEY_K)){ angV -= keyRotVel*frametime *pi/180; }
    if(Input::getKey(GLFW_KEY_I)){ angV += keyRotVel*frametime *pi/180; }

    if(Input::getKey(GLFW_KEY_Z)){ r_raw -= shiftMultiplier*keyZoomVel*frametime; }
    if(Input::getKey(GLFW_KEY_X)){ r_raw += shiftMultiplier*keyZoomVel*frametime; }

    // Camera Mouse Rotation-Zoom
    if(Input::getMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)){
        angH += Input::getMouseRel(0)*dragRotVel*pi/180;
        angV += Input::getMouseRel(1)*dragRotVel*pi/180;
    }
    
    r_raw -= Input::getMouseScroll()*wheelZoomVel*shiftMultiplier;

    // R Clamping
    if (r_raw < 0.01) r_raw = 0.01;

    // Camera Translation
    if (Input::getKey(GLFW_KEY_W)){
        pivotX += -pivotVel*cos(angV)*sin(angH)*frametime; 
        pivotY += -pivotVel*cos(angV)*cos(angH)*frametime;
    }
    if (Input::getKey(GLFW_KEY_S)){
        pivotX -= -pivotVel*cos(angV)*sin(angH)*frametime; 
        pivotY -= -pivotVel*cos(angV)*cos(angH)*frametime;
    }
    if (Input::getKey(GLFW_KEY_A)){
        pivotX += pivotVel*cos(angV)*sin(angH+pi/2)*frametime; 
        pivotY += pivotVel*cos(angV)*cos(angH+pi/2)*frametime;
    }
    if (Input::getKey(GLFW_KEY_D)){
        pivotX -= pivotVel*cos(angV)*sin(angH+pi/2)*frametime; 
        pivotY -= pivotVel*cos(angV)*cos(angH+pi/2)*frametime;
    }
    if(Input::getKey(GLFW_KEY_C)){ pivotX = 0; pivotY = 0; }
}

//### Main ###//
int main(void){
    
    //### Create Window ###//
    int winWidth = 1280, winHeight = 800;
    bool fullscreen = false; // Fullscreen set winWidth and winHeight to native resolution
    const char* winCaption = "3D Graphicator";
    
    int hints[] = {
        GLFW_CONTEXT_VERSION_MAJOR, 3,
        GLFW_CONTEXT_VERSION_MINOR, 3,
        GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE,
        GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE,
        
        GLFW_RESIZABLE, GLFW_FALSE, //Disable Resize
        GLFW_SAMPLES, 16 //MSAA (x16)
    };
    
    Renderer::createWindow(winWidth, winHeight, winCaption, hints, sizeof(hints), 0, fullscreen);
    imGuiInit();
    ImGuiIO& IGIO = IG::GetIO();
    IGIO.IniFilename = NULL;

    //### Load Settings ###//
    std::ifstream settings("settings.cfg");
    std::string iFuncs[] = {
                            "sin(x+b*cos(a*y)*sin(a*x))", 
                            "0.5*sin(z)+0.5", "0.5*cos(z)+0.5", "1",
                            "1", "0.5*sin(z)+0.5", "0.5"
                           };

    if(settings.is_open()){
        std::string line;
        getline(settings, line);

        int i = 0;
        while(getline(settings, line)){
            iFuncs[i] = line;
            i++;
        }
    }
    
    //### Objects ###//
    OrbitCamera mainCamera;
    Axes axes;
    
    Function3D funcDrawer(iFuncs[0], iFuncs[1], iFuncs[2], iFuncs[3], iFuncs[4], iFuncs[5], iFuncs[6], 15.5f, 150, 1, 1);

    ImFont* fontM = IGIO.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 20);
    ImFont* fontS = IGIO.Fonts->AddFontFromFileTTF("Roboto-Medium.ttf", 15);
    FuncGUI funcGUI(fontM, fontS, funcDrawer, axes, mainCamera);

    //### Main Loop ###//
    while (!Renderer::windowShouldClose())
    {
        //### Begin Frame ###//
        Renderer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //Empty the color and depth buffers
        imGuiNewFrame();
        
        //### Update GUI, Controls and Camera ###//
        funcGUI.update();
        if (!funcGUI.isBeingUsed()) processInput(mainCamera, funcDrawer, funcGUI);
        mainCamera.update();

        if(Input::getKeyDown(GLFW_KEY_F11)){
            if(fullscreen) Renderer::setWindowed(winWidth, winHeight);
            else Renderer::setFullscreen();
            fullscreen = !fullscreen;
        }

        //### Render ###//
        axes.draw(mainCamera);
        funcDrawer.draw(mainCamera);
        if(Input::getKeyDown(GLFW_KEY_F5)){takeScreenshot(BMP); funcGUI.screenshotTextTimer = 2.0f;}
        imGuiRender();
        
        //### End Frame ###//
        Renderer::swapBuffers();
        Input::resetEvents(); // Clear input event lists and get the new event lists
        Clock::updateFrameTime(true, true, 0.1);
    }
    funcGUI.saveSettings();
    
    imGuiEnd();
    Renderer::terminate();
    return 0;
}