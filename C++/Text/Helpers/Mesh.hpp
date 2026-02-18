#pragma once
#include "Common/Common.hpp"

struct DrawData{
    int vertexCount;
    GLenum drawMode = GL_TRIANGLES;
    bool usesIB;
};

class Mesh{
private:
    unsigned int VAid = 0;
    unsigned int VBid = 0;
    unsigned int IBid = 0;

    DrawData p_drawData;

    void processObjVLine(std::string& line, std::vector<float>& vec, int initial_index){
        std::string numStr;
        for (int i=initial_index; i<line.size(); i++)
        {   
            if(line[i]!=' ' && line[i]!='\n'){
                numStr = numStr + line[i];
            }else{
                vec.push_back(std::stof(numStr));
                numStr.clear();
            }
        }
    }
public:
    const DrawData& drawData = p_drawData;

    Mesh(){}

    Mesh(float* vertexData, int sizeofVertexData, std::initializer_list<int> componentList, unsigned int* indexData = nullptr, int sizeofIndexData = 0){
        setData(vertexData, sizeofVertexData, componentList, indexData, sizeofIndexData);
    }

    Mesh(float* vertexData, int sizeofVertexData, std::initializer_list<int> componentList, GLenum drawMode, unsigned int* indexData = nullptr, int sizeofIndexData = 0){
        setData(vertexData, sizeofVertexData, componentList, drawMode, indexData, sizeofIndexData);
    }

    Mesh(std::string path, bool loadUVs = true, bool loadNormals = true){
        loadDataFromObj(path, loadUVs, loadNormals);
    }
    
    void setData(float* vertexData, int sizeofVertexData, std::initializer_list<int> componentList, unsigned int* indexData = nullptr, int sizeofIndexData = 0){
        
        if(VAid==0) glGenVertexArrays(1, &VAid);
        glBindVertexArray(VAid);
        
        if(VBid==0) glGenBuffers(1, &VBid);
        glBindBuffer(GL_ARRAY_BUFFER, VBid);
        glBufferData(GL_ARRAY_BUFFER, sizeofVertexData, vertexData, GL_STATIC_DRAW);

        int stride = 0;
        for(int component: componentList){
            stride += component*sizeof(float);
        }

        int offset = 0;
        int index = 0;
        for(int component: componentList){
            glVertexAttribPointer(index, component, GL_FLOAT, false, stride, (void*)offset);
            glEnableVertexAttribArray(index);
            offset += component*sizeof(float);
            index++;
        }

        if(indexData != nullptr){
            if(IBid==0) glGenBuffers(1, &IBid);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBid);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeofIndexData, indexData, GL_STATIC_DRAW);
            p_drawData.vertexCount = sizeofIndexData/sizeof(unsigned int);
            p_drawData.usesIB = true;
        }else{
            p_drawData.vertexCount = sizeofVertexData/offset;
            p_drawData.usesIB = false;
            if(IBid!=0) glDeleteBuffers(1, &IBid);
        }

        glBindVertexArray(0);
    }

    void setData(float* vertexData, int sizeofVertexData, std::initializer_list<int> componentList, GLenum drawMode, unsigned int* indexData = nullptr, int sizeofIndexData = 0){
        p_drawData.drawMode = drawMode;
        setData(vertexData, sizeofVertexData, componentList, indexData, sizeofIndexData);
    }

    void bind(){
        glBindVertexArray(VAid);
    }

    void loadDataFromObj(std::string path, bool loadUVs = true, bool loadNormals = true){
        std::vector<float> positionsTmp;
        std::vector<float> UVsTmp;
        std::vector<float> normalsTmp;

        std::vector<float> vertOut;

        std::ifstream objFile(path);
        if(!objFile.is_open()){ std::cout << "Error al cargar "+path << std::endl; return; }

        std::string line;
        while(getline(objFile, line)){
            line.append("\n");
            if(line[0]=='v' && line[1]==' '){
                processObjVLine(line, positionsTmp, 2);
            } else if(line[0]=='v' && line[1]=='t'){
                processObjVLine(line, UVsTmp, 3);
            } else if(line[0]=='v' && line[1]=='n'){
                processObjVLine(line, normalsTmp, 3);
            } 
            
            else if(line[0]=='f' && line[1]==' '){
                std::string numStr = "";
                int numLoading = 0;
                for (int i=2; i<line.size(); i++){
                    if(line[i]!='/' && line[i]!=' ' && line[i]!='\n'){
                        numStr = numStr + line[i];
                    }else{
                        if(numStr!=""){
                            int index = std::stoi(numStr)-1;

                            if(numLoading==0){
                                vertOut.push_back(positionsTmp[3*index+0]);
                                vertOut.push_back(positionsTmp[3*index+1]);
                                vertOut.push_back(positionsTmp[3*index+2]);
                            } else if(numLoading==1 && loadUVs){
                                vertOut.push_back(UVsTmp[2*index+0]);
                                vertOut.push_back(UVsTmp[2*index+1]);
                            } else if(numLoading==2 && loadNormals){
                                vertOut.push_back(normalsTmp[3*index+0]);
                                vertOut.push_back(normalsTmp[3*index+1]);
                                vertOut.push_back(normalsTmp[3*index+2]);
                            }

                            numStr.clear();
                        }
                        
                        if(line[i]=='/'){
                            numLoading = (numLoading+1)%3;
                        }else if(line[i]==' '){
                            numLoading = 0;
                        }
                    }
                }
            }
        }

        std::initializer_list<int> componentList;

        if(!loadUVs && !loadNormals){
            componentList = {3};
        }else if(loadUVs && !loadNormals){
            componentList = {3, 2};
        }else if(!loadUVs && loadNormals){
            componentList = {3, 3};
        }else{
            componentList = {3, 2, 3};
        }
        
        setData(vertOut.data(), vertOut.size()*sizeof(float), componentList);
    }

    ~Mesh(){
        glDeleteVertexArrays(1, &VAid);
        glDeleteBuffers(1, &VBid);
        glDeleteBuffers(1, &IBid);
    }
};
