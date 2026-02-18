#pragma once
#include "Common/Common.hpp"
#include <glm/glm.hpp>
#include <unordered_map>
#include <iterator>
#include <algorithm>

enum ShaderStringType{
    SHADER_FILEPATH, SHADER_CODE
};

class Shader
{
private:
    unsigned int rendererID = 0;
    std::unordered_map<const char*, int> uniformLocationCache;
    std::string dbgFilepath;
    
    unsigned int compileShader(unsigned int type, const std::string& source){
        
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
            std::cout << "(Source: " << dbgFilepath.c_str() << ")" << std::endl;
            std::cout << message << std::endl;
            glDeleteShader(id);
            exit(-1);
        }

        return id;
    }
    void createProgram(const std::string& str, ShaderStringType strType){
        
        // Get and Parse Vertex and Fragment Shader Codes
        std::stringstream inCodeStream;
        if(strType == SHADER_FILEPATH){
            std::ifstream inFileStream(str);
            if(!inFileStream.is_open()){cout << "Error opening shader file: " << str << endl; exit(-1);}
            dbgFilepath = str;
            inCodeStream << inFileStream.rdbuf();
        }
        else{
            inCodeStream << str;
        }

        enum class ShaderType{ NONE = -1, VERTEX = 0, FRAGMENT = 1 };
        
        std::string line;
        std::stringstream outCodeStreams[2];
        ShaderType type = ShaderType::NONE;

        while(getline(inCodeStream, line)){
            if (line.find("#shader") != std::string::npos){
                if(line.find("vertex") != std::string::npos)
                    type = ShaderType::VERTEX;
                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::FRAGMENT;
            }
            else if(type != ShaderType::NONE){
                outCodeStreams[(int)type] << line << "\n";
            }
        }

        // Create Program
        rendererID = glCreateProgram();
        
        unsigned int vShader = compileShader(GL_VERTEX_SHADER, outCodeStreams[0].str());
        unsigned int fShader = compileShader(GL_FRAGMENT_SHADER, outCodeStreams[1].str());

        glAttachShader(rendererID, vShader);
        glAttachShader(rendererID, fShader);
        glLinkProgram(rendererID);
        glValidateProgram(rendererID);

        glDeleteShader(vShader);
        glDeleteShader(fShader);
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
    Shader(){}
    Shader(const std::string& str, ShaderStringType strType = SHADER_FILEPATH){
        createProgram(str, strType);
    }
    ~Shader(){
        glDeleteProgram(rendererID);
    }

    void setProgram(const std::string& str, ShaderStringType strType = SHADER_FILEPATH){
        if (glIsProgram(rendererID)) glDeleteProgram(rendererID);
        if(!uniformLocationCache.empty()) uniformLocationCache.clear();
        createProgram(str, strType);
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
    void setUniform(const char* name, glm::vec2 value){
        this->bind();
        glUniform2fv(getUniformLocation(name), 1, &value[0]);
    }
};
