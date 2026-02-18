#shader vertex
#version 450 core

layout(location = 0) in vec2 vertPosition;

uniform mat4 VP_mat;
uniform vec2 position;

void main(){
    gl_Position = VP_mat*vec4(vertPosition+position, 0, 1);
}

#shader fragment
#version 450 core

out vec4 fragColor;
uniform vec4 color;

void main(){
    fragColor = color;
}
