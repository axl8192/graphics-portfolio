#pragma once
#include "../_Framework/GLFramework.hpp"

static std::string defaultShaderStr = 
R"(

#shader vertex
#version 330 core

uniform mat4 MVP;
layout(location = 0) in vec4 position;


void main(){  
    gl_Position = MVP*position;
}  

#shader fragment
#version 330 core

uniform vec3 u_color;
out vec3 color;

void main(){
    color = u_color;
}

)";

static std::string func3DShaderStr = 
R"(

#shader vertex
#version 330 core

uniform mat4 MVP;
layout(location = 0) in vec4 position;

uniform int isSolid;
uniform float a;
uniform float b;
uniform float t;

out vec3 vcolor;

void main(){
    vec4 out_pos = position;

    float x = position.x;
    float y = position.y;

    #sust
    
    out_pos.z = z;
    gl_Position = MVP*out_pos;

    #sust //if(isSolid==1) vcolor = vec3(0.5*sin(z)+0.5, 0.5*cos(z)+0.5, 1);
    #sust //else vcolor = u_color;
}  

#shader fragment
#version 330 core

out vec3 color;
in vec3 vcolor;

void main(){
    color = vcolor;
}

)";

