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