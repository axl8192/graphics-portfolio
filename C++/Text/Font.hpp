#pragma once
#include <ft2build.h>
#include <freetype/freetype.h>
#include "Helpers/Helpers.hpp"
#include "RectMesh.hpp"

// GPU
Shader* glyphsShader = nullptr;

struct Font{
    float glyphWidth;
    float glyphHeight;
    float glyphTableWidth;
    float glyphTableHeight;

    unsigned int glyphVA;
    unsigned int glyphTableTexture;

    unsigned int cursorVA;
    Shader* cursorShader;

    Mesh* glyphTableDbgMesh;
    Shader* glyphTableDbgShader;

    Font(const char* fontPath, unsigned int vSize){
        //### Load font ###//
        FT_Library ft;
        if (FT_Init_FreeType(&ft)){ cout << "Freetype Init Error" << endl; exit(-1); }

        FT_Face face;
        if (FT_New_Face(ft, fontPath, 0, &face)){ cout << "Face Error" << endl; exit(-1); }
        FT_Set_Pixel_Sizes(face, 0, vSize);

        //### Calculate max bearing ###//
        int maxBearingY = 0;
        for(int i = 0; i<128; i++){
            if(!FT_Load_Char(face, (char)i, FT_LOAD_RENDER)){
                maxBearingY = glm::max(face->glyph->bitmap_top, maxBearingY);
            }else{cout << "Char Error: " << (char)i << endl; exit(-1);}
        }
        
        //### Create glyph table ###// 
        glyphWidth = face->max_advance_width*face->size->metrics.x_ppem/face->units_per_EM + 1;
        glyphHeight = face->bbox.yMax*face->size->metrics.y_ppem/face->units_per_EM + 1;
        glyphTableWidth = glyphWidth*16;
        glyphTableHeight = glyphHeight*8;

        glCreateTextures(GL_TEXTURE_2D, 1, &glyphTableTexture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTextureStorage2D(glyphTableTexture, 1, GL_R8, glyphTableWidth, glyphTableHeight);

        for(int i = 0; i<128; i++){
            if(!FT_Load_Char(face, (char)i, FT_LOAD_RENDER)){
                glTextureSubImage2D(glyphTableTexture, 0, (i%16)*glyphWidth+face->glyph->bitmap_left, (i/16)*glyphHeight+maxBearingY-face->glyph->bitmap_top, face->glyph->bitmap.width, face->glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer); //Pretend that glTexture have D3D coords is easier. The image will be y-flipped if used normally, but it can be used normally if you also flip the texCoords. 
            }else{cout << "Char Error: " << i << endl;}
        }

        //### Glyph mesh and shader ###//
        float glyphVertexData[] = {
            0.0,          0.0,              0.0f/glyphTableWidth,       0.0f/glyphTableHeight,
            0.0,          glyphHeight,      0.0f/glyphTableWidth,       glyphHeight/glyphTableHeight,
            glyphWidth,   0.0,              glyphWidth/glyphTableWidth, 0.0f/glyphTableHeight,
            glyphWidth,   glyphHeight,      glyphWidth/glyphTableWidth, glyphHeight/glyphTableHeight,
        };
        unsigned int glyphVB;
        createRectMesh(&glyphVA, &glyphVB, glyphVertexData, sizeof(glyphVertexData), {2, 2});
        if(glyphsShader == nullptr) glyphsShader = new Shader("Shaders/glyph.glsl");

        //### Cursor mesh and shader ###//
        float cursorVData[] = {
            0.0,          0.0,
            0.0,          glyphHeight,
            glyphWidth/7, 0.0,
            glyphWidth/7, glyphHeight,
        };
        unsigned int cursorVB;
        createRectMesh(&cursorVA, &cursorVB, cursorVData, sizeof(cursorVData), {2});
        cursorShader = new Shader("Shaders/cursor.glsl");

        //### Glyph table debug mesh and shader ###//
        float glyphTableDbgVertexData[] = {
            0.0,             0.0,                  0.0f, 0.0f,
            0.0,             glyphTableHeight,     0.0f, 1.0f,
            glyphTableWidth, 0.0,                  1.0f, 0.0f,
            glyphTableWidth, glyphTableHeight,     1.0f, 1.0f,
        };
        unsigned int defaultSpriteIndexData[6] = { 0,1,2, 1,3,2 };
        glyphTableDbgMesh = new Mesh(glyphTableDbgVertexData, sizeof(glyphTableDbgVertexData), {2,2}, defaultSpriteIndexData, sizeof(defaultSpriteIndexData));
        glyphTableDbgShader = new Shader("Shaders/basicTextureShader.glsl");

        //### Free Freetype ###//
        FT_Done_Face(face);
        FT_Done_FreeType(ft);
    }
};

Font* fileTextEditorFont; //create allways before any FileTextEditor
Font* fileTabFont; //create allways before any FileTextEditor