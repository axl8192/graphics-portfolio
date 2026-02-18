#version 450

//## Outputs ##//
layout(location = 0) out vec4 color;  

//## Uniform Inputs ##//
uniform int win_width;
uniform int win_height;
uniform mat4 inv_VP_Mat;

uniform int listSize;
layout(std430, binding = 0) buffer pointList_buff{
    vec4 pointList[];
};
layout(std430, binding = 1) buffer colorList_buff{
    vec4 colorList[];
};

//## Distances ##//
float euclidean_distance(vec2 v1, vec2 v2){
    return length(v1-v2);
}

float manhattan_distance(vec2 v1, vec2 v2){
    return abs(v1.x-v2.x) + abs(v1.y-v2.y);
}

float chebyshev_distance(vec2 v1, vec2 v2){
    return max(abs(v1.x-v2.x), abs(v1.y-v2.y));
}

//## Distance Function Selection ##//
float distance_function(vec2 v1, vec2 v2){ return euclidean_distance(v1, v2); }

//## Main ##//
void main(){

    float x_wind = gl_FragCoord.x;
    float y_wind = gl_FragCoord.y;

    float x_norm = clamp(x_wind/win_width*2 - 1, -1, 1);
    float y_norm = clamp(y_wind/win_height*2 - 1, -1, 1);
    vec4 pos = inv_VP_Mat*vec4(x_norm, y_norm, 0, 1);

    vec4 min_color = vec4(0,0,0,0);
    float min_distance = 20000000;
    int min_index = 0;
    for(int i = 0; i<listSize; i++){
        
        float dist = distance_function(pointList[i].xy, pos.xy);
        if(dist < min_distance){
            min_distance = dist;
            min_color = colorList[i];
            min_index = i;
        }
    }

    float second_min_distance = 20000000;
    vec4 second_min_color = vec4(0,0,0,0);
    bool chosen_second_min = false;
    for(int i = 0; i<listSize; i++){
        if(i == min_index) { continue; }
        
        float dist = distance_function(pointList[i].xy, pos.xy);
        if(dist < second_min_distance){
            second_min_distance = dist;
            second_min_color = colorList[i];
            chosen_second_min = true;
        }
    }

    //if(chosen_second_min && abs(second_min_distance-min_distance)<=0.03){ min_color = vec4(1,1,1,1); }

    /*vec4 second_min_color = vec4(1,1,1,1);
    float second_min_distance = 20000000;
    bool chosen_second_min = false;
    for(int i = 0; i<listSize; i++){
        if(i == min_index) { continue; }
        
        float dist = length(pointList[i].xy-pos.xy);
        if(dist < second_min_distance && abs(dist-min_distance)<0.1){
            second_min_distance = dist;
            second_min_color = colorList[i];
            chosen_second_min = true;
        }
    }

    if(chosen_second_min){ min_color = (second_min_color+min_color)/2; }*/

    /*vec4 min_color = vec4(0,0,0,0);
    float min_distance = 20000000;
    int sampled = 0;
    for(int i = 0; i<listSize; i++){
        
        float dist = length(pointList[i].xy-pos.xy);
        if(dist < 3){
            min_distance = dist;
            min_color += colorList[i];
            sampled += 1;
        }
    }
    min_color /= sampled;*/

    /*vec4 min_color = vec4(0,0,0,0);
    for(int i = 0; i<listSize; i++){
        float dist = length(pointList[i].xy-pos.xy);
        min_color += 1/pow(dist, 2)*colorList[i];
    }
    min_color /= listSize;
    min_color.w = 1;*/

    color = min_color;
}  
