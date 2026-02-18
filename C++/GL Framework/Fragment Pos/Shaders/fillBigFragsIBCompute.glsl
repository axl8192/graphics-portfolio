#version 450 core
layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;

layout(std430, binding = 2) buffer bigFragIndBuff {
    uint bigFragIndList[];
};

void main(){
    uint i = uint(gl_WorkGroupID.x+gl_WorkGroupID.y*gl_NumWorkGroups.x);

    bigFragIndList[i*6+0] = 4*i + 0;
    bigFragIndList[i*6+1] = 4*i + 1;
    bigFragIndList[i*6+2] = 4*i + 2;

    bigFragIndList[i*6+3] = 4*i + 2;
    bigFragIndList[i*6+4] = 4*i + 3;
    bigFragIndList[i*6+5] = 4*i + 0;
}