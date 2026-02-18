#pragma once
#include "../_Framework/GLFramework.hpp"
#include "FPCamera.hpp"

class DirectionalLight{
public:
    glm::vec3 color = glm::vec3(1,1,1);
    glm::vec3 direction = glm::vec3(0,-1,0);
    float ambient = 0.01;
};

class PointLight{
private:
    Mesh mesh;
    Shader basicShader;
    FPCamera& camera;

public:
    glm::vec3 position = glm::vec3(3,2,3);
    glm::vec3 color = glm::vec3(1,1,1);
    float a = 0.05, b = 0.01;

    PointLight(FPCamera& iCamera): camera(iCamera){
        mesh.loadDataFromObj("Models/cube.obj", false, false);
        basicShader.setProgram("Shaders/basicShader.glsl");
    }

    void draw(){
        basicShader.setUniform("MVP", camera.getProjMat()*camera.getViewMat()*glm::translate(position)*glm::scale(glm::vec3(0.3,0.3,0.3)));
        basicShader.setUniform("u_color", color);
        mesh.bind();
        Renderer::draw(mesh.drawData);
    }
};

class SpotLight{
private:
    Mesh mesh;
    Shader basicShader;
    FPCamera& camera;
    
public:
    glm::vec3 position = glm::vec3(3.7,2,3);
    glm::vec3 color = glm::vec3(0,0,1);
    glm::vec3 direction = glm::vec3(0,0.1,-1);
    float inAng = 30.0f, outAng = 35.0f;
    float a = 0.05, b = 0.01;

    SpotLight(FPCamera& iCamera): camera(iCamera){
        mesh.loadDataFromObj("Models/piramid.obj", false, false);
        basicShader.setProgram("Shaders/basicShader.glsl");
    }

    void draw(){
        float angh = atan2f(direction.z, direction.x);
        float angv = atan2f(direction.y, direction.x);
        basicShader.setUniform("MVP", camera.getProjMat()*camera.getViewMat()*
                                      glm::translate(position)*glm::scale(glm::vec3(0.3,0.3,0.3))*
                                      glm::rotate(angh, glm::vec3(0,1,0))*glm::rotate(-angv, glm::vec3(0,0,1)));
        basicShader.setUniform("u_color", color);
        mesh.bind();
        Renderer::draw(mesh.drawData);
    }
};

