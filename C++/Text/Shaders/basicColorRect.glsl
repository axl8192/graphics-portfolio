#shader vertex
#version 450 core

layout(location = 0) in vec2 vertPosition;

uniform mat4 projMat;

void main(){
    gl_Position = projMat*vec4(vertPosition, 0, 1);
}

#shader fragment
#version 450 core

out vec4 fragColor;
uniform vec4 color;

void main(){
    fragColor = color;
}
