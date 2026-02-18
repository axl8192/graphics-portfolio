#shader vertex
#version 450 core

layout(location = 0) in vec3 positionIn;
layout(location = 1) in vec2 UVin;

uniform mat4 modelViewMat;
uniform mat4 projMat;

uniform float nearPlane;
uniform float farPlane;
uniform int upperCodedZLimit;

out vec2 UV;
out float zIntCoded;

void main(){
    vec4 viewPosition = modelViewMat*vec4(positionIn, 1.0);
    zIntCoded = (-viewPosition.z-nearPlane)/(farPlane-nearPlane) * upperCodedZLimit;
    gl_Position = projMat*viewPosition;
    UV = UVin;
}

#shader fragment
#version 450 core

in vec2 UV;
in float zIntCoded;
layout(binding = 0) uniform sampler2D textureSampler0;
uniform int winWidth;
layout(location = 0) out vec4 fragColor;

layout(std430, binding = 0) buffer zBuffer{
    int zList[];
};

void main(){
    fragColor = texture(textureSampler0, UV);
    atomicMin(zList[int(gl_FragCoord.x) + int(gl_FragCoord.y)*winWidth], int(zIntCoded));
}