#shader vertex
#version 450 core

uniform mat4 viewProjMat;
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vColor;

out vec4 color;

void main(){
    gl_Position = viewProjMat*vec4(vPosition,1.0);
    color = vec4(vColor,1.0);
}  

#shader fragment
#version 450 core

in vec4 color;
out vec4 out_color;

void main(){
    out_color = color;
}