#shader vertex
#version 450 core

layout(location = 0) in vec2 vertPosition;
layout(location = 1) in vec2 vertUV;
out vec2 UV;

uniform mat4 MVP;

void main(){
    gl_Position = MVP*vec4(vertPosition, 0, 1);
    UV = vertUV;
}

#shader fragment
#version 450 core

in vec2 UV;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D sampler0;

void main(){
    fragColor = vec4(texture(sampler0, UV).r);
}
