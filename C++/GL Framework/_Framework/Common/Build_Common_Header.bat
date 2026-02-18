@echo off
mkdir ".obj" > nul  2>&1
call vcvars32.bat
cl /c /Zi Common.cpp /Fo:".obj/" /YcCommon.hpp /MT /EHsc
