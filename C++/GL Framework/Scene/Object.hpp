#pragma once
#include "../_Framework/GLFramework.hpp"
#include "FPCamera.hpp"
#include "Lights.hpp"

class Object
{
private:
    Mesh mesh;
    Shader basicShader;
    Texture texture;
    
    FPCamera& camera;

    PointLight pLight;
    DirectionalLight dLight;
    SpotLight sLight;
public:
    glm::vec3 position = glm::vec3(1,1.1f,1.2f);
    glm::vec3 rotation = glm::vec3(0,pi,0);
    
    Object(FPCamera& iCamera, DirectionalLight& idLight, PointLight& ipLight, SpotLight& isLight): camera(iCamera), dLight(idLight), pLight(ipLight), sLight(isLight){
        mesh.loadDataFromObj("Models/CubeDimension.obj");
        texture.setTexture("Textures/cubeTexture.png");
        basicShader.setProgram("Shaders/cubeShader.glsl");
    }
    
    void draw(){
        glm::mat4 modelMat = glm::translate(position)*glm::rotate(rotation.y, glm::vec3(0,1,0));
        //rotation.y += 1*Clock::getFrameTime();

        basicShader.setUniform("MVP", camera.getProjMat()*camera.getViewMat()*modelMat);
        basicShader.setUniform("objModelMat", modelMat);
        basicShader.setUniform("cameraPos", camera.getPosition());

        basicShader.setUniform("directionalColor", dLight.color);
        basicShader.setUniform("directionalDirection", dLight.direction);
        basicShader.setUniform("directionalAmbient", dLight.ambient);

        basicShader.setUniform("pointPos", pLight.position);
        basicShader.setUniform("pointColor", pLight.color);
        basicShader.setUniform("pointIntParamA", pLight.a);
        basicShader.setUniform("pointIntParamB", pLight.b);

        basicShader.setUniform("spotPos", sLight.position);
        basicShader.setUniform("spotColor", sLight.color);
        basicShader.setUniform("spotIntParamA", sLight.a);
        basicShader.setUniform("spotIntParamB", sLight.b);
        basicShader.setUniform("rawSpotDir", sLight.direction);
        basicShader.setUniform("rawSpotInAng", sLight.inAng);
        basicShader.setUniform("rawSpotOutAng", sLight.outAng);
        
        texture.bind();
        mesh.bind();
        Renderer::draw(mesh.drawData);
    }
};

