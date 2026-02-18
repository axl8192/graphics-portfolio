#pragma once
#include "../_Framework/GLFramework.hpp"
#include "Axes.hpp"
#include "OrbitCamera.hpp"
#include "3DFunctionDrawer.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_internal.h>
#include <imgui/imgui_stdlib.h>

namespace IG = ImGui;

void imGuiInit(){
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(Renderer::getWindow(), true);
    ImGui_ImplOpenGL3_Init();
    ImGui::StyleColorsDark();
}

void imGuiNewFrame(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void imGuiEnd(){
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void imGuiRender(){
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

class FuncGUI
{
private:
    bool isUsing;

    ImFont* fontS;
    ImFont* fontM; 
    Function3D& funcDrawer;
    Axes& axes;
    OrbitCamera& mainCam;

    bool funcAutoUpdate = false, solidColorFuncAutoUpdate = false, wireColorFuncAutoUpdate = false;
    bool solidColorsHeadOpened = false, wireColorsHeadOpened = false, funcParamsHeadOpened = false, othersHeadOpened = false;

    const std::string favFuncsFileName = "fav.funcs";
    std::vector<std::string> favFuncList;
    float savedInicatorTextTimer;

public:
    float screenshotTextTimer;
    bool show = true;

    FuncGUI(ImFont* iFontM, ImFont* iFontS, Function3D& iFuncDrawer, Axes& iAxes, OrbitCamera& iMainCam): 
        fontM(iFontM), 
        fontS(iFontS), 
        funcDrawer(iFuncDrawer), 
        axes(iAxes), 
        mainCam(iMainCam) 
    {
        std::ifstream favStream(favFuncsFileName);
        std::string currentFuncText;
        while(getline(favStream, currentFuncText))
        {   
            favFuncList.push_back(currentFuncText);
        }
        favStream.close();

        std::ifstream settings("settings.cfg");

        char charSettings[]{
                            0,0,0,
                            0,0,0,0,
                            1,0,0,1,0
                           };

        if(settings.is_open()){
            settings.read(charSettings, sizeof(charSettings));
            settings.close();
        }

        funcAutoUpdate = charSettings[0];
        solidColorFuncAutoUpdate = charSettings[1];
        wireColorFuncAutoUpdate = charSettings[2];

        solidColorsHeadOpened = charSettings[3];
        wireColorsHeadOpened = charSettings[4];
        funcParamsHeadOpened = charSettings[5];
        othersHeadOpened = charSettings[6];

        axes.showArrows = charSettings[7];
        axes.showLines = charSettings[8];
        mainCam.pivotVisible = charSettings[9];
        funcDrawer.showSolid = charSettings[10];
        funcDrawer.showWireframe = charSettings[11];
        
    }

    void update(){
        //### Main Options Window ###// 
        isUsing = false;
        if(!show) return;

        // Check if clear active focus
        if(Input::getMouseButtonDown(GLFW_MOUSE_BUTTON_MIDDLE) || Input::getMouseButtonDown(GLFW_MOUSE_BUTTON_RIGHT)){
            IG::ClearActiveID();
            IG::FocusWindow(NULL);
        }
        
        // Func Writers Backgrounds
        ImVec4 loadedBgCol(0,0.7,0,0.8), notLoadedBgCol(1,0,0,0.8);
        ImVec4 funcBg, colFuncRBg, colFuncGBg, colFuncBBg, wireColFuncRBg, wireColFuncGBg, wireColFuncBBg;
        
        if(funcDrawer.prepFunc == funcDrawer.getRenderedFunc() || funcAutoUpdate){ funcBg = loadedBgCol; } else { funcBg = notLoadedBgCol; }

        if(funcDrawer.prepColorFuncR == funcDrawer.getRenderedColorFuncR() || solidColorFuncAutoUpdate){ colFuncRBg = loadedBgCol; } else { colFuncRBg = notLoadedBgCol; }
        if(funcDrawer.prepColorFuncG == funcDrawer.getRenderedColorFuncG() || solidColorFuncAutoUpdate){ colFuncGBg = loadedBgCol; } else { colFuncGBg = notLoadedBgCol; }
        if(funcDrawer.prepColorFuncB == funcDrawer.getRenderedColorFuncB() || solidColorFuncAutoUpdate){ colFuncBBg = loadedBgCol; } else { colFuncBBg = notLoadedBgCol; }

        if(funcDrawer.prepWireColorFuncR == funcDrawer.getRenderedWireColorFuncR() || wireColorFuncAutoUpdate){ wireColFuncRBg = loadedBgCol; } else { wireColFuncRBg = notLoadedBgCol; }
        if(funcDrawer.prepWireColorFuncG == funcDrawer.getRenderedWireColorFuncG() || wireColorFuncAutoUpdate){ wireColFuncGBg = loadedBgCol; } else { wireColFuncGBg = notLoadedBgCol; }
        if(funcDrawer.prepWireColorFuncB == funcDrawer.getRenderedWireColorFuncB() || wireColorFuncAutoUpdate){ wireColFuncBBg = loadedBgCol; } else { wireColFuncBBg = notLoadedBgCol; }
        
        // Init Options Window
        IG::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
        IG::PushFont(fontM);
        IG::PushStyleColor(ImGuiCol_TextSelectedBg, ImVec4(0, 0.1, 0.5, 0.5));
        IG::SetNextWindowPos(ImVec2(10.0f, 10.0f));
        IG::SetNextWindowSize(ImVec2(0.0f, 0.0f));
        IG::Begin("Main", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize);
        
        // Function Writer
        IG::AlignTextToFramePadding();
        IG::Text("z =");
        IG::SameLine();
        IG::SetNextItemWidth(450);
        IG::PushStyleColor(ImGuiCol_FrameBg, funcBg);
        
        if (IG::InputText("##FuncText", &funcDrawer.prepFunc, (funcAutoUpdate? 0: ImGuiInputTextFlags_EnterReturnsTrue))){
            funcDrawer.updateFunc();
        }
        if(IG::IsItemActive()) { isUsing = true; }
        IG::PopStyleColor();

        // Auto Update Func
        IG::SameLine();
        if(IG::Checkbox("Auto Update##Func", &funcAutoUpdate)){
            if(funcAutoUpdate) funcDrawer.updateFunc();
        }
        IG::Separator();

        // Save Func
        IG::Spacing();

        IG::Spacing();
        IG::SameLine();

        if(IG::Button("Save Function")){
            funcDrawer.updateFunc();

            favFuncList.push_back(funcDrawer.prepFunc);
            
            std::ofstream favStream(favFuncsFileName, std::ios::app);
            favStream << funcDrawer.prepFunc << std::endl;
            favStream.close();

            savedInicatorTextTimer = 1.0f;
        }
        
        // Saved Indicator
        if(savedInicatorTextTimer > 0){
            IG::SameLine();
            IG::TextColored(ImVec4(0,1,0,savedInicatorTextTimer), "Saved!");
            savedInicatorTextTimer -= Clock::getFrameTime(); 
        }
        
        // Load Func
        IG::Spacing();
        IG::SameLine();

        IG::SetNextItemWidth(450);
        if(IG::BeginCombo("##comboLoad", "Load Function")){
            isUsing = true;

            IG::PushStyleColor(ImGuiCol_Button, ImVec4(0.7,0,0,1));
            IG::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1,0.3,0.3,1));
            IG::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1,0.2,0.2,1));

            bool changed = false;
            int pos = 0;
            for(std::string funcText: favFuncList){   
                IG::AlignTextToFramePadding();
                IG::SetNextItemWidth(450);
                if(ImGui::Selectable(funcText.c_str(), false, ImGuiSelectableFlags_SelectOnRelease, ImVec2(450,0))){
                    funcDrawer.prepFunc = funcText;
                    funcDrawer.updateFunc();
                }
                IG::SameLine();
                
                if(ImGui::Button(("X##"+std::to_string(pos)).c_str())){
                    favFuncList.erase(favFuncList.begin() + pos);
                    changed = true;
                }

                pos++;
            }

            IG::PopStyleColor();
            IG::PopStyleColor();
            IG::PopStyleColor();

            if(changed){
                std::ofstream favStream(favFuncsFileName, std::ios::trunc);
                for(std::string funcText: favFuncList){
                    favStream << funcText << std::endl;
                }
                favStream.close();
            }

            IG::EndCombo();
        }
        IG::Spacing();
        
        // Colors Solid
        if(solidColorsHeadOpened = IG::CollapsingHeader("Solid Colors", (solidColorsHeadOpened? ImGuiTreeNodeFlags_DefaultOpen: 0))){
            if (IG::Checkbox("Auto Update##SolidC", &solidColorFuncAutoUpdate)){
                if(solidColorFuncAutoUpdate) funcDrawer.updateFunc();
            }

            IG::AlignTextToFramePadding();
            IG::Text("R = ");
            IG::SameLine(40.0f, 0.0f);
            IG::SetNextItemWidth(300);
            IG::PushStyleColor(ImGuiCol_FrameBg, colFuncRBg);
            if (IG::InputText("##RS", &funcDrawer.prepColorFuncR, (solidColorFuncAutoUpdate? 0: ImGuiInputTextFlags_EnterReturnsTrue))){
                funcDrawer.updateFunc();
            }
            if(IG::IsItemActive()) { isUsing = true; }
            IG::PopStyleColor();

            IG::AlignTextToFramePadding();
            IG::Text("G = ");
            IG::SameLine(40.0f, 0.0f);
            IG::SetNextItemWidth(300);
            IG::PushStyleColor(ImGuiCol_FrameBg, colFuncGBg);
            if (IG::InputText("##GS", &funcDrawer.prepColorFuncG, (solidColorFuncAutoUpdate? 0: ImGuiInputTextFlags_EnterReturnsTrue))){
                funcDrawer.updateFunc();
            }
            if(IG::IsItemActive()) { isUsing = true; }
            IG::PopStyleColor();

            IG::AlignTextToFramePadding();
            IG::Text("B = ");
            IG::SameLine(40.0f, 0.0f);
            IG::SetNextItemWidth(300);
            IG::PushStyleColor(ImGuiCol_FrameBg, colFuncBBg);
            if (IG::InputText("##BS", &funcDrawer.prepColorFuncB, (solidColorFuncAutoUpdate? 0: ImGuiInputTextFlags_EnterReturnsTrue))){
                funcDrawer.updateFunc();
            }
            if(IG::IsItemActive()) { isUsing = true; }
            IG::PopStyleColor(); 
        }

        // Colors Wireframe
        if(wireColorsHeadOpened = IG::CollapsingHeader("Wireframe Colors", (wireColorsHeadOpened? ImGuiTreeNodeFlags_DefaultOpen: 0))){
            if(IG::Checkbox("Auto Update##WireC", &wireColorFuncAutoUpdate)){
                if(wireColorFuncAutoUpdate) funcDrawer.updateFunc();
            };

            IG::AlignTextToFramePadding();
            IG::Text("R = ");
            IG::SameLine(40.0f, 0.0f);
            IG::SetNextItemWidth(300);
            IG::PushStyleColor(ImGuiCol_FrameBg, wireColFuncRBg);
            if (IG::InputText("##RW", &funcDrawer.prepWireColorFuncR, (wireColorFuncAutoUpdate? 0: ImGuiInputTextFlags_EnterReturnsTrue))){
                funcDrawer.updateFunc();
            }
            if(IG::IsItemActive()) { isUsing = true; }
            IG::PopStyleColor();

            IG::AlignTextToFramePadding();
            IG::Text("G = ");
            IG::SameLine(40.0f, 0.0f);
            IG::SetNextItemWidth(300);
            IG::PushStyleColor(ImGuiCol_FrameBg, wireColFuncGBg);
            if (IG::InputText("##GW", &funcDrawer.prepWireColorFuncG, (wireColorFuncAutoUpdate? 0: ImGuiInputTextFlags_EnterReturnsTrue))){
                funcDrawer.updateFunc();
            }
            if(IG::IsItemActive()) { isUsing = true; }
            IG::PopStyleColor();

            IG::AlignTextToFramePadding();
            IG::Text("B = ");
            IG::SameLine(40.0f, 0.0f);
            IG::SetNextItemWidth(300);
            IG::PushStyleColor(ImGuiCol_FrameBg, wireColFuncBBg);
            if (IG::InputText("##BW", &funcDrawer.prepWireColorFuncB, (wireColorFuncAutoUpdate? 0: ImGuiInputTextFlags_EnterReturnsTrue))){
                funcDrawer.updateFunc();
            }
            if(IG::IsItemActive()) { isUsing = true; }
            IG::PopStyleColor();
        }
        
        // Variables
        if(funcParamsHeadOpened = IG::CollapsingHeader("Function Parameters", (funcParamsHeadOpened? ImGuiTreeNodeFlags_DefaultOpen: 0))){
            IG::Text(("a = " + std::to_string(funcDrawer.a) + ",").c_str());
            IG::SameLine();
            IG::Text(("b = " + std::to_string(funcDrawer.b) + ",").c_str());
            IG::SameLine();
            IG::Text(("t = " + std::to_string(funcDrawer.t)).c_str());

            IG::Text(("Size = " + std::to_string(funcDrawer.getSize()) + ",").c_str());
            IG::SameLine();
            IG::Text(("Count = " + std::to_string(funcDrawer.getCount())).c_str());
        }

        // Others
        if(othersHeadOpened = IG::CollapsingHeader("Others", (othersHeadOpened? ImGuiTreeNodeFlags_DefaultOpen: 0))){
            // Show
            IG::AlignTextToFramePadding();
            IG::Text("Show: ");
            IG::SameLine();
            IG::Checkbox("Axes Arrows", &axes.showArrows);
            IG::SameLine(0,20);
            IG::Checkbox("Axes Lines", &axes.showLines);
            IG::SameLine(0,20);
            IG::Checkbox("Pivot", &mainCam.pivotVisible);

            // Draw Modes
            IG::AlignTextToFramePadding();
            IG::Text("Mode:");
            IG::SameLine(0,11);
            IG::Checkbox("Solid", &funcDrawer.showSolid);
            IG::SameLine(0,78);
            IG::Checkbox("Wireframe", &funcDrawer.showWireframe);

            if(!funcDrawer.showSolid && !funcDrawer.showWireframe){
                IG::SameLine(0,20);
                IG::TextColored(ImVec4(1,0,0,1), "!!! NOT DRAWING ANYTHING !!!");
            }
            IG::Spacing();

            // FOV
            IG::Text("FOV:");
            IG::SameLine(0,22);
            IG::SliderFloat("##FOV", &mainCam.fov, 10.0f, 150.0f, "%.1fº");

            // Controls
            if(IG::TreeNode("Controls")){
                IG::PushFont(fontS);
                IG::Text("WASD - Move camera pivot");
                IG::SameLine(230,0);
                IG::Text("IJKLZX | MOUSE_WHEEL/MIDDLE_BUTTON - Orbit camera");
                IG::Text("Left/Right - Change 'a' parameter");
                IG::SameLine(230, 0);
                IG::Text("Up/Down - Change 'b' parameter");
                IG::Text("Q/E - Change Count");
                IG::SameLine(230, 0);
                IG::Text("N/M - Change Size");
                IG::Text("SPACE - Hide UI");
                IG::SameLine(230, 0);
                IG::Text("F5 - Take screenshot");
                IG::Text("C - Center Pivot");
                IG::SameLine(230, 0);
                IG::Text("SHIFT - Faster zoom and parameter changes");
                IG::PopFont();
                IG::TreePop();
            }
        }
        
        // End Options Window
        float optWinHeight = IG::GetWindowHeight();
        IG::End();
        
        //### Error Window ###//
        if(funcDrawer.getError()){
            IG::SetNextWindowPos(ImVec2(10, optWinHeight+15));
            IG::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            IG::Begin("Error", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing);
            IG::TextColored(ImVec4(1,0,0,1), "!!! ERROR !!!");
            IG::End();
        }
        
        //### Screenshot Taken Window ###//
        if(screenshotTextTimer > 0){
            IG::SetNextWindowPos(ImVec2(Renderer::getWinWitdh()-180, 10));
            IG::SetNextWindowSize(ImVec2(0.0f, 0.0f));
            IG::Begin("Screenshot", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoBackground);
            IG::TextColored(ImVec4(0,1,0,screenshotTextTimer), "Screenshot Saved!");
            screenshotTextTimer -= Clock::getFrameTime();
            IG::End();
        }

        //### End GUI ###//
        IG::PopStyleVar();
        IG::PopFont();
        IG::PopStyleColor();
    }
    
    void saveSettings(){
        std::ofstream settings("settings.cfg", std::ios::trunc);
        settings.write(reinterpret_cast<char*>(&funcAutoUpdate), 1);
        settings.write(reinterpret_cast<char*>(&solidColorFuncAutoUpdate), 1);
        settings.write(reinterpret_cast<char*>(&wireColorFuncAutoUpdate), 1);

        settings.write(reinterpret_cast<char*>(&solidColorsHeadOpened), 1);
        settings.write(reinterpret_cast<char*>(&wireColorsHeadOpened), 1);
        settings.write(reinterpret_cast<char*>(&funcParamsHeadOpened), 1);
        settings.write(reinterpret_cast<char*>(&othersHeadOpened), 1);

        settings.write(reinterpret_cast<char*>(&axes.showArrows), 1);
        settings.write(reinterpret_cast<char*>(&axes.showLines), 1);
        settings.write(reinterpret_cast<char*>(&mainCam.pivotVisible), 1);
        settings.write(reinterpret_cast<char*>(&funcDrawer.showSolid), 1);
        settings.write(reinterpret_cast<char*>(&funcDrawer.showWireframe), 1);

        settings << std::endl;
        settings << funcDrawer.prepFunc << std::endl;

        settings << funcDrawer.prepColorFuncR << std::endl;
        settings << funcDrawer.prepColorFuncG << std::endl;
        settings << funcDrawer.prepColorFuncB << std::endl;

        settings << funcDrawer.prepWireColorFuncR << std::endl;
        settings << funcDrawer.prepWireColorFuncG << std::endl;
        settings << funcDrawer.prepWireColorFuncB << std::endl;

        settings.close();
    }

    bool isBeingUsed(){return isUsing;}
};
