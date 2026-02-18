#pragma once
#include <ft2build.h>
#include <freetype/freetype.h>
#include <vector>
#include "Helpers/Helpers.hpp"
#include "LinkedList.hpp"
#include "Font.hpp"
using glm::vec2; using glm::vec3; using glm::vec4; using glm::mat4;

//### Variables ###//
int winWidth = 1280, winHeight = 720;
const char* winCaption = "Text";
GLFWwindow* mainWindow;

mat4 projMat;
unsigned int tabWidth = 4;
bool renderTabs = false;
std::vector<char> wordSeparatorList = {' ', '\n', '\t', '.', ',', ';', ':'}; // @TODO limitar size por motivos de optimizacion
int textFileSizeLimit = 20*pow(10, 6); //bytes

int fileTabHeight = 30;
vec4 textBackground = vec4(0,0,0,1);

vec4 fileTabUnselectedColor = vec4(0.4,0.4,0.4,1);
Shader* colorRectShader;
float barWidth = 20.0f;

void initFileTabGraphics(){
    colorRectShader = new Shader("Shaders/basicColorRect.glsl");
}
