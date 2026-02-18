#pragma once
#include "../_Framework/GLFramework.hpp"
#include "FPCamera.hpp"
#include "Globals.hpp"

class Object
{
private:
    Mesh mesh;
    Shader basicShader;
    Texture texture;
public:
    glm::vec3 position = glm::vec3(1,1.1f,1.2f);
    glm::vec3 rotation = glm::vec3(0,pi,0);
    
    Object(){
        mesh.loadDataFromObj("Models/CubeDimension.obj");
        texture.setTexture("Textures/cubeTexture.png");
        basicShader.setProgram("Shaders/object.glsl");
    }
    
    void draw(){
        glm::mat4 modelMat = glm::translate(position)*glm::rotate(rotation.y, glm::vec3(0,1,0));
        //rotation.y += 1*Clock::getFrameTime();

        basicShader.setUniform("winWidth", Renderer::getWinWitdh());
        basicShader.setUniform("modelViewMat", mainCamera.getViewMat()*modelMat);
        basicShader.setUniform("projMat", mainCamera.getProjMat());
        basicShader.setUniform("nearPlane", mainCamera.nearPlane);
        basicShader.setUniform("farPlane", mainCamera.farPlane);
        basicShader.setUniform("upperCodedZLimit", upperCodedZLimit);
        texture.bind();
        mesh.bind();
        Renderer::draw(mesh.drawData);
    }
};

