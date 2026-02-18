#pragma once
#include "Common/Common.hpp"

#include <stb_image.h>

class Texture
{
private:
    unsigned int rendererID;
    unsigned char* localBuffer;
    int width, height, BPP;
public:
    Texture(){
        glGenTextures(1, &rendererID);
    }
    Texture(std::string path){
        glGenTextures(1, &rendererID);
        setTexture(path);
    }
    ~Texture(){
        glDeleteTextures(1, &rendererID);
    }

    void setTexture(std::string path){
        // Load Image File
        stbi_set_flip_vertically_on_load(1);
        localBuffer = stbi_load(path.c_str(), &width, &height, &BPP, 4);
        
        glBindTexture(GL_TEXTURE_2D, rendererID);
        // Obligatory Parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        // Give Data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        if(localBuffer) stbi_image_free(localBuffer);
    }
    void bind(unsigned int slot = 0){
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, rendererID);
    }
    void unbind(){
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};