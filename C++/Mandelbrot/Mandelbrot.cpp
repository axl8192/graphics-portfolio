#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <chrono>
#include <math.h>

using namespace std;

struct ShaderSource{
    std::string vertex;
    std::string fragment;
};

static ShaderSource parseShader(const std::string& filePath){
    std::ifstream stream(filePath);
	
    enum class ShaderType{
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };
    
    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    
    while(getline(stream, line)){
        if (line.find("#shader") != std::string::npos){
            if(line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
			
        }
        else{
            ss[(int)type] << line << "\n";
        }
    }
    
    return {ss[0].str(), ss[1].str()};
}

static unsigned int compileShader(unsigned int type, const std::string& source){
    
    /* Compile shader */
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);
    
    /* Error check and return */
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    
    if(result==GL_FALSE){
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*) alloca(length*sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << (type==GL_VERTEX_SHADER? "Vertex": "Fragment") << " shader compilation failed!" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }
    
    return id;
}

static unsigned int createShader(const std::string& vertexString, const std::string& fragmentString){
    unsigned int program = glCreateProgram();
    unsigned int vShader = compileShader(GL_VERTEX_SHADER, vertexString);
    unsigned int fShader = compileShader(GL_FRAGMENT_SHADER, fragmentString);
    
    glAttachShader(program, vShader);
    glAttachShader(program, fShader);
    glLinkProgram(program);
    glValidateProgram(program);
    
    glDeleteShader(vShader);
    glDeleteShader(fShader);
    
    return program;
}

#define prec64 //Define to double precision

#ifdef prec64
long long cx;
long long cy;
unsigned long long unit;
#else
int cx;
int cy;
unsigned int unit;
#endif

int wind_x = 800;
int wind_y = 800;

void zoom(double step){
    cx = cx - (int)(step*(double)(wind_x/2 - cx)/unit);
    cy = cy - (int)(step*(double)(wind_y/2 - cy)/unit);
    unit += step;
}

int main(void){
    /* Initialize the library */
    if (!glfwInit()) return -1;
    
    /* Create a windowed mode window and its OpenGL context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    
    GLFWwindow* window = glfwCreateWindow(wind_x, wind_y, "Hello World", (0? glfwGetPrimaryMonitor(): NULL), NULL);
    if (!window){ glfwTerminate(); return -1; }
    
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    
    /* Init GLEW */
    if (glewInit() != GLEW_OK) cout << "Error" << endl;
    cout << "GL_VERSION: " << glGetString(GL_VERSION) << endl;
    
    /*Vertex Array Object*/
    unsigned int vaoID;
    glGenVertexArrays(1,&vaoID);
    glBindVertexArray(vaoID);
    
    /* Vertex Buffer */
    float positions[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f,  1.0f,
        -1.0f,  1.0f,
    };
    
    unsigned int vertBuffID;
    glGenBuffers(1, &vertBuffID);
    glBindBuffer(GL_ARRAY_BUFFER, vertBuffID);
    glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float), positions, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float)*2, 0);
    
    /* Index Buffer */
    unsigned int indices[] = {
        0,1,2,
        2,3,0,
    };
    
    unsigned int indBuffID;
    glGenBuffers(1, &indBuffID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indBuffID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    /* Shader */
#ifdef prec64
    ShaderSource shaderSource = parseShader("mandel64.glsl");
    cout << "Shader loaded: mandel64.glsl" << endl;
#else
    ShaderSource shaderSource = parseShader("mandel32.glsl");
    cout << "Shader loaded: mandel32.glsl" << endl;
#endif
    
    unsigned int shader = createShader(shaderSource.vertex, shaderSource.fragment);
    glUseProgram(shader);
    
    /*Main Vars*/
    cx = 600;
    cy = 400;
    unit = 250;
    unsigned int maxIters = 200;
    int cx_loc = glGetUniformLocation(shader, "u_cx");
    int cy_loc = glGetUniformLocation(shader, "u_cy");
    int unit_loc = glGetUniformLocation(shader, "u_unit");
    int maxIters_loc = glGetUniformLocation(shader, "u_maxIters");
    
    double mouseX, mouseY;
    double mouseXPrev, mouseYPrev;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    
    int camSpeed = 1;
    double dTime;
    
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        glfwSetTime(0.0);
        
        /* Controls */
        mouseXPrev = mouseX;
        mouseYPrev = mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        
        //Drag
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {
            cx += mouseX-mouseXPrev;
            cy -= mouseY-mouseYPrev;
        }
        
        //Move
        if (glfwGetKey(window,GLFW_KEY_LEFT)){
            cx += (int)(camSpeed*dTime*1000);
        }else if (glfwGetKey(window,GLFW_KEY_RIGHT)){
            cx -= (int)(camSpeed*dTime*1000);
        }
        
        if (glfwGetKey(window,GLFW_KEY_DOWN)){
            cy += (int)(camSpeed*dTime*1000);
        }else if (glfwGetKey(window,GLFW_KEY_UP)){
            cy -= (int)(camSpeed*dTime*1000);
        }
        
        //Zoom
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS){
            if (abs(cx) < 3e9) zoom(unit/10);
        }else if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && unit > 200){
            zoom(-(double)unit / 10); 
        }
        
        //Iterations
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && maxIters>1){
            maxIters -= 1;
        }else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
            maxIters += 1;
        }
        
        //Quit and restart
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){break;}
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
            cx = 600;
            cy = 400;
            unit = 250;
        }
        
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);
        
#ifdef prec64
        glUniform1i64ARB(cx_loc, cx);
        glUniform1i64ARB(cy_loc, cy);
        glUniform1ui64ARB(unit_loc, unit);
#else
        glUniform1i(cx_loc, cx);
        glUniform1i(cy_loc, cy);
        glUniform1ui(unit_loc, unit);
#endif
        glUniform1ui(maxIters_loc, maxIters);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        
        /* Poll for and process events */
        glfwPollEvents();
        
        /* Caption (Iters and FPS) */
        ostringstream caption;
        dTime = glfwGetTime();
        caption << "Iters: " << maxIters << " - FPS: " << 1/dTime;
        glfwSetWindowTitle(window, caption.str().c_str());
    }
    
    glDeleteProgram(shader);
    glfwTerminate();
    return 0;
}