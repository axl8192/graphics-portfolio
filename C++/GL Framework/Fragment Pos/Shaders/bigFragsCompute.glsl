#version 450 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

uniform mat4 inverseViewMat;
uniform int winWidth;
uniform int winHeight;
uniform int upperCodedZLimit;
uniform float nearPlane;
uniform float farPlane;
uniform float tanAngOverTwo;

layout(binding = 0) uniform sampler2D textureSampler0;

layout(std430, binding = 0) buffer zBuffer{
    int zList[];
};

layout(std430, binding = 1) buffer bigFragVertBuffer{
    float bigFragVertList[];
};

void main(){

    //### Some Initial Variables ###//
    int i = int(gl_WorkGroupID.x+gl_WorkGroupID.y*gl_NumWorkGroups.x);
    float aspect = float(winWidth)/float(winHeight);
    
    //### Calculate View Space Position and Size ###//
    float zv = -(zList[i]/float(upperCodedZLimit) * (farPlane-nearPlane) + nearPlane);
    
    float xscreen = (i%winWidth)+0.5;
    float xndc = 2*xscreen/winWidth - 1;
    float xv = xndc*(-zv)*aspect*tanAngOverTwo;

    float yscreen = int(i/winWidth)+0.5;
    float yndc = 2*yscreen/winHeight - 1;
    float yv = yndc*(-zv)*tanAngOverTwo;

    float semiSize = (zv*tanAngOverTwo)/winHeight;
    
    //### Set World Pos and Color ###//
    vec4 worldPos[] = { 
                        inverseViewMat*vec4(xv-semiSize, yv-semiSize, zv, 1.0f), 
                        inverseViewMat*vec4(xv+semiSize, yv-semiSize, zv, 1.0f),
                        inverseViewMat*vec4(xv+semiSize, yv+semiSize, zv, 1.0f),
                        inverseViewMat*vec4(xv-semiSize, yv+semiSize, zv, 1.0f)
                      };

    float xWG = gl_WorkGroupID.x;
    float yWG = gl_WorkGroupID.y;
    vec4 color = texture(textureSampler0, vec2((xWG+0.5)/winWidth, (yWG+0.5)/winHeight));

    for(int reli = 0; reli < 4; reli++){
        bigFragVertList[6*4*i+6*reli+0] = worldPos[reli].x;
        bigFragVertList[6*4*i+6*reli+1] = worldPos[reli].y;
        bigFragVertList[6*4*i+6*reli+2] = worldPos[reli].z;

        bigFragVertList[6*4*i+6*reli+3] = color.r;
        bigFragVertList[6*4*i+6*reli+4] = color.g;
        bigFragVertList[6*4*i+6*reli+5] = color.b;
    }
    
}