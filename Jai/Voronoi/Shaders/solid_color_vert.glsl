#version 330 core
layout(location = 0) in vec4 position;
uniform mat4 VP_Mat;
uniform mat4 M_Mat;

void main(){  
    gl_Position = VP_Mat*M_Mat*position;  
}  
