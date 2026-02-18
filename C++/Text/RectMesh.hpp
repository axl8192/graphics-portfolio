#pragma once
#include "Helpers/Helpers.hpp"

unsigned int defaultRectIB;

void initGlobalIB(){
    unsigned int defaultSpriteIndexData[6] = { 0,1,2, 1,3,2 };
    glCreateBuffers(1, &defaultRectIB);
    glNamedBufferData(defaultRectIB, sizeof(defaultSpriteIndexData), defaultSpriteIndexData, GL_STATIC_DRAW);
}

void createRectMesh(unsigned int* VA, unsigned int* VB, float* vertData, int sizeofVertData, std::initializer_list<int> format){
    glCreateBuffers(1, VB);
    glNamedBufferData(*VB, sizeofVertData, vertData, GL_STATIC_DRAW);

    glCreateVertexArrays(1, VA);
    
    int stride = 0;
    int index = 0;
    for(int elementCount: format){
        glEnableVertexArrayAttrib(*VA, index);
        glVertexArrayAttribFormat(*VA, index, elementCount, GL_FLOAT, GL_FALSE, stride*sizeof(float));
        glVertexArrayAttribBinding(*VA, index, 0);

        stride += elementCount;
        index += 1;
    }
    
    glVertexArrayVertexBuffer(*VA, 0, *VB, 0, stride*sizeof(float));

    glVertexArrayElementBuffer(*VA, defaultRectIB);
}

