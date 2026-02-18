#pragma once
#include "../_Framework/GLFramework.hpp"
#include "FPCamera.hpp"

class Skybox
{
private:
    FPCamera* mainCamera;
    
    Mesh mesh;
    Shader shader;
    unsigned int textureID;
public:
    Skybox(FPCamera* iMainCamera, std::string posX, std::string negX, std::string posY, std::string negY, std::string posZ, std::string negZ){
        
        //Cube and Shader Data
        mainCamera = iMainCamera;

        float cubeSemiLength = 0.5f;
        float skyboxPos_Data[] = {
            cubeSemiLength,cubeSemiLength,cubeSemiLength,
            cubeSemiLength,cubeSemiLength,-cubeSemiLength,
            cubeSemiLength,-cubeSemiLength,cubeSemiLength,
            cubeSemiLength,-cubeSemiLength,-cubeSemiLength,
            -cubeSemiLength,cubeSemiLength,cubeSemiLength,
            -cubeSemiLength,cubeSemiLength,-cubeSemiLength,
            -cubeSemiLength,-cubeSemiLength,cubeSemiLength,
            -cubeSemiLength,-cubeSemiLength,-cubeSemiLength,
        };
        unsigned int skyboxInd_Data[] = {
            0,1,2,
            1,3,2,
            
            6,5,4,
            6,7,5,

            7,1,5,
            7,3,1,

            4,0,6,
            0,2,6,
            
            1,4,5,
            1,0,4,

            2,3,7,
            7,6,2
        };
        mesh.setData(skyboxPos_Data, sizeof(skyboxPos_Data), {3}, skyboxInd_Data, sizeof(skyboxInd_Data));
        shader.setProgram("Shaders/Skybox.glsl");

        //Cubemap Texture
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

        int width, height, channels;
        std::string texturePaths[] = {posX, negX, posY, negY, posZ, negZ};
        for(int i=0; i<6; i++){
            unsigned char *data = stbi_load(texturePaths[i].c_str(), &width, &height, &channels, 3);
            if (data) glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            else std::cout << "Error loading "+texturePaths[i] << std::endl;
            stbi_image_free(data);
        }

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    void render(){
        glDepthMask(GL_FALSE);
        shader.setUniform("MVP", mainCamera->getProjMat()*glm::mat4(glm::mat3(mainCamera->getViewMat())));
        glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
        mesh.bind();
        Renderer::draw(mesh.drawData);
        glDepthMask(GL_TRUE);
    }

};

