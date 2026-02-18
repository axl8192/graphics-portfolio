@echo off

:: Create .obj directory
mkdir ".obj/" > nul  2>&1

:: App Name
set exeName=Mandelbrot

:: Generate exe
del /q %exeName%.exe
call "C:/Archivos de programa/Microsoft Visual Studio 12.0/VC/vcvarsall.bat"
cl *.cpp /nologo /MT /EHsc /Fo:".obj/" /Fe:%exeName%.exe ^
	glew32s.lib glfw3.lib gdi32.lib opengl32.lib user32.lib shell32.lib

:: Remove .obj directory
start /B cmd /c rd /s /q ".obj/"

:: Execute exe
echo ---------------------------------------------------------------------------
call %exeName%.exe
