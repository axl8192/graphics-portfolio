#pragma once
#include "Globals.hpp"
#include <stdio.h>
#include <wchar.h>
#include "RectMesh.hpp"
#include <direct.h>

struct TextLL{
    LLNode* entry; // the entry represents no character; the chars begins in ->next; NEVER remove the entry;
    LLNode* ending; // the ending represents no character; NEVER remove the ending;

    char* filePath;
    char* fileName;

    int nLines = 1;

    TextLL(const char* iFilePath){
        LLCreate(&entry, &ending);

        // Get file name and set fileName
        int fileNameBeginingIndex = 0;
        for(int i = strlen(iFilePath)-1; i>=0; i--){
            if(iFilePath[i] == '/' || iFilePath[i] == '\\'){
                fileNameBeginingIndex = i+1;
                break;
            }
        }
        fileName = (char*)malloc((strlen(iFilePath)-fileNameBeginingIndex)+1);
        strcpy(fileName, &(iFilePath[fileNameBeginingIndex]));

        // Set full filePath
        bool isRelative = true;
        for(int i=0; i<strlen(iFilePath); i++){
            if(iFilePath[i] == ':'){
                isRelative = false;
                break;
            }
        }
        
        if(isRelative){
            char* cwdStr = (char*)malloc(256+1);
            cwdStr = getcwd(cwdStr, 256);

            filePath = (char*)malloc(strlen(cwdStr)+strlen(iFilePath)+1);
            strcpy(filePath, cwdStr);
            strcat(filePath, "\\");
            strcat(filePath, iFilePath);
            free(cwdStr);
        }else{
            filePath = (char*)malloc(strlen(iFilePath)+1);
            strcpy(filePath, iFilePath);
        }
        
        // Load file
        FILE* fileStream = fopen(filePath, "rb");
        if(fileStream == nullptr){cout << "Error opening: " << filePath << endl; exit(-1);}

        fseek(fileStream, 0, SEEK_END);
        int size = ftell(fileStream);
        if(size > textFileSizeLimit){cout << "Error: exceded file size limit on loading: " << filePath  <<  endl; exit(-1);}
        
        fseek(fileStream, 0, SEEK_SET);
        LLNode* currentNode = entry;
        while(true){
            int character = fgetc(fileStream);
            if (character == EOF){ break; }
            if (character == '\n'){ nLines += 1; }
            LLInsertNode(currentNode, (char)character);
            currentNode = currentNode->next;
        }
        
        fclose(fileStream);
    }

    ~TextLL(){
        free(filePath);
        free(fileName);

        LLNode* currentNode = entry->next;
        while(currentNode != ending){
            currentNode = currentNode->next;
            LLRemoveNode(currentNode->previous);
        }

        delete entry;
        delete ending;
    }
};

struct FileTextEditor{

    // Rect properties
    int rectWidth, rectHeight;
    int rectX, rectY;
    
    // TextLL
    TextLL* textLL;
    LLNode* firstShowingNode; // it's really the newLine or llEntry before the first char being rendered. @TODO change that fact? Doesn't change horizontally
    LLNode* cursorNode;

    //
    int firstLineShowing = 1;
    int firstColumnShowing = 1;
    int cursorLine = 1;
    int cursorColumn = 1;
    int practicalCursorColumn = cursorColumn; // The column value for movement purposes. Doesn't necessarially correspond with the actual cursor column
    
    int maxLinesToShow;
    int maxColumnsToShow;

    vec2* dUVList = nullptr;
    vec2* dPosList = nullptr;
    vec4* colorList = nullptr;
    unsigned int dUVBuffer;
    unsigned int dPosBuffer;
    unsigned int colorBuffer; // @TODO change this for an indexed color list? make it a char

    int maxLineNumberDigitCount;
    int maxLineNumber;
    int spacesAfterLineNumber;
    char* lineNumberStr = nullptr;
    char* lineNumberStrCorr = nullptr;

    // Frame specific variables
    int dx, dy;
    int glyphsToRender;

    // Editor tab
    unsigned int tabBgBgVA;
    unsigned int tabBgBgVB;
    unsigned int tabBgVA;
    unsigned int tabBgVB;

    vec2* tabDUVList = nullptr;
    vec2* tabDPosList = nullptr;
    vec4* tabColorList = nullptr;
    unsigned int tabDPosBuffer;
    unsigned int tabDUVBuffer;
    unsigned int tabColorBuffer;

    // Vertical scroll bar
    unsigned int vBarBgVA;
    unsigned int vBarBgVB;
    unsigned int vBarVA;
    unsigned int vBarVB;

    void setMaxLineNumberDigitCount(int count){
        if(lineNumberStr != nullptr) free(lineNumberStr);
        if(lineNumberStrCorr != nullptr) free(lineNumberStrCorr);
        
        maxLineNumberDigitCount = count;
        maxLineNumber = pow(10, maxLineNumberDigitCount)-1;
        spacesAfterLineNumber = 1;
        lineNumberStr = (char*)malloc(maxLineNumberDigitCount+1);
        lineNumberStrCorr = (char*)malloc(maxLineNumberDigitCount+spacesAfterLineNumber+1);
    }

    void setSize(int width, int height){
        rectWidth = width-barWidth;
        rectHeight = height;
        rectX = 0;
        rectY = fileTabHeight;
        
        if(dPosList!=nullptr) delete[] dPosList;
        if(dUVList!=nullptr) delete[] dUVList;
        if(colorList!=nullptr) delete[] colorList;
        glDeleteBuffers(1, &dUVBuffer);
        glDeleteBuffers(1, &dPosBuffer);
        glDeleteBuffers(1, &colorBuffer);
        
        maxColumnsToShow = (int)(rectWidth/fileTextEditorFont->glyphWidth) - (maxLineNumberDigitCount+spacesAfterLineNumber) + 1;
        maxLinesToShow = (int)(rectHeight/fileTextEditorFont->glyphHeight)+1;

        int bufferSize = maxLinesToShow*maxColumnsToShow + maxLinesToShow*(maxLineNumberDigitCount+spacesAfterLineNumber);
        
        glCreateBuffers(1, &dUVBuffer);
        glNamedBufferData(dUVBuffer, sizeof(vec2)*bufferSize, nullptr, GL_STATIC_DRAW);
        glCreateBuffers(1, &dPosBuffer);
        glNamedBufferData(dPosBuffer, sizeof(vec2)*bufferSize, nullptr, GL_STATIC_DRAW);
        glCreateBuffers(1, &colorBuffer);
        glNamedBufferData(colorBuffer, sizeof(vec4)*bufferSize, nullptr, GL_STATIC_DRAW);
        
        dPosList = new vec2[bufferSize];
        dUVList = new vec2[bufferSize];
        colorList = new vec4[bufferSize];

        float vTabBgBgData[] = {
            0.0,        0.0,
            0.0,        fileTabHeight,
            winWidth,   0.0,
            winWidth,   fileTabHeight,
        };
        glNamedBufferData(tabBgBgVB, sizeof(vTabBgBgData), vTabBgBgData, GL_STATIC_DRAW);

        float vBarBgVData[] = {
            rectWidth,      rectY,
            rectWidth,      rectY+rectHeight,
            rectWidth+barWidth, rectY,          
            rectWidth+barWidth, rectY+rectHeight,
        };
        glNamedBufferData(vBarBgVB, sizeof(vBarBgVData), vBarBgVData, GL_STATIC_DRAW);
    }

    FileTextEditor(TextLL* iTextLL){
        
        // Init textLL
        textLL = iTextLL;
        firstShowingNode = textLL->entry;
        cursorNode = textLL->entry->next;

        // Init scroll bar
        createRectMesh(&vBarBgVA, &vBarBgVB, nullptr, 2*4*sizeof(float), {2});
        createRectMesh(&vBarVA, &vBarVB, nullptr, 2*4*sizeof(float), {2});

        // Init editor tab
        createRectMesh(&tabBgVA, &tabBgVB, nullptr, 4*2*sizeof(float), {2});

        tabDPosList = new vec2[strlen(textLL->filePath)];
        tabDUVList = new vec2[strlen(textLL->filePath)];
        tabColorList = new vec4[strlen(textLL->filePath)];

        int filePathCharLen = strlen(textLL->filePath);
        glCreateBuffers(1, &tabDUVBuffer);
        glNamedBufferData(tabDUVBuffer, sizeof(vec2)*filePathCharLen, nullptr, GL_STATIC_DRAW);
        glCreateBuffers(1, &tabDPosBuffer);
        glNamedBufferData(tabDPosBuffer, sizeof(vec2)*filePathCharLen, nullptr, GL_STATIC_DRAW);
        glCreateBuffers(1, &tabColorBuffer);
        glNamedBufferData(tabColorBuffer, sizeof(vec4)*filePathCharLen, nullptr, GL_STATIC_DRAW);

        createRectMesh(&tabBgBgVA, &tabBgBgVB, nullptr, 4*2*sizeof(float), {2});

        // Init size and line number max digit count
        setMaxLineNumberDigitCount(4);
        setSize(winWidth, winHeight-fileTabHeight);
    }

    void inline addGlyphToRenderListsASCII(char character, vec4 color){
        dUVList[glyphsToRender].x = (character%16)*fileTextEditorFont->glyphWidth/fileTextEditorFont->glyphTableWidth;
        dUVList[glyphsToRender].y = (character/16)*fileTextEditorFont->glyphHeight/fileTextEditorFont->glyphTableHeight;
        dPosList[glyphsToRender].x = rectX+dx*fileTextEditorFont->glyphWidth;
        dPosList[glyphsToRender].y = rectY+dy*fileTextEditorFont->glyphHeight;
        colorList[glyphsToRender] = color;
        glyphsToRender += 1;
    }

    void inline drawLineNumber(int lineNumber){
        assert(lineNumber <= maxLineNumber);
        sprintf(lineNumberStr, "%i", lineNumber);
        sprintf(lineNumberStrCorr, "");
        for(int i = 0; i<maxLineNumberDigitCount-strlen(lineNumberStr); i++){
            lineNumberStrCorr = strcat(lineNumberStrCorr, " ");
        }
        lineNumberStrCorr = strcat(lineNumberStrCorr, lineNumberStr);
        for(int i = 0; i<spacesAfterLineNumber; i++){
            lineNumberStrCorr = strcat(lineNumberStrCorr, " ");
        }
        
        for(int i=0; i<strlen(lineNumberStrCorr); i++){
            addGlyphToRenderListsASCII(lineNumberStrCorr[i], (lineNumber==cursorLine? vec4(1.0f, 0.5f, 0.5f, 1.0f): vec4(0.5f, 0.5f, 0.5f, 1.0f)));
            dx += 1;
        }
    }

    void drawTextASCII(){
        
        // Fill text property lists
        dx = 0;
        dy = 0;
        glyphsToRender = 0;

        LLNode* currentNode = firstShowingNode->next;
        int lineNumber = firstLineShowing;
        int columnNumber = 1;
        vec2 cursorPos = vec2(0,0);

        drawLineNumber(lineNumber);

        if(currentNode == cursorNode){ cursorPos = vec2(rectX+dx*fileTextEditorFont->glyphWidth, rectY+dy*fileTextEditorFont->glyphHeight); }

        while(currentNode != nullptr){
            if(currentNode->character == '\n'){
                if(lineNumber == firstLineShowing+maxLinesToShow - 1){
                    break;
                }else{
                    dx = 0;
                    dy += 1;
                    lineNumber += 1;
                    columnNumber = 1;
                    drawLineNumber(lineNumber);
                }
            }else if(currentNode->character == '\t'){
                for(int i = 0; i < tabWidth; i++){
                    if(columnNumber >= firstColumnShowing && columnNumber <= firstColumnShowing+maxColumnsToShow-1){
                        if(renderTabs){ addGlyphToRenderListsASCII((i!=tabWidth-1? '-': '>'), glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)); }
                        dx += 1;
                    }
                    columnNumber += 1;
                }
            }else{
                if(columnNumber >= firstColumnShowing && columnNumber <= firstColumnShowing+maxColumnsToShow-1){
                    addGlyphToRenderListsASCII(currentNode->character, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
                    dx += 1;
                }
                columnNumber += 1;
            }

            if(columnNumber > firstColumnShowing+maxColumnsToShow){
                while(currentNode != nullptr && currentNode->character != '\n'){
                    currentNode = currentNode->next;
                }
            }else{
                currentNode = currentNode->next;
                if(currentNode == cursorNode){ cursorPos = glm::vec2(rectX+dx*fileTextEditorFont->glyphWidth, rectY+dy*fileTextEditorFont->glyphHeight); }
            }
        }

        // Draw editor text
        glNamedBufferSubData(dUVBuffer, 0, glyphsToRender*sizeof(vec2), dUVList);
        glNamedBufferSubData(dPosBuffer, 0, glyphsToRender*sizeof(vec2), dPosList);
        glNamedBufferSubData(colorBuffer, 0, glyphsToRender*sizeof(vec4), colorList);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, dUVBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, dPosBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, colorBuffer);

        glBindVertexArray(fileTextEditorFont->glyphVA);
        glyphsShader->setUniform("MVP", projMat);
        glyphsShader->bind();
        glBindTextureUnit(0, fileTextEditorFont->glyphTableTexture);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, glyphsToRender);

        // Draw cursor
        if((cursorLine >= firstLineShowing && cursorLine <= firstLineShowing+maxLinesToShow - 1) &&
           (cursorColumn >= firstColumnShowing && cursorColumn <= firstColumnShowing+maxColumnsToShow)){
            glBindVertexArray(fileTextEditorFont->cursorVA);
            fileTextEditorFont->cursorShader->setUniform("VP_mat", projMat);
            fileTextEditorFont->cursorShader->setUniform("position", cursorPos);
            fileTextEditorFont->cursorShader->setUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 0.85f));
            fileTextEditorFont->cursorShader->bind();
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }

        // Draw vertical scroll bar background
        glBindVertexArray(vBarBgVA);
        colorRectShader->setUniform("MVP", projMat);
        colorRectShader->setUniform("color", vec4(0.2, 0.2, 0.2, 1));
        colorRectShader->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Draw vertical scroll bar background
        float yTop = rectY+(firstLineShowing-1)/(float(textLL->nLines)+(maxLinesToShow-1)) * rectHeight;
        float yBottom = yTop+maxLinesToShow/(float(textLL->nLines)+(maxLinesToShow-1)) * rectHeight;
        float vBarVData[] = {
            rectWidth,      yTop,
            rectWidth,      yBottom,
            rectWidth+barWidth, yTop,          
            rectWidth+barWidth, yBottom,
        };
        glNamedBufferData(vBarVB, sizeof(vBarVData), vBarVData, GL_STATIC_DRAW);

        glBindVertexArray(vBarVA);
        colorRectShader->setUniform("MVP", projMat);
        colorRectShader->setUniform("color", vec4(0.7, 0.7, 0.7, 1));
        colorRectShader->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }

    ~FileTextEditor(){
        delete textLL; //?

        delete[] dUVList;
        delete[] dPosList;
        delete[] colorList;
        glDeleteBuffers(1, &dUVBuffer);
        glDeleteBuffers(1, &dPosBuffer);
        glDeleteBuffers(1, &colorBuffer);

        free(lineNumberStr);
        free(lineNumberStrCorr);

        glDeleteVertexArrays(1, &tabBgBgVA);
        glDeleteBuffers(1, &tabBgBgVB);
        glDeleteVertexArrays(1, &tabBgVA);
        glDeleteBuffers(1, &tabBgVB);

        delete[] tabDUVList;
        delete[] tabDPosList;
        delete[] tabColorList;
        glDeleteBuffers(1, &tabDUVBuffer);
        glDeleteBuffers(1, &tabDPosBuffer);
        glDeleteBuffers(1, &tabColorBuffer); 

        glDeleteVertexArrays(1, &vBarBgVA);
        glDeleteBuffers(1, &vBarBgVB);
        glDeleteVertexArrays(1, &vBarVA);
        glDeleteBuffers(1, &vBarVB);
    }
};
FileTextEditor* SFTE;

//### FileEditorLL ###//
struct FileEditorNode{
    FileTextEditor* editor;
    FileEditorNode* next;
};

void LLCreate(FileEditorNode** entryNode){
    (*entryNode) = new FileEditorNode;

    (*entryNode)->next = (*entryNode);
    (*entryNode)->editor = nullptr;
}

void LLInsertNode(FileEditorNode* prevNode, FileTextEditor* editor){
    FileEditorNode* newNode = new FileEditorNode;

    newNode->next = prevNode->next;
    newNode->editor = editor;
    prevNode->next = newNode;
}

FileEditorNode* fileEditorLLEntry;
FileEditorNode* currentEditorNode;

void insertEditorBack(FileTextEditor* editor){
    FileEditorNode* currentNode = fileEditorLLEntry->next;

    while(currentNode->next != fileEditorLLEntry){
        currentNode = currentNode->next;
    }

    LLInsertNode(currentNode, editor);
}

//###  ###//
void drawEditorTabs(){
    
    // Draw editor tabs background's background
    glBindVertexArray(SFTE->tabBgBgVA);
    colorRectShader->setUniform("color", vec4(0.1,0.1,0.1,1));
    colorRectShader->setUniform("projMat", projMat);
    colorRectShader->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    
    // Draw editor tabs text and background
    int dx = strlen(SFTE->lineNumberStrCorr)*fileTextEditorFont->glyphWidth;

    FileEditorNode* currentNode = fileEditorLLEntry->next;
    while(currentNode != fileEditorLLEntry){
        FileTextEditor* fileEditor = currentNode->editor;

        // Calculate tab dimensions
        int width = strlen(fileEditor->textLL->fileName)*fileTabFont->glyphWidth + 2*fileTabFont->glyphWidth;
        int height = fileTabHeight;
        
        // Draw tab background
        float fileTabVData[] = {
            dx+0.0,     0.0,
            dx+0.0,     height,
            dx+width,   0.0,
            dx+width,   height,
        };
        glNamedBufferSubData(fileEditor->tabBgVB, 0, sizeof(fileTabVData), fileTabVData);

        glBindVertexArray(fileEditor->tabBgVA);
        colorRectShader->setUniform("color", (fileEditor==SFTE? vec4(0.7,0.7,0.7,1): fileTabUnselectedColor));
        colorRectShader->setUniform("projMat", projMat);
        colorRectShader->bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Draw tab text
        int filePathCharLen = strlen(fileEditor->textLL->fileName);
        for(int i = 0; i<filePathCharLen; i++){
            fileEditor->tabDUVList[i].x = (fileEditor->textLL->fileName[i]%16)*fileTabFont->glyphWidth/fileTabFont->glyphTableWidth;
            fileEditor->tabDUVList[i].y = (fileEditor->textLL->fileName[i]/16)*fileTabFont->glyphHeight/fileTabFont->glyphTableHeight;
            fileEditor->tabDPosList[i].x = dx+fileTabFont->glyphWidth + i*fileTabFont->glyphWidth;
            fileEditor->tabDPosList[i].y = (fileTabHeight - fileTabFont->glyphHeight)/2;
            fileEditor->tabColorList[i] = vec4(1,1,1,1);
        }
        glNamedBufferSubData(fileEditor->tabDUVBuffer, 0, filePathCharLen*sizeof(vec2), fileEditor->tabDUVList);
        glNamedBufferSubData(fileEditor->tabDPosBuffer, 0, filePathCharLen*sizeof(vec2), fileEditor->tabDPosList);
        glNamedBufferSubData(fileEditor->tabColorBuffer, 0, filePathCharLen*sizeof(vec4), fileEditor->tabColorList);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, fileEditor->tabDUVBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, fileEditor->tabDPosBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, fileEditor->tabColorBuffer);
        
        glBindVertexArray(fileTabFont->glyphVA);
        glyphsShader->setUniform("MVP", projMat);
        glyphsShader->bind();
        glBindTextureUnit(0, fileTabFont->glyphTableTexture);
        glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr, filePathCharLen);

        dx += width + 1;

        currentNode = currentNode->next;
    }
}

