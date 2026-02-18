#pragma once
#include "../_Framework/GLFramework.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<stb_image_write.h>

#include <time.h>
#include <direct.h>

enum IM_FORMAT{
    BMP, PNG
};

void takeScreenshot(IM_FORMAT format){   
    //## Get Pixels ##//
    int imWidth=Renderer::getWinWitdh(), imHeight=Renderer::getWinHeight();
    unsigned char* pixels = new unsigned char[imWidth*imHeight*3];
    
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glPixelStorei(GL_PACK_LSB_FIRST, GL_TRUE);
    glReadBuffer(GL_BACK);
    glReadPixels(0, 0, imWidth, imHeight, (format==BMP? GL_BGR_EXT: GL_RGB), GL_UNSIGNED_BYTE, pixels);

    //## Write File ##//
    // Set Name and Dir
    char filenameC[50];
    time_t t = time(NULL);
    strftime(filenameC, sizeof(filenameC), "%Y%m%d_%H%M%S", localtime(&t));

    std::string screensDirName = "Screenshots";
    _mkdir("Screenshots"); //same as: system(("mkdir "+screensDirName+" >nul 2>&1").c_str()); but without showing console
    
    std::string filename = screensDirName+"/"+std::string(filenameC);
    
    // Different Format Writers
    if(format == BMP){

        // Open File
        std::ofstream capture(filename+".bmp", std::ios::binary | std::ios::trunc);

        // Header 1
        unsigned int bmpSize = 54+imWidth*imHeight*3;
        unsigned short uShortZero = 0;
        unsigned int offset = 54;

        capture << "BM"; // BMP identifier
        capture.write(reinterpret_cast<char*>(&bmpSize), 4); // Document Size (bytes)
        capture.write(reinterpret_cast<char*>(&uShortZero), 2); // Reserved (default uShort 0)
        capture.write(reinterpret_cast<char*>(&uShortZero), 2); // Reserved (default uShort 0)
        capture.write(reinterpret_cast<char*>(&offset), 4); // Byte where pixel data begins

        // Header 2
        unsigned int bmpCoreHeaderSize = 40;
        unsigned short colorPlanes = 1;
        unsigned short bitsPerPixel = 24;

        unsigned int compression = 0;
        unsigned int uIntZero = 0;
        int intTen = 10;

        capture.write(reinterpret_cast<char*>(&bmpCoreHeaderSize), 4); // Header Size
        capture.write(reinterpret_cast<char*>(&imWidth), 4); // Pixel Width
        capture.write(reinterpret_cast<char*>(&imHeight), 4); // Pixel Height
        capture.write(reinterpret_cast<char*>(&colorPlanes), 2); // Number of color planes (must be uShort 1)
        capture.write(reinterpret_cast<char*>(&bitsPerPixel), 2); // Bits Per Pixel

        capture.write(reinterpret_cast<char*>(&compression), 4); // Compression
        capture.write(reinterpret_cast<char*>(&uIntZero), 4); // Size of the raw bitmap data (not compressed: uInt 0)
        capture.write(reinterpret_cast<char*>(&intTen), 4); // Horizontal resolution (px/meter)
        capture.write(reinterpret_cast<char*>(&intTen), 4); // Vertical resolution (px/meter)
        capture.write(reinterpret_cast<char*>(&uIntZero), 4); // Number of colors in the color palette (default: uInt 0)
        capture.write(reinterpret_cast<char*>(&uIntZero), 4); // Number of important colors (default: uInt 0)
        
        // Body
        capture.write((const char*)pixels, imWidth*imHeight*3);

        // Close File
        capture.close();
    }else if(format==PNG){
        stbi_flip_vertically_on_write(1);
        stbi_write_png((filename+".png").c_str(), imWidth, imHeight, 3, pixels, 0);
    }
    
    delete[] pixels;
}