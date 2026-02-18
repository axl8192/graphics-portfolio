#pragma once
#include "../_Framework/GLFramework.hpp"

const float pi = glm::radians(180.0f);

class FPCamera
{
private:
    glm::vec3 position;
    glm::vec3 direction;
    glm::mat4 viewMat;
    glm::mat4 projMat;
    
public:
    float sensibility; // Degrees/pixel
    float velocity; // Units/sec

    float angH;
    float angV;

    float fov = 65.0f;

    bool canMove;
    
    FPCamera(glm::vec3 iPosition=glm::vec3(4.0f,2.5f,7.0f), float iAngH=-117*pi/180, float iAngV=-18*pi/180, float iSensibility=0.004f, float iVelocity=4.0f){
        position = iPosition;
        sensibility = iSensibility;
        velocity = iVelocity;
        angH = iAngH;
        angV = iAngV;

        canMove = true;
        update();
        canMove = false;
    }

    void update(){
        if(!canMove) return;

        /*cout << "Pos: " << position.x << ", " << position.y << ", " << position.z << endl;
        cout << "Ang: " << angH*180.0f/pi << ", " << angV*180.0f/pi << endl;*/
        
        //### Inputs ###//
        float frameTime = Clock::getFrameTime();

        // Rotation mouse
        angH += sensibility*Input::getMouseRel(0); //(Nota: el factor tiempo se tiene en cuenta implicitamente, permitiendo un movimento estable independiente de los fps)
        angV -= sensibility*Input::getMouseRel(1);

        angV = glm::clamp(angV, -pi/2, pi/2);
        
        // Movement
        direction = glm::vec3(cos(angV)*cos(angH), sin(angV), cos(angV)*sin(angH));

        if(Input::getKey(GLFW_KEY_W)) position += velocity*direction*frameTime;
        if(Input::getKey(GLFW_KEY_S)) position -= velocity*direction*frameTime;
        if(Input::getKey(GLFW_KEY_D)) position += velocity*glm::vec3(cos(angH+pi/2), 0, sin(angH+pi/2))*frameTime;
        if(Input::getKey(GLFW_KEY_A)) position -= velocity*glm::vec3(cos(angH+pi/2), 0, sin(angH+pi/2))*frameTime;
        if(Input::getKey(GLFW_KEY_LEFT_SHIFT)) position += velocity*glm::vec3(0, 1, 0)*frameTime;
        if(Input::getKey(GLFW_KEY_LEFT_CONTROL)) position -= velocity*glm::vec3(0, 1, 0)*frameTime;

        //### Model and view matrices ###//
        glm::vec3 up = glm::vec3(cos(angV+pi/2)*cos(angH), sin(angV+pi/2), cos(angV+pi/2)*sin(angH));
        viewMat = glm::lookAt(position, position+direction, up);
        projMat = glm::perspective(glm::radians(fov), (float)Renderer::getWinWitdh()/(float)Renderer::getWinHeight(), 0.1f, 1000.0f);
    }

    glm::mat4 getViewMat(){ return viewMat; }
    glm::mat4 getProjMat(){ return projMat; }
    glm::vec3 getPosition(){ return position; }
};