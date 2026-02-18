#shader vertex
#version 450 core

layout(location = 0) in vec2 vertPosition;
layout(location = 1) in vec2 vertUV;
out vec2 UV;
out vec4 color;

uniform mat4 MVP;

layout(std430, binding = 0) buffer dUVBuff{
    vec2 dUVList[];
};

layout(std430, binding = 1) buffer dPosBuff{
    vec2 dPosList[];
};

layout(std430, binding = 2) buffer colorBuff{
    vec4 colorList[];
};

void main(){
    gl_Position = MVP*vec4(vertPosition+dPosList[gl_InstanceID], 0, 1);
    UV = vertUV+dUVList[gl_InstanceID];
    color = colorList[gl_InstanceID];
}

#shader fragment
#version 450 core

in vec2 UV;
in vec4 color;
out vec4 fragColor;

layout(binding = 0) uniform sampler2D sampler0;

void main(){
    fragColor = vec4(1,1,1,texture(sampler0, UV).r)*color;
}
