#shader vertex
#version 400 core
layout(location = 0) in vec4 position;

void main(){
    gl_Position = position;
}

#shader fragment
#version 400 core
#extension GL_ARB_gpu_shader_int64 : enable

out vec4 color;  

uniform uint u_maxIters;
uniform int64_t u_cx;
uniform int64_t u_cy;
uniform uint64_t u_unit;

void main(){

    double ReZ = 0.0;
    double ImZ = 0.0;
    
    double ReC = (double(gl_FragCoord.x-0.5) - u_cx)/u_unit;
    double ImC = (double(gl_FragCoord.y-0.5) - u_cy)/u_unit;
    
    float i = 0;
    for(; i<u_maxIters; i++){
        double nReZ = (ReZ+ImZ)*(ReZ-ImZ) + ReC;
        double nImZ = 2*ReZ*ImZ + ImC;

        ReZ = nReZ;
        ImZ = nImZ;

        if (ReZ*ReZ+ImZ*ImZ >= pow(2,2)){
            break;
        }
    }
    
    float intensity = i/u_maxIters;
    color = vec4(intensity, intensity, intensity, 1.0);
    
}  
