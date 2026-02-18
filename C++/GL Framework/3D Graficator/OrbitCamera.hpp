#pragma once
#include "../_Framework/GLFramework.hpp"
#include "ShaderCodes.hpp"

void interpFloat(float& var, float obj, int smoothness){
    if (var != obj){
        float dif = obj-var;
        var += dif/smoothness;

        if (fabs(dif) <= 10e-4) var = obj;
    }
}

const float pi = glm::radians(180.0f);

class OrbitCamera
{
private:
    float r;

    glm::mat4 viewMat;
    glm::mat4 projMat;

    glm::vec3 position;

    // Pivot Mark
    Mesh solidPivotMesh;
    Mesh wirePivotMesh;
    Shader pivotShader;

public:
    float keyRotVel; // Degrees/sec
    float keyZoomVel; // Units/sec
    float dragRotVel; // Degrees/pixel
    float wheelZoomVel; // Units/scroll
    float pivotVel; // Units/sec

    float r_raw;
    float angH;
    float angV;
    float pivotX, pivotY;

    bool pivotVisible = false;
    bool drawPivotWireframe = true;
    float fov = 65.0f;
    
    OrbitCamera(float iKeyRotVel=60, float iKeyZoomVel=5, float iDragRotVel=0.3, float iWheelZoomVel=1, float iPivotVel=20, 
                float iR=25, float iAngH=135.0f*pi/180, float iAngV=45.0f*pi/180):
        keyRotVel(iKeyRotVel),
        keyZoomVel(iKeyZoomVel),
        dragRotVel(iDragRotVel),
        wheelZoomVel(iWheelZoomVel),
        pivotVel(iPivotVel),

        r_raw(iR),
        r(iR),
        angH(iAngH),
        angV(iAngV),
        pivotX(0), pivotY(0)
    {   
        // Pivot Mark
        float h = 0.9;
        float sqrSide = 0.8;

        float pivotPos_Data[] = {
            0.0f, 0.0f, 0.0f,
            sqrSide/2, sqrSide/2, h,
            -sqrSide/2, sqrSide/2, h,
            -sqrSide/2, -sqrSide/2, h,
            sqrSide/2, -sqrSide/2, h
        };

        unsigned int pivotWireInd_Data[] = {
            0,1,
            0,2,
            0,3,
            0,4,

            1,2,
            2,3,
            3,4,
            4,1
        };

        unsigned int pivotSolidInd_Data[] = {
            0,1,2,
            0,2,3,
            0,3,4,
            0,4,1,

            1,2,3,
            3,4,1
        };

        wirePivotMesh.setData(pivotPos_Data, sizeof(pivotPos_Data), {3}, GL_LINES, pivotWireInd_Data, sizeof(pivotWireInd_Data));
        solidPivotMesh.setData(pivotPos_Data, sizeof(pivotPos_Data), {3}, GL_TRIANGLES, pivotSolidInd_Data, sizeof(pivotSolidInd_Data));
        pivotShader.setProgram(defaultShaderStr, SHADER_CODE);
    }
    void update(){
        // Interpolate Zoom
        interpFloat(r, r_raw, 5);

        // Calculate viewMat and projMat
        projMat = glm::perspective(glm::radians(fov), (float)Renderer::getWinWitdh()/(float)Renderer::getWinHeight(), 0.1f, 1000.0f);

        glm::vec3 pivotPos(pivotX, pivotY, 0);
        position = glm::vec3(r*cos(angV)*sin(angH)+pivotPos.x, r*cos(angV)*cos(angH)+pivotPos.y, r*sin(angV));
        glm::vec3 cameraUp(cos(angV+pi/2)*sin(angH), cos(angV+pi/2)*cos(angH), sin(angV+pi/2));
        viewMat = glm::lookAt(position, pivotPos, cameraUp);

        // Show Pivot Indicator
        if(pivotVisible){
            pivotShader.setUniform("MVP", projMat*viewMat*glm::translate(glm::mat4(1.0f), glm::vec3(pivotX, pivotY, 0.0f)));
            pivotShader.setUniform("u_color", glm::vec3(1.0f,0.0f,0.0f));
            
            solidPivotMesh.bind();
            Renderer::draw(solidPivotMesh.drawData);

            if(drawPivotWireframe){
                pivotShader.setUniform("u_color", glm::vec3(1.0f,1.0f,1.0f));
                wirePivotMesh.bind();
                Renderer::draw(wirePivotMesh.drawData);
            }
        }
    }

    glm::mat4 getViewMat(){ return viewMat; }
    glm::mat4 getProjMat(){ return projMat; }
    glm::vec3 getPosition(){ return position; }
};