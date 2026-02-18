#pragma once
#include "Globals.hpp"
#include "FileTextEditor.hpp"

void draw(){
    glClearColor(textBackground.r, textBackground.g, textBackground.b, textBackground.a);
    glClear(GL_COLOR_BUFFER_BIT);
    drawEditorTabs();
    currentEditorNode->editor->drawTextASCII();
    glfwSwapBuffers(mainWindow);
}

