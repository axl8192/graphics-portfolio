#include "../_Framework/GLFramework.hpp"
#include "FPCamera.hpp"
#include "Object.hpp"
#include "Globals.hpp"
#include "ComputeShader.hpp"

//### Main ###//
int main(void){
    
    //### Create Window ###//
    const int winWidth = 1280, winHeight = 720;
    bool fullscreen = false; // Fullscreen set winWidth and winHeight to native resolution
    const char* winCaption = "Big Fragments";
    
    int hints[] = {
        GLFW_CONTEXT_VERSION_MAJOR, 4,
        GLFW_CONTEXT_VERSION_MINOR, 5,
        GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE,
        GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE,
        
        GLFW_RESIZABLE, GLFW_FALSE, //Disable Resize
        GLFW_SAMPLES, 16, //MSAA
    };
    
    Renderer::createWindow(winWidth, winHeight, winCaption, hints, sizeof(hints), 0, fullscreen, GL_LEQUAL, false);
    Input::setCursorMode(GLFW_CURSOR_DISABLED);
    Input::setRawMouseMotion(true);

    //### zBuffer and Compute Shaders ###//
    unsigned int zBuffer;
    glCreateBuffers(1, &zBuffer);
    glNamedBufferStorage(zBuffer, sizeof(int)*winWidth*winHeight, NULL, GL_NONE);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, zBuffer);

    ComputeShader bigFragsComputeShader("Shaders/bigFragsCompute.glsl");
    ComputeShader fillBigFragsIBComputeShader("Shaders/fillBigFragsIBCompute.glsl");

    //### Big Frags (Vertex and Index) Buffers and Shader ###//
    unsigned int bigFragsVB;
    glCreateBuffers(1, &bigFragsVB);
    glNamedBufferStorage(bigFragsVB, 6*4*winWidth*winHeight*sizeof(float), NULL, GL_NONE); //(x,y,z, r,g,b)*4*npixels
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, bigFragsVB);
    
    unsigned int bigFragsIB;
    glCreateBuffers(1, &bigFragsIB);
    glNamedBufferStorage(bigFragsIB, 6*winWidth*winHeight*sizeof(unsigned int), NULL, GL_NONE); //(a,b,c, d,e,f)*npixels
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, bigFragsIB);
    fillBigFragsIBComputeShader.run(winWidth, winHeight, 1);

    unsigned int bigFragsVA;
    glCreateVertexArrays(1, &bigFragsVA);
    glVertexArrayVertexBuffer(bigFragsVA, 0, bigFragsVB, 0, 6*sizeof(float));

    glEnableVertexArrayAttrib(bigFragsVA, 0);
    glVertexArrayAttribFormat(bigFragsVA, 0, 3, GL_FLOAT, GL_FALSE, 0);
    glVertexArrayAttribBinding(bigFragsVA, 0, 0);

    glEnableVertexArrayAttrib(bigFragsVA, 1);
    glVertexArrayAttribFormat(bigFragsVA, 1, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float));
    glVertexArrayAttribBinding(bigFragsVA, 1, 0);

    glVertexArrayElementBuffer(bigFragsVA, bigFragsIB);

    Shader bigFragsShader("Shaders/bigFrags.glsl");

    //### Color Framebuffer ###//
    unsigned int framebufferColorTexture;
    glCreateTextures(GL_TEXTURE_2D, 1, &framebufferColorTexture);
    glTextureStorage2D(framebufferColorTexture, 1, GL_RGBA8, winWidth, winHeight);

    unsigned int framebuffer;
    glCreateFramebuffers(1, &framebuffer);
    glNamedFramebufferTexture(framebuffer, GL_COLOR_ATTACHMENT0, framebufferColorTexture, 0);

    //### Camera and Main Obj ###//
    mainCamera.setParams(65, 0.1f, 100.0f);
    Object obj;

    //### Main Loop ###//
    bool renderFrags = false;
    while (!Renderer::windowShouldClose())
    {
        //### Camera input ###//
        if(Input::getKeyDown(GLFW_KEY_SPACE)) mainCamera.canMove = !mainCamera.canMove;
        mainCamera.update();

        //### Render ###//
        Renderer::clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if(!renderFrags){
            glClearNamedBufferData(zBuffer, GL_R32I, GL_RED_INTEGER, GL_INT, &upperCodedZLimit);
            obj.draw();
        }
        
        if(renderFrags){
            glBindVertexArray(bigFragsVA);
            bigFragsShader.setUniform("viewProjMat", mainCamera.getProjMat()*mainCamera.getViewMat());
            bigFragsShader.bind();
            glDrawElements(GL_TRIANGLES, 6*winWidth*winHeight, GL_UNSIGNED_INT, nullptr);
        }

        //### Generate big fragments ###//
        if(Input::getKeyDown(GLFW_KEY_Q)){
            if(!renderFrags){

                //### Call compute shader to fill vertex buffer buffer ###//
                glBlitNamedFramebuffer(0, framebuffer, 0, 0, winWidth, winHeight, 0, 0, winWidth, winHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST); //From 0 (back buffer) to framebuffer

                glm::mat4 inverseViewMat = glm::inverse(mainCamera.getViewMat());
                
                bigFragsComputeShader.setUniform("inverseViewMat", inverseViewMat);
                bigFragsComputeShader.setUniform("winWidth", winWidth);
                bigFragsComputeShader.setUniform("winHeight", winHeight);
                bigFragsComputeShader.setUniform("upperCodedZLimit", upperCodedZLimit);
                bigFragsComputeShader.setUniform("nearPlane", mainCamera.nearPlane);
                bigFragsComputeShader.setUniform("farPlane", mainCamera.farPlane);
                bigFragsComputeShader.setUniform("tanAngOverTwo", tan(glm::radians(mainCamera.fov)/2.0f));

                glBindTextureUnit(0, framebufferColorTexture);

                bigFragsComputeShader.run(winWidth, winHeight, 1);
            }
            renderFrags = !renderFrags;
        }

        //### End frame ###//
        Renderer::swapBuffers();
        Input::resetEvents();
        Clock::updateFrameTime(true, true, 0.1);
    }

    //### Delete Everything and Quit ###//
    glDeleteBuffers(1, &zBuffer);

    glDeleteBuffers(1, &bigFragsVB);
    glDeleteBuffers(1, &bigFragsIB);
    glDeleteVertexArrays(1, &bigFragsVA);

    glDeleteFramebuffers(1, &framebuffer);
    glDeleteTextures(1, &framebufferColorTexture);

    Renderer::terminate();
    return 0;
}