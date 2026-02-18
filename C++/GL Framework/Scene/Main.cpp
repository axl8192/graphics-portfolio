#include "../_Framework/GLFramework.hpp"
#include "Axes.hpp"
#include "FPCamera.hpp"
#include "Skybox.hpp"
#include "Object.hpp"
#include "Lights.hpp"

//### Main ###//
int main(void){
    
    //### Create Window ###//
    int winWidth = 1280, winHeight = 800;
    bool fullscreen = false; // Fullscreen set winWidth and winHeight to native resolution
    const char* winCaption = "Scene";
    
    int hints[] = {
        GLFW_CONTEXT_VERSION_MAJOR, 4,
        GLFW_CONTEXT_VERSION_MINOR, 2,
        GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE,
        GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE,
        
        GLFW_RESIZABLE, GLFW_FALSE, //Disable Resize
        GLFW_SAMPLES, 16 //MSAA (x16)
    };
    
    Renderer::createWindow(winWidth, winHeight, winCaption, hints, sizeof(hints), 0, fullscreen, GL_LEQUAL, false);
    Input::setCursorMode(GLFW_CURSOR_DISABLED);
    Input::setRawMouseMotion(true);
    
    //### Objects ###//
    FPCamera mainCamera;
    
    //Skybox skybox(&mainCamera, "Textures/Storforsen3/posx.jpg", "Textures/Storforsen3/negx.jpg", "Textures/Storforsen3/posy.jpg", "Textures/Storforsen3/negy.jpg", "Textures/Storforsen3/posz.jpg", "Textures/Storforsen3/negz.jpg");
    Axes axes;
    DirectionalLight dLight;
    PointLight pLight(mainCamera);
    SpotLight sLight(mainCamera);
    
    Object obj(mainCamera, dLight, pLight, sLight);

    //### Main Loop ###//
    while (!Renderer::windowShouldClose())
    {
        //### Begin Frame ###//
        Renderer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //### Get Input and Update Camera ###//
        if(Input::getKeyDown(GLFW_KEY_SPACE)) mainCamera.canMove = !mainCamera.canMove;
        mainCamera.update();

        //### Render ###//
        //skybox.render();
        pLight.draw();
        sLight.draw();
        obj.draw();
        axes.draw(mainCamera);

        //### End Frame ###//
        Renderer::swapBuffers();
        Input::resetEvents();
        Clock::updateFrameTime(true, true, 0.1);
    }

    Renderer::terminate();
    return 0;
}