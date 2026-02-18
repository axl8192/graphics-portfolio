#pragma once
#include "Common/Common.hpp"
#include "Shader.hpp"
#include "Input.hpp"
#include "Mesh.hpp"
#include "Clock.hpp"

#ifdef INCLUDES_SOUND
    #include "Sound.hpp"
#endif

void GLAPIENTRY DebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
    std::cout << (type==GL_DEBUG_TYPE_ERROR?"***ERROR*** ":"") << "GL_CALLBACK: " << message << std::endl;
}

class Renderer
{
private:
    static GLFWwindow* window;
    static int winWidth;
    static int winHeight;

    static GLFWmonitor* monitor;
public:
    static const char* winCaption;
    static glm::mat4 mainVPmat;

    static void createWindow(int winWidth, int winHeight, const char* winCaption, int* hints, int hintsSize, 
                             int vSync = 1, bool fullscreen = false, GLenum zTest = GL_LEQUAL, bool backfaceCulling = false, bool blending = false){
    
        /* Initialize GLFW */
        if (!glfwInit()) { std::cout << "Glfw Window Error" << std::endl; exit(-1); }
        
        /* Define window's hints */
        int hintCount = hintsSize/(2*sizeof(int));
        for(int i=0; i<hintCount; i++){
            glfwWindowHint(hints[2*i], hints[2*i+1]);
        }

        /* Create window and its OpenGL context */
        if (fullscreen){
            Renderer::monitor = glfwGetPrimaryMonitor();
            const GLFWvidmode* mode = glfwGetVideoMode(monitor);
            Renderer::winWidth = mode->width;
            Renderer::winHeight = mode->height;
        }
        else{
            Renderer::monitor = NULL;
            Renderer::winWidth = winWidth;
            Renderer::winHeight = winHeight;
        }
        Renderer::winCaption = winCaption;
        Renderer::window = glfwCreateWindow(Renderer::winWidth, Renderer::winHeight, Renderer::winCaption, Renderer::monitor, NULL);

        if (!Renderer::window){ glfwTerminate(); std::cout << "GLFW Window Error" << std::endl; exit(-1); }

        /* Make the window's context current */
        glfwMakeContextCurrent(Renderer::window);

        /* Initialize GLEW */
        if (glewInit() != GLEW_OK) { std::cout << "GLEW Error" << std::endl; exit(-1); }

        /* Some other window and render options */
        glfwSwapInterval(vSync); //VSYNC (number of screen updates for OpenGL to wait before render)

        if(zTest!=GL_NONE){
            glEnable(GL_DEPTH_TEST); //Enable depth test
            glDepthFunc(zTest); //Accept fragment if it closer or at equal distance to the camera than the former one
        }

        if(backfaceCulling){
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            glFrontFace(GL_CCW);
        }

        if(blending){
            glEnable(GL_BLEND); // Enable Blending
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Select Blending Function
        }
        
        glEnable(GL_DEBUG_OUTPUT); //Enable Debug
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); //Make Debug Synchronous
        glDebugMessageCallback(DebugCallback, 0); //Specify Callback Function
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE); //Disable Non-Error Notifications
        
        /*Init input capture and sound player*/
        Input::init(window);
        Clock::init(window, winCaption);
        #ifdef INCLUDES_SOUND
            SoundPlayer::init();
        #endif

        /* Print OpenGl Version */
        std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
    }

    static void draw(DrawData drawData){
        if(drawData.usesIB){
            glDrawElements(drawData.drawMode, drawData.vertexCount, GL_UNSIGNED_INT, nullptr);
        }else{
            glDrawArrays(drawData.drawMode, 0, drawData.vertexCount);
        }
    }

    static void clear(GLbitfield mask){glClear(mask);}
    static void swapBuffers(){glfwSwapBuffers(Renderer::window);}

    static GLFWwindow* getWindow(){return Renderer::window;}
    static bool windowShouldClose(){return glfwWindowShouldClose(window);}
    static void terminate(){
        glfwTerminate();
        #ifdef INCLUDES_SOUND
            SoundPlayer::deinit();
        #endif
    }

    static int getWinWitdh(){return winWidth;}
    static int getWinHeight(){return winHeight;}

    static void setFullscreen(int width = 0, int height = 0){
        Renderer::monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        Renderer::winWidth = (width==0? mode->width: width);
        Renderer::winHeight = (height==0? mode->height: height);
    
        glfwSetWindowMonitor(window, monitor, 0, 0, winWidth, winHeight, mode->refreshRate);
        glViewport(0, 0, Renderer::getWinWitdh(), Renderer::getWinHeight());
    }
    static void setWindowed(int width, int height){
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        Renderer::winWidth = width;
        Renderer::winHeight = height;
    
        glfwSetWindowMonitor(window, NULL, mode->width/2-width/2, mode->height/2-height/2, width, height, mode->refreshRate);
        glViewport(0, 0, Renderer::getWinWitdh(), Renderer::getWinHeight());
    }
};

GLFWwindow* Renderer::window;
const char* Renderer::winCaption;
int Renderer::winWidth;
int Renderer::winHeight;
GLFWmonitor* Renderer::monitor;
glm::mat4 Renderer::mainVPmat;