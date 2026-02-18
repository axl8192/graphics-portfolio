
////////
while(currentNode != nullptr){
    char character = currentNode->character;
    
    if(currentNode == cursorTextNode){
        cursorPos = glm::vec2(ix+dx*glyphWidth, iy+dy*glyphHeight);
    }

    if(character == '\n'){
        dx = 0;
        dy += 1;
        lineNumber += 1;
        columnNumber = 1;
        
        if(lineNumber-firstTextLineShowing == maxLinesToShow){
            char* dotdotdot = (char*)malloc(maxLineNumberDigitCount+spacesAfterLineNumber+1);
            sprintf(dotdotdot, "");
            for(int i = 0; i<maxLineNumberDigitCount-3; i++){
                dotdotdot = strcat(dotdotdot, " ");
            }
            dotdotdot = strcat(dotdotdot, "...");
            for(int i = 0; i<spacesAfterLineNumber; i++){
                dotdotdot = strcat(dotdotdot, " ");
            }

            for(int i=0; i<strlen(dotdotdot); i++){
                float dPosx = ix+dx*glyphWidth;
                float dPosy = iy+dy*glyphHeight;
            
                float dUVx = (dotdotdot[i]%16)*glyphWidth/glyphTableWidth;
                float dUVy = (dotdotdot[i]/16)*glyphHeight/glyphTableHeight;

                glyphsShader->setUniform("dPos", glm::vec2(dPosx, dPosy));
                glyphsShader->setUniform("dUV", glm::vec2(dUVx, dUVy));
                glyphsShader->setUniform("textColor", glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

                dx += 1;
            }
            
            break;
        }

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
            float dPosx = ix+dx*glyphWidth;
            float dPosy = iy+dy*glyphHeight;
        
            float dUVx = (lineNumberStrCorr[i]%16)*glyphWidth/glyphTableWidth;
            float dUVy = (lineNumberStrCorr[i]/16)*glyphHeight/glyphTableHeight;

            glyphsShader->setUniform("dPos", glm::vec2(dPosx, dPosy));
            glyphsShader->setUniform("dUV", glm::vec2(dUVx, dUVy));
            glyphsShader->setUniform("textColor", (lineNumber==cursorLine? glm::vec4(1.0f, 0.5f, 0.5f, 1.0f): glm::vec4(0.5f, 0.5f, 0.5f, 1.0f)));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            dx += 1;
        }
    }else if(columnNumber >= firstTextColumnShowing-1){
        if(character == '\t'){
            for(int i = 0; i < tabWidth; i++){
                float dPosx = ix+dx*glyphWidth;
                float dPosy = iy+dy*glyphHeight;
                
                float dUVx = (' '%16)*glyphWidth/glyphTableWidth;
                float dUVy = (' '/16)*glyphHeight/glyphTableHeight;

                glyphsShader->setUniform("dPos", glm::vec2(dPosx, dPosy));
                glyphsShader->setUniform("dUV", glm::vec2(dUVx, dUVy));
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

                dx += 1;
                columnNumber += 1;
            }
        }else if(currentNode->next != nullptr){
            float dPosx = ix+dx*glyphWidth;
            float dPosy = iy+dy*glyphHeight;
            
            float dUVx = (character%16)*glyphWidth/glyphTableWidth;
            float dUVy = (character/16)*glyphHeight/glyphTableHeight;

            glyphsShader->setUniform("dPos", glm::vec2(dPosx, dPosy));
            glyphsShader->setUniform("dUV", glm::vec2(dUVx, dUVy));
            glyphsShader->setUniform("textColor", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

            dx += 1;
            columnNumber += 1;
        }
    }

    if(columnNumber > maxColumnsToShow+firstTextColumnShowing){
        while(currentNode->character != '\n' && currentNode->next != nullptr){
            currentNode = currentNode->next;
        }
    }else if(columnNumber < firstTextColumnShowing - 1){
        if(character == '\t'){
            columnNumber += tabWidth;
        }else{
            columnNumber += 1;
        }
        currentNode = currentNode->next;
    }else{
        currentNode = currentNode->next;
    }
}

if(cursorLine >= firstTextLineShowing && cursorLine <= firstTextLineShowing+maxLinesToShow - 1){
    glBindVertexArray(cursorVA);
    cursorShader->setUniform("VP_mat", projMat);
    cursorShader->setUniform("position", cursorPos);
    cursorShader->setUniform("color", glm::vec4(1.0f, 1.0f, 1.0f, 0.85f));
    cursorShader->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}


/////
// Glyph Table (DEBUG)
glyphTableDbgMesh->bind();
glyphTableDbgShader->setUniform("MVP", projMat*glm::translate(glm::vec3(0,300,0)));
glyphTableDbgShader->bind();
glBindTextureUnit(0, glyphTableTexture);
glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

//------------------------------------------------------------------ CTRL
//### Move cursor ctrl ###//
void moveTextCursorCtrlRight(){
    LLNode* currentNode = cursorTextNode;
    bool canSkip = false;
    bool breakCondition = false;

    while(currentNode->next != nullptr && !breakCondition){
        if(currentNode->character == '\n'){ cursorLinePlus(); }
        
        currentNode = currentNode->next;
        
        if(!canSkip){
            canSkip = true;
            for(char character: wordSeparatorList){
                if(currentNode->character == character){
                    canSkip = false;
                }
            }
        }else{
            for(char character: wordSeparatorList){
                if(currentNode->character == character){
                    breakCondition = true;
                }
            }
        }
    }
    
    cursorTextNode = currentNode;
    practicalCursorColumn = calcCursorColum();
    cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

void moveTextCursorCtrlLeft(){
    LLNode* currentNode = cursorTextNode;
    bool canSkip = false;
    bool breakCondition = false;

    while(currentNode->previous->previous != nullptr && !breakCondition){
        currentNode = currentNode->previous;

        if(currentNode->character == '\n'){ cursorLineMinus(); }
        
        if(!canSkip){
            canSkip = true;
            for(char character: wordSeparatorList){
                if(currentNode->character == character){
                    canSkip = false;
                }
            }
        }else{
            for(char character: wordSeparatorList){
                if(currentNode->previous->character == character){
                    breakCondition = true;
                }
            }
        }
    }
    
    cursorTextNode = currentNode;
    practicalCursorColumn = calcCursorColum();
    cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

//### Remove ctrl ###//
void removePreviousCharCtrl(){
    if(cursorTextNode->previous->previous != nullptr){
        LLNode* currentNode = cursorTextNode->previous;
        bool canSkip = false;
        bool breakCondition = false;

        while(currentNode->previous->previous != nullptr && !breakCondition){
            if(!canSkip){
                canSkip = true;
                for(char character: wordSeparatorList){
                    if(currentNode->character == character){
                        canSkip = false;
                    }
                }
            }else{
                for(char character: wordSeparatorList){
                    if(currentNode->previous->character == character){
                        breakCondition = true;
                    }
                }
            }

            if(currentNode->character == '\n'){ cursorLineMinus(); }
            currentNode = currentNode->previous;
            LLRemoveNode(currentNode->next);
        }
        
        if(currentNode->character == '\n'){ cursorLineMinus(); }
        currentNode = currentNode->next;
        LLRemoveNode(currentNode->previous);
        
        cursorTextNode = currentNode;
    }
    practicalCursorColumn = calcCursorColum();
    cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}

void removeCurrentCharCtrl(){
    LLNode* currentNode = cursorTextNode;
    bool canSkip = false;
    bool breakCondition = false;

    while(currentNode->next != nullptr && !breakCondition){
        currentNode = currentNode->next;
        LLRemoveNode(currentNode->previous);
        
        if(!canSkip){
            canSkip = true;
            for(char character: wordSeparatorList){
                if(currentNode->character == character){
                    canSkip = false;
                }
            }
        }else{
            for(char character: wordSeparatorList){
                if(currentNode->character == character){
                    breakCondition = true;
                }
            }
        }
    }
    
    cursorTextNode = currentNode;
    practicalCursorColumn = calcCursorColum();
    cursorColumn = calcCursorColum();
    adjustFirstShowingLineToCursor();
}



