#shader vertex
#version 400 core
layout(location = 0) in vec4 position;

void main(){
    gl_Position = position;  
}

#shader fragment
#version 400 core

out vec4 color;  

uniform uint u_maxIters;
uniform int u_cx;
uniform int u_cy;
uniform uint u_unit;

void main(){
    float ReZ = 0.0;
    float ImZ = 0.0;
    
    float ReC = (float(gl_FragCoord.x-0.5) - u_cx)/u_unit;
    float ImC = (float(gl_FragCoord.y-0.5) - u_cy)/u_unit;
    
    float i = 0;
    for(; i<u_maxIters; i++){
        float nReZ = (ReZ+ImZ)*(ReZ-ImZ) + ReC;
        float nImZ = 2*ReZ*ImZ + ImC;

        ReZ = nReZ;
        ImZ = nImZ;

        if (ReZ*ReZ+ImZ*ImZ >= pow(2,2)){
            break;
        }
    }
    
    float intensity = i/u_maxIters;
    color = vec4(intensity, intensity, intensity, 1.0);
}  
