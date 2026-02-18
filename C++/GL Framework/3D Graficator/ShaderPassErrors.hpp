#pragma once
#include "../_Framework/GLFramework.hpp"
#include <unordered_map>

struct ShaderSource{
    std::string vertex;
    std::string fragment;
};

class ShaderPassErrors
{
private:
    unsigned int rendererID = 0;
    std::unordered_map<const char*, int> uniformLocationCache;

    ShaderSource parseShaderFile(const std::string& filePath, std::string sustList[]){
        std::ifstream stream(filePath);

        enum class ShaderType{
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };
        std::string line;
        std::stringstream ss[2];
        ShaderType type = ShaderType::NONE;
        
        int sust_n = 0;
        while(getline(stream, line)){
            if (line.find("#shader") != std::string::npos){
                if(line.find("vertex") != std::string::npos)
                    type = ShaderType::VERTEX;
                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::FRAGMENT;
            }
            else if(line.find("#sust") != std::string::npos){
                ss[(int)type] << sustList[sust_n] << "\n";
                sust_n += 1;
            }
            else{
                ss[(int)type] << line << "\n";
            }
        }

        return {ss[0].str(), ss[1].str()};
    }
    ShaderSource parseShaderCode(const std::string& code, std::string sustList[]){
        enum class ShaderType{
            NONE = -1, VERTEX = 0, FRAGMENT = 1
        };
        std::stringstream inCodeStream(code);
        std::string line;
        std::stringstream outCodeStreams[2];
        ShaderType type = ShaderType::NONE;
        
        int sust_n = 0;
        while(getline(inCodeStream, line)){
            if (line.find("#shader") != std::string::npos){
                if(line.find("vertex") != std::string::npos)
                    type = ShaderType::VERTEX;
                else if (line.find("fragment") != std::string::npos)
                    type = ShaderType::FRAGMENT;
            }
            else if(line.find("#sust") != std::string::npos){
                outCodeStreams[(int)type] << sustList[sust_n] << "\n";
                sust_n += 1;
            }
            else{
                outCodeStreams[(int)type] << line << "\n";
            }
        }

        return {outCodeStreams[0].str(), outCodeStreams[1].str()};
    }
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
            std::cout << message << std::endl;
            glDeleteShader(id);
            return 0;
        }

        return id;
    }
    bool createProgram(ShaderSource shaderSource){
        this->rendererID = glCreateProgram();
        
        unsigned int vShader = this->compileShader(GL_VERTEX_SHADER, shaderSource.vertex);
        unsigned int fShader = this->compileShader(GL_FRAGMENT_SHADER, shaderSource.fragment);

        if (vShader==0 || fShader==0){return true;}

        glAttachShader(this->rendererID, vShader);
        glAttachShader(this->rendererID, fShader);
        glLinkProgram(this->rendererID);
        glValidateProgram(this->rendererID);

        glDeleteShader(vShader);
        glDeleteShader(fShader);

        return false;
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
    ShaderPassErrors(){}
    ShaderPassErrors(const std::string& str, ShaderStringType strType = SHADER_FILEPATH, bool* error = nullptr, std::string sustList[] = {}){
        
        ShaderSource shaderSource;
        if (strType == SHADER_FILEPATH) shaderSource = parseShaderFile(str, sustList);
        else shaderSource = parseShaderCode(str, sustList);

        if (error != nullptr) *error = createProgram(shaderSource);
        else createProgram(shaderSource);
    }
    ~ShaderPassErrors(){
        glDeleteProgram(rendererID);
    }

    void bind(){
        glUseProgram(rendererID);
    }
    void unbind(){
        glUseProgram(0);
    }

    void setProgram(const std::string& str, ShaderStringType strType = SHADER_FILEPATH, bool* error = nullptr, std::string sustList[] = {}){
        
        ShaderSource shaderSource;
        if (strType == SHADER_FILEPATH) shaderSource = parseShaderFile(str, sustList);
        else shaderSource = parseShaderCode(str, sustList);

        if (glIsProgram(rendererID)) glDeleteProgram(rendererID);
        if(!uniformLocationCache.empty()) uniformLocationCache.clear();
        if (error != nullptr) *error = createProgram(shaderSource);
        else createProgram(shaderSource);
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
};
