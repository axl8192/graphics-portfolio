#pragma once
#include "../_Framework/GLFramework.hpp"
#include "OrbitCamera.hpp"
#include "ShaderCodes.hpp"

class Axes
{
private:
    Mesh arrowSolidMesh;
    Mesh arrowWireMesh;
    Mesh centerMesh;
    Mesh axesLinesMesh;
    Shader axisShader;

    glm::mat4 xAxis_ModelMat;
    glm::mat4 yAxis_ModelMat;
    glm::mat4 zAxis_ModelMat;

public:
    bool showArrows = true;
    bool showLines = false;
    bool showArrowWireframe = false;

    Axes(){
        // Axis Arrow
        float axisWidth = 0.025;
        float arrowLength = 0.1;
        float arrowRelWidth = 2.0f;

        float axisPos_Data[] = {
            -axisWidth/2, axisWidth/2, 0,
            axisWidth/2, axisWidth/2, 0,
            axisWidth/2, -axisWidth/2, 0,
            -axisWidth/2, -axisWidth/2, 0,

            -axisWidth/2, axisWidth/2, 1-axisWidth/2-arrowLength,
            axisWidth/2, axisWidth/2, 1-axisWidth/2-arrowLength,
            axisWidth/2, -axisWidth/2, 1-axisWidth/2-arrowLength,
            -axisWidth/2, -axisWidth/2, 1-axisWidth/2-arrowLength,

            -arrowRelWidth*axisWidth/2, arrowRelWidth*axisWidth/2, 1-axisWidth/2-arrowLength,
            arrowRelWidth*axisWidth/2, arrowRelWidth*axisWidth/2, 1-axisWidth/2-arrowLength,
            arrowRelWidth*axisWidth/2, -arrowRelWidth*axisWidth/2, 1-axisWidth/2-arrowLength,
            -arrowRelWidth*axisWidth/2, -arrowRelWidth*axisWidth/2, 1-axisWidth/2-arrowLength,

            0,0,1,
        };

        unsigned int axisInd_Data[] = {
                1,2,5,
                5,6,2,

                0,3,7,
                7,4,0,

                7,6,3,
                3,2,6,

                0,1,5,
                5,0,4,

                8,10,9,
                8,10,11,

                8,12,9,
                9,12,10,
                10,12,11,
                11,12,8,
        };

        unsigned int axisWireInd_Data[] = {
            0,1,
            1,2,
            2,3,
            3,0,

            0,4,
            1,5,
            2,6,
            3,7,

            8,9,
            9,10,
            10,11,
            11,8,

            8,12,
            9,12,
            10,12,
            11,12
        };

        // XYZ Center
        float xyzCenterPos_Data[] = {
            -axisWidth/2, axisWidth/2, axisWidth/2,
            axisWidth/2, axisWidth/2, axisWidth/2,
            axisWidth/2, -axisWidth/2, axisWidth/2,
            -axisWidth/2, -axisWidth/2, axisWidth/2,

            -axisWidth/2, axisWidth/2, -axisWidth/2,
            axisWidth/2, axisWidth/2, -axisWidth/2,
            axisWidth/2, -axisWidth/2, -axisWidth/2,
            -axisWidth/2, -axisWidth/2, -axisWidth/2,
        };

        unsigned int xyzCenterInd_Data[] = {
                0,1,3,
                3,2,1,

                1,2,5,
                5,6,2,

                4,7,5,
                5,6,7,

                0,3,7,
                7,4,0,

                7,6,3,
                3,2,6,

                0,6,1,
                1,6,4,
        };

        // Axes Lines
        float axesLinesLimit = 20.0f;

        float axesLinesPos_Data[] = {
            axesLinesLimit, 0.0f, 0.0f,
            -axesLinesLimit, 0.0f, 0.0f,

            0.0f, axesLinesLimit, 0.0f,
            0.0f, -axesLinesLimit, 0.0f,

            0.0f, 0.0f, axesLinesLimit,
            0.0f, 0.0f, -axesLinesLimit,
        };

        unsigned int axesLinesInd_data[] = {
            0,1,
            2,3,
            4,5,
        };

        arrowSolidMesh.setData(axisPos_Data, sizeof(axisPos_Data), {3}, axisInd_Data, sizeof(axisInd_Data));
        arrowWireMesh.setData(axisPos_Data, sizeof(axisPos_Data), {3}, GL_LINES, axisWireInd_Data, sizeof(axisWireInd_Data));
        centerMesh.setData(xyzCenterPos_Data, sizeof(xyzCenterPos_Data), {3}, xyzCenterInd_Data, sizeof(xyzCenterInd_Data));
        axesLinesMesh.setData(axesLinesPos_Data, sizeof(axesLinesPos_Data), {3}, GL_LINES, axesLinesInd_data, sizeof(axesLinesInd_data));
        
        axisShader.setProgram(defaultShaderStr, SHADER_CODE);

        xAxis_ModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(axisWidth/2, 0, 0))*
                         glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0,1,0));
        yAxis_ModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, axisWidth/2, 0))*
                         glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1,0,0));
        zAxis_ModelMat = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, axisWidth/2));
    }

    void draw(OrbitCamera& cam){
        //### MVP ###//
        glm::mat4 viewMat = cam.getViewMat();
        glm::mat4 projMat = cam.getProjMat();

        glm::mat4 x_MVP = projMat*viewMat*xAxis_ModelMat;
        glm::mat4 y_MVP = projMat*viewMat*yAxis_ModelMat;
        glm::mat4 z_MVP = projMat*viewMat*zAxis_ModelMat;
        glm::mat4 c_MVP = projMat*viewMat;

        axisShader.bind();

        //### Render Lines ###//
        if(showLines){
            axisShader.setUniform("u_color", glm::vec3(1.0f, 1.0f, 1.0f));
            axisShader.setUniform("MVP", c_MVP);
            axesLinesMesh.bind();
            Renderer::draw(axesLinesMesh.drawData);
        }

        //### Render Arrows ###//
        if(showArrows){
            
            //Solid
            arrowSolidMesh.bind();
            
            axisShader.setUniform("MVP", x_MVP);
            axisShader.setUniform("u_color", glm::vec3(1.0f, 0.0f, 0.0f));
            Renderer::draw(arrowSolidMesh.drawData);

            axisShader.setUniform("MVP", y_MVP);
            axisShader.setUniform("u_color", glm::vec3(0.0f, 1.0f, 0.0f));
            Renderer::draw(arrowSolidMesh.drawData);

            axisShader.setUniform("MVP", z_MVP);
            axisShader.setUniform("u_color", glm::vec3(0.0f, 0.0f, 1.0f));
            Renderer::draw(arrowSolidMesh.drawData);
            
            //Wire
            axisShader.setUniform("u_color", glm::vec3(1.0f, 1.0f, 1.0f));
            
            centerMesh.bind();
            axisShader.setUniform("MVP", c_MVP);
            Renderer::draw(centerMesh.drawData); 

            if(showArrowWireframe){
                arrowWireMesh.bind();
            
                axisShader.setUniform("MVP", x_MVP);
                Renderer::draw(arrowWireMesh.drawData);
        
                axisShader.setUniform("MVP", y_MVP);
                Renderer::draw(arrowWireMesh.drawData);

                axisShader.setUniform("MVP", z_MVP);
                Renderer::draw(arrowWireMesh.drawData);     
            }
        }
    }
};