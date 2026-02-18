#pragma once
#include "../_Framework/GLFramework.hpp"
#include <unordered_map>
#include <iterator>
#include <algorithm>

class ComputeShader
{
private:
    unsigned int rendererID = 0;
    std::unordered_map<const char*, int> uniformLocationCache;
    void createProgram(const std::string& filePath){
        // Get and Parse Vertex and Fragment Shader Codes
        std::ifstream inFileStream(filePath);
        if(!inFileStream.is_open()){cout << "Error opening shader file: " << filePath << endl; exit(-1);}

        std::stringstream outCodeStream;
        outCodeStream << inFileStream.rdbuf();

        // Create Program
        rendererID = glCreateProgram();
        unsigned int shaderID = glCreateShader(GL_COMPUTE_SHADER);

        std::string& stdstrsrc = outCodeStream.str();
        const char* src = stdstrsrc.c_str();
        glShaderSource(shaderID, 1, &src, nullptr);
        glCompileShader(shaderID);

        // Error check
        int result;
        glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);

        if(result==GL_FALSE){
            int length;
            glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
            char* message = (char*) alloca(length*sizeof(char));
            glGetShaderInfoLog(shaderID, length, &length, message);
            std::cout << "Compute shader compilation failed!" << std::endl;
            std::cout << "(Source: " << filePath.c_str() << ")" << std::endl;
            std::cout << message << std::endl;
            glDeleteShader(shaderID);
            exit(-1);
        }

        // Link shader and program
        glAttachShader(rendererID, shaderID);
        glLinkProgram(rendererID);
        glValidateProgram(rendererID);

        glDeleteShader(shaderID);
    }

    int getUniformLocation(const char* name){
        if(uniformLocationCache.find(name) != uniformLocationCache.end()){
            return uniformLocationCache[name];
        }
        
        int location = glGetUniformLocation(rendererID, name);
        uniformLocationCache[name] = location;
        
        return location;
    }
public:
    ComputeShader(){}
    ComputeShader(const std::string& str){
        createProgram(str);
    }
    ~ComputeShader(){
        glDeleteProgram(rendererID);
    }

    void setProgram(const std::string& str){
        if (glIsProgram(rendererID)) glDeleteProgram(rendererID);
        if(!uniformLocationCache.empty()) uniformLocationCache.clear();
        createProgram(str);
    }

    void bind(){
        glUseProgram(rendererID);
    }
    void unbind(){
        glUseProgram(0);
    }

    void setUniform(const char* name, glm::mat4 value){
        this->bind();
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
    }
    void setUniform(const char* name, int value){
        this->bind();
        glUniform1i(getUniformLocation(name), value);
    }
    void setUniform(const char* name, float value){
        this->bind();
        glUniform1f(getUniformLocation(name), value);
    }
    void setUniform(const char* name, glm::vec3 value){
        this->bind();
        glUniform3fv(getUniformLocation(name), 1, &value[0]);
    }
    void setUniform(const char* name, glm::vec4 value){
        this->bind();
        glUniform4fv(getUniformLocation(name), 1, &value[0]);
    }

    void run(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z){
        this->bind();
        glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    }
};