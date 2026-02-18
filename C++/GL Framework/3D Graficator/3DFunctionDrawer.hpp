#pragma once
#include "../_Framework/GLFramework.hpp"
#include <cstring>
#include "OrbitCamera.hpp"
#include "ShaderCodes.hpp"
#include "ShaderPassErrors.hpp"

class Function3D
{
private:
    unsigned int wireVAid;
    unsigned int solidVAid;
    unsigned int VBid;
    unsigned int wireIBid;
    unsigned int solidIBid;
    
    ShaderPassErrors funcShader;

    int wireIndCount;
    int solidIndCount;

    float size; // Half the grid side length
    unsigned int count; // Number of samples por grid side 

    std::string renderedFunc;
    std::string renderedColorFuncR;
    std::string renderedColorFuncG;
    std::string renderedColorFuncB;

    std::string renderedWireColorFuncR;
    std::string renderedWireColorFuncG;
    std::string renderedWireColorFuncB;

    bool shaderError;
    void updateGrid(){
        unsigned int countSqrd = count*count;

        wireIndCount = 4*countSqrd;
        solidIndCount = 6*countSqrd;

        float* grid_PosData = new float[3*countSqrd];
        unsigned int* gridWire_IndData = new unsigned int[wireIndCount];
        unsigned int* gridSolid_IndData = new unsigned int[solidIndCount];

        for(int i = 0; i<countSqrd; i++){
            grid_PosData[3*i] = (i%count) * 2*size/(count-1) - size;
            grid_PosData[3*i+1] = int(i/count) * 2*size/(count-1) - size;
            grid_PosData[3*i+2] = 0;
            
            if(i%count<(count-1)){
                gridWire_IndData[4*i] = i;
                gridWire_IndData[4*i+1] = i+1;
                
            }else{
                gridWire_IndData[4*i] = i;
                gridWire_IndData[4*i+1] = i;
            }
            
            if (int(i/count)<(count-1)){
                gridWire_IndData[4*i+2] = i;
                gridWire_IndData[4*i+3] = i+count;
            }else{
                gridWire_IndData[4*i+2] = i;
                gridWire_IndData[4*i+3] = i;
            }

            if(i%count<(count-1) && int(i/count)<(count-1)){
                gridSolid_IndData[6*i] = i;
                gridSolid_IndData[6*i+1] = i+1;
                gridSolid_IndData[6*i+2] = i+count;
                gridSolid_IndData[6*i+3] = i+count;
                gridSolid_IndData[6*i+4] = i+count+1;
                gridSolid_IndData[6*i+5] = i+1;
            }else{
                gridSolid_IndData[6*i] = 0;
                gridSolid_IndData[6*i+1] = 0;
                gridSolid_IndData[6*i+2] = 0;
                gridSolid_IndData[6*i+3] = 0;
                gridSolid_IndData[6*i+4] = 0;
                gridSolid_IndData[6*i+5] = 0;
            }
        }

        glBindVertexArray(0); // to avoid changing an incorrect VA binded IB
        glBindBuffer(GL_ARRAY_BUFFER, VBid);
        glBufferData(GL_ARRAY_BUFFER, 3*countSqrd*sizeof(float), grid_PosData, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireIBid);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, wireIndCount*sizeof(unsigned int), gridWire_IndData, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidIBid);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, solidIndCount*sizeof(unsigned int), gridSolid_IndData, GL_DYNAMIC_DRAW);

        delete[] grid_PosData;
        delete[] gridWire_IndData;
        delete[] gridSolid_IndData;
    }
public:
    std::string prepFunc;
    std::string prepColorFuncR;
    std::string prepColorFuncG;
    std::string prepColorFuncB;

    std::string prepWireColorFuncR;
    std::string prepWireColorFuncG;
    std::string prepWireColorFuncB;

    bool showSolid = true, showWireframe = false;
    float a, b, t;

    Function3D(const std::string iFunc, 
               const std::string iColorFuncR, const std::string iColorFuncG, const std::string iColorFuncB,
               const std::string iWireColorFuncR, const std::string iWireColorFuncG, const std::string iWireColorFuncB,
               int iSize, unsigned int iCount, float ia, float ib):
        size(iSize),
        count(iCount),
        a(ia),
        b(ib),

        prepFunc(iFunc),
        prepColorFuncR(iColorFuncR),
        prepColorFuncG(iColorFuncG),
        prepColorFuncB(iColorFuncB),

        prepWireColorFuncR(iWireColorFuncR),
        prepWireColorFuncG(iWireColorFuncG),
        prepWireColorFuncB(iWireColorFuncB)
    {   
        // Generate Buffers/Arrays
        glGenVertexArrays(1, &wireVAid);
        glGenVertexArrays(1, &solidVAid);

        glGenBuffers(1, &VBid);
        glGenBuffers(1, &wireIBid);
        glGenBuffers(1, &solidIBid);

        // Set VAs structure and binded buffers 
        glBindVertexArray(solidVAid);
        glBindBuffer(GL_ARRAY_BUFFER, VBid);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, solidIBid);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(wireVAid);
        glBindBuffer(GL_ARRAY_BUFFER, VBid);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, wireIBid);
        glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, (void*)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);
        
        // Initialize function and grid
        updateFunc();
        updateGrid();
    }

    void updateFunc(){
        renderedFunc = prepFunc;
        renderedColorFuncR = prepColorFuncR;
        renderedColorFuncG = prepColorFuncG;
        renderedColorFuncB = prepColorFuncB;
        renderedWireColorFuncR = prepWireColorFuncR;
        renderedWireColorFuncG = prepWireColorFuncG;
        renderedWireColorFuncB = prepWireColorFuncB;

        std::string shaderSusts[3];
        shaderSusts[0] = "float z = "+renderedFunc+";";
        shaderSusts[1] = "if(isSolid==1) vcolor = vec3("+renderedColorFuncR+","+renderedColorFuncG+","+renderedColorFuncB+");";
        shaderSusts[2] = "else vcolor = vec3("+renderedWireColorFuncR+","+renderedWireColorFuncG+","+renderedWireColorFuncB+");";
        funcShader.setProgram(func3DShaderStr, SHADER_CODE, &shaderError, shaderSusts);
    }

    void draw(OrbitCamera& cam){
        t = Clock::getTime();
        
        if(!shaderError){
            funcShader.bind();
            funcShader.setUniform("MVP", cam.getProjMat()*cam.getViewMat());
            funcShader.setUniform("a", a);
            funcShader.setUniform("b", b);
            funcShader.setUniform("t", t);

            if (showWireframe){
                funcShader.setUniform("isSolid", 0);
                glBindVertexArray(wireVAid);
                glDrawElements(GL_LINES, wireIndCount, GL_UNSIGNED_INT, nullptr);
            }
            
            if (showSolid){
                funcShader.setUniform("isSolid", 1);
                glBindVertexArray(solidVAid);
                glDrawElements(GL_TRIANGLES, solidIndCount, GL_UNSIGNED_INT, nullptr);
            }
        }
    }

    void changeCount(int step){ if (step+int(count) >= 2){ count += step; updateGrid(); } }
    void changeSize(float step){ size += step; if(size<0){ size = 0; } updateGrid(); }

    float getSize(){ return size; }
    unsigned int getCount(){ return count; }

    std::string getRenderedFunc(){ return renderedFunc; }

    std::string getRenderedColorFuncR(){ return renderedColorFuncR; }
    std::string getRenderedColorFuncG(){ return renderedColorFuncG; }
    std::string getRenderedColorFuncB(){ return renderedColorFuncB; }

    std::string getRenderedWireColorFuncR(){ return renderedWireColorFuncR; }
    std::string getRenderedWireColorFuncG(){ return renderedWireColorFuncG; }
    std::string getRenderedWireColorFuncB(){ return renderedWireColorFuncB; }

    bool getError(){ return shaderError; }
    
    ~Function3D(){
        glDeleteVertexArrays(1, &wireVAid);
        glDeleteVertexArrays(1, &solidVAid);

        glDeleteBuffers(1, &VBid);
        glDeleteBuffers(1, &wireIBid);
        glDeleteBuffers(1, &solidIBid);
    }
};