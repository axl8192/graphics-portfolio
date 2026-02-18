#shader vertex
#version 420 core

uniform mat4 MVP;
layout(location = 0) in vec3 positionIn;
layout(location = 1) in vec2 UVin;
layout(location = 2) in vec3 normalIn;

out vec3 position;
out vec2 UV;
out vec3 normalRaw;

uniform mat4 objModelMat;

void main(){
    gl_Position = MVP*vec4(positionIn, 1.0);

    position = vec3(objModelMat*vec4(positionIn,1));
    UV = UVin;
    normalRaw =  mat3(transpose(inverse(objModelMat)))*normalIn;
}

#shader fragment
#version 420 core

layout(binding = 0) uniform sampler2D textureSampler0;
in vec3 position;
in vec2 UV;
in vec3 normalRaw;

uniform vec3 cameraPos;

uniform vec3 directionalColor;
uniform vec3 directionalDirection;
uniform float directionalAmbient;

uniform vec3 pointPos;
uniform vec3 pointColor;
uniform float pointIntParamA;
uniform float pointIntParamB;

uniform vec3 spotPos;
uniform vec3 spotColor;
uniform float spotIntParamA;
uniform float spotIntParamB;
uniform vec3 rawSpotDir;
uniform float rawSpotInAng;
uniform float rawSpotOutAng;

layout(location = 0) out vec4 fragColor;

vec3 directionalLight(){
    float ambient = directionalAmbient;
    
    float diffuseMult = 0.1;
    vec3 normal = normalize(normalRaw);
    vec3 lightDir = normalize(-directionalDirection);
    float diffuse = max(dot(lightDir, normal), 0)*diffuseMult;

    float specularMult = 0.2;
    vec3 viewDir = normalize(cameraPos-position);
    vec3 reflectionDir = reflect(-lightDir, normal);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0), 8);
    float specular = specAmount*specularMult;
    
    return (ambient+diffuse+specular)*directionalColor;
}

vec3 pointLight(){
    float diffuseMult = 1.0;
    vec3 normal = normalize(normalRaw);
    vec3 lightDir = normalize(pointPos-position);
    float diffuse = max(dot(lightDir, normal), 0)*diffuseMult;

    float specularMult = 0.5;
    vec3 viewDir = normalize(cameraPos-position);
    vec3 reflectionDir = reflect(-lightDir, normal);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0), 8);
    float specular = specAmount*specularMult;

    float lightDist = distance(pointPos, position);
    float a = pointIntParamA;
    float b = pointIntParamB;
    float intensity = 1/(a*pow(lightDist,2)+b*lightDist+1);
    
    return intensity*(diffuse+specular)*pointColor;
}

vec3 spotLight(){
    float diffuseMult = 1.0;
    vec3 normal = normalize(normalRaw);
    vec3 lightDir = normalize(spotPos-position);
    float diffuse = max(dot(lightDir, normal), 0)*diffuseMult;

    float specularMult = 0.5;
    vec3 viewDir = normalize(cameraPos-position);
    vec3 reflectionDir = reflect(-lightDir, normal);
    float specAmount = pow(max(dot(viewDir, reflectionDir), 0), 8);
    float specular = specAmount*specularMult;

    float lightDist = distance(spotPos, position);
    float a = spotIntParamA;
    float b = spotIntParamB;
    float intensity = 1/(a*pow(lightDist,2)+b*lightDist+1);

    vec3 spotDir = normalize(rawSpotDir);
    float spotInAng = radians(rawSpotInAng);
    float spotOutAng = radians(rawSpotOutAng);
    float spotIntensity = clamp((dot(lightDir, -spotDir) - cos(spotOutAng))/(cos(spotInAng) - cos(spotOutAng)), 0.0, 1.0);    
    
    return spotIntensity*intensity*(diffuse+specular)*spotColor;
}

void main(){
    vec3 light = vec3(0,0,0);

    light += directionalLight();
    light += pointLight();
    light += spotLight();
    
    fragColor = texture(textureSampler0, UV)*vec4(light,1);
}