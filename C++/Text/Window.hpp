#pragma once
#include "Globals.hpp"
#include "InputCallbacks.hpp"
#include "Draw.hpp"

void GLAPIENTRY debugCallback_gl(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam){
    std::cout << (type==GL_DEBUG_TYPE_ERROR?"***ERROR*** ":"") << "GL_CALLBACK: " << message << std::endl;
}

void window_size_callback(GLFWwindow* window, int width, int height){
    if(width > 0 && height > 0){
        winWidth = width;
        winHeight = height;
        glViewport(0, 0, winWidth, winHeight);
        projMat = glm::ortho(0.0f, (float)winWidth, (float)winHeight, 0.0f, 0.0f, 1.0f);
        
        FileEditorNode* currentNode = fileEditorLLEntry->next;
        while(currentNode != fileEditorLLEntry){
            FileTextEditor* fileEditor = currentNode->editor;
            fileEditor->setSize(winWidth, winHeight-fileTabHeight);
            currentNode = currentNode->next;
        }
        draw();
    }
}

void drop_callback(GLFWwindow* window, int count, const char** paths){
    for (int i = 0; i<count; i++){
        DWORD fileType = GetFileAttributesA(paths[i]);
        
        if(FILE_ATTRIBUTE_DIRECTORY&fileType){
            cout << "Cannot open a directory" << endl;
        }else{
            SFTE = new FileTextEditor(new TextLL(paths[i]));
            insertEditorBack(SFTE);
            
            while(currentEditorNode->next != fileEditorLLEntry){
                currentEditorNode = currentEditorNode->next;
            }

        }
    }
}

void createWindow(){
    if (!glfwInit()) { std::cout << "GLFW Window Error" << std::endl; exit(-1); }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 1);

    mainWindow = glfwCreateWindow(winWidth, winHeight, winCaption, NULL, NULL);
    if (!mainWindow){ glfwTerminate(); std::cout << "GLFW Window Error" << std::endl; exit(-1); }
    glfwMakeContextCurrent(mainWindow);

    if (glewInit() != GLEW_OK) { std::cout << "GLEW Error" << std::endl; exit(-1); }

    glfwSwapInterval(0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Clock::init(mainWindow, winCaption);

    glfwSetCharCallback(mainWindow, character_callback);
    glfwSetKeyCallback(mainWindow, key_callback);
    glfwSetDropCallback(mainWindow, drop_callback);

    glfwSetWindowSizeLimits(mainWindow, 200, 200, GLFW_DONT_CARE, GLFW_DONT_CARE);
    glfwSetWindowSizeCallback(mainWindow, window_size_callback);

    #ifdef DEBUG_BUILD
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(debugCallback_gl, 0);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, 0, GL_FALSE);
        std::cout << "GL_VERSION: " << glGetString(GL_VERSION) << std::endl;
    #endif
}


