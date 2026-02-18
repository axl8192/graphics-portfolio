#include "Globals.hpp"
#include "Window.hpp"
#include "Draw.hpp"
#include "FileTextEditor.hpp"
#include "RectMesh.hpp"

int main(void){
    createWindow();
    initGlobalIB();
    initFileTabGraphics();
    projMat = glm::ortho(0.0f, (float)winWidth, (float)winHeight, 0.0f, 0.0f, 1.0f);

    fileTextEditorFont = new Font("Fonts/consola.ttf", 18);
    fileTabFont = new Font("Fonts/consola.ttf", 15);
    
    LLCreate(&fileEditorLLEntry);
    SFTE = new FileTextEditor(new TextLL("stress.txt")); //////// REMOVE SFTE, and only use currentEditorNode->editor
    
    insertEditorBack(SFTE);
    insertEditorBack(new FileTextEditor(new TextLL("saveTest.jai")));
    insertEditorBack(new FileTextEditor(new TextLL("third.txt")));

    currentEditorNode = fileEditorLLEntry->next;

    while (!glfwWindowShouldClose(mainWindow))
    {
        getInput();
        draw();

        glfwPollEvents();
        Clock::updateFrameTime(true, true, 0.1f);
    }

    glfwTerminate();
    return 0;
}