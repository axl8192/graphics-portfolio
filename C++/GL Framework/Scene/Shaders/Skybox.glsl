#shader vertex
#version 330 core

uniform mat4 MVP;
layout(location = 0) in vec4 position;

out vec3 cubemapDir;

void main(){  
    gl_Position = MVP*position;
    cubemapDir = position.xyz;
}  

#shader fragment
#version 330 core

layout(location = 0) out vec4 color;

uniform samplerCube textureSampler;
in vec3 cubemapDir;

void main(){
    color = texture(textureSampler, cubemapDir);
}