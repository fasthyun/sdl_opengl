#include <qopengl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <string>
#include <algorithm>
//#define STB_IMAGE_IMPLEMENTATION
#include "./support/stb_image.h"
#include "texture.h"

FT_Library ft;
GLuint tex;

int atlas_width;
int atlas_height;

struct character_info {
  float ax; // advance.x
  float ay; // advance.y

  float bw; // bitmap.width;
  float bh; // bitmap.rows;

  float bl; // bitmap_left;
  float bt; // bitmap_top;

  float tx; // x offset of glyph in texture coordinates
} c[128];
//std::map<char, Character> Characters;
uint8_t *imgBuff;


/*  Create checkerboard texture  */
#define checkImageWidth 64
#define checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLuint texName;

void makeCheckImage(void)
{
   int i, j, c;

   for (i = 0; i < checkImageHeight; i++) {
      for (j = 0; j < checkImageWidth; j++) {
         c = ((((i&0x8)==0)^((j&0x8))==0))*255;
         checkImage[i][j][0] = (GLubyte) c;
         checkImage[i][j][1] = (GLubyte) c;
         checkImage[i][j][2] = (GLubyte) c;
         checkImage[i][j][3] = (GLubyte) 255;
      }
   }
}

void init(void)
{
    //glShadeModel(GL_FLAT);
   glEnable(GL_DEPTH_TEST);

   makeCheckImage();
   //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

   glGenTextures(1, &texName);
   glBindTexture(GL_TEXTURE_2D, texName);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth,
                checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                checkImage);
}

void checkError(){
while (GLenum error = glGetError())
{
    std::string errStr = "";
    switch (error)
    {
        case GL_NO_ERROR:          errStr = "No Errors"; break;
        case GL_INVALID_ENUM:      errStr = "Invalid Enum"; break;
        case GL_INVALID_VALUE:     errStr = "Invalid Value"; break;
        case GL_INVALID_OPERATION: errStr = "Invalid Operation"; break;
        case GL_INVALID_FRAMEBUFFER_OPERATION: errStr = "Invalid Framebuffer Operation"; break;
        case GL_OUT_OF_MEMORY:   errStr = "Out of Memory"; break;
        case GL_STACK_UNDERFLOW: errStr = "Stack Underflow"; break;
        case GL_STACK_OVERFLOW:  errStr = "Stack Overflow"; break;

        default: errStr = "Unknown"; break;
    }

    std::cout << "[OpenGL Error] - " << errStr << "\n";
}
}

void font_init2(){
    int w,h;
    int comp;

    GLubyte data[] = {
        0, 0, 255, 255,
        255, 0, 0, 255,
        0, 0, 255, 255,
        255, 0, 0, 255,
    };
    init();
    //return;

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2,
            0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

 //glGenerateMipmap(GL_TEXTURE_2D);
    checkError();

    printf("font_init2() %d \n",tex);
    return;
    /*
    imgBuff = stbi_load("/home/hyun/works/qt_opengl/font-map-mtl.png", &w, &h, &comp, 0);
    if(imgBuff == nullptr)
        throw(std::string("Failed to load texture"));
    printf("image w=%d h=%d comp=%d \n",w,h,comp);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(comp == 3)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, imgBuff);
    else if(comp == 4)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBuff);

    stbi_image_free(imgBuff);

    glBindTexture(GL_TEXTURE_2D, 0);
    */
}

int font_init()
{    
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "./andalemo.ttf", 0, &face))
    {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }


    int w = 0;
    unsigned int h = 0;


    int pensize = 48;
    FT_Set_Pixel_Sizes(face, pensize, 0);
    unsigned int max_w=0;
    unsigned int max_h=0;

    for(int i = 32; i < 128; i++) {
      if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
        fprintf(stderr, "Loading character %c failed!\n", i);
        continue;
      }
      FT_GlyphSlot g = face->glyph;
      max_w = std::max(max_w, g->bitmap.width);
      max_h = std::max(max_h, g->bitmap.rows);
    }

    printf("max_h=%d  max_w=%d \n",max_h,max_w);
    /* you might as well save this value as it is needed later on */
    //atlas_width = w;


    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);


    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    int x = 0;
    int err_code;
    for(int i = 32; i < 38; i++) {
        err_code=FT_Load_Char(face, i, FT_LOAD_RENDER);
        if (err_code!=0)
        {
            fprintf(stderr, "Loading character %c failed! %d \n", i,err_code);
            continue;
        }

        FT_GlyphSlot g = face->glyph;

        c[i].ax = g->advance.x >> 6;
        c[i].ay = g->advance.y >> 6;

        c[i].bw = g->bitmap.width;
        c[i].bh = g->bitmap.rows;

        c[i].bl = g->bitmap_left;
        c[i].bt = g->bitmap_top;

        c[i].tx = (float)x / w;
        // printf("idx=%d width=%d \n",i,g->bitmap.width);
        glTexSubImage2D(GL_TEXTURE_2D, 0 /*level*/ , x, 0, g->bitmap.width, g->bitmap.rows,
                        GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        x += g->bitmap.width;
        h = std::max(h, g->bitmap.rows);
    }
    atlas_width = x;
    atlas_height =h;
    return 0;
    printf("font_init()");
}


void display(void)
{
   //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   glEnable(GL_TEXTURE_2D);
   glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
   glBindTexture(GL_TEXTURE_2D, texName);
   glBegin(GL_QUADS);
   glTexCoord2f(0.0, 0.0); glVertex3f(-5.0, -5.0, 0.0);
   glTexCoord2f(0.0, 1.0); glVertex3f(-5.0, 5.0, 0.0);
   glTexCoord2f(1.0, 1.0); glVertex3f(0.0, 5.0, 0.0);
   glTexCoord2f(1.0, 0.0); glVertex3f(0.0, -5.0, 0.0);

   glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
   glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
   glTexCoord2f(1.0, 1.0); glVertex3f(2.41421, 1.0, -1.41421);
   glTexCoord2f(1.0, 0.0); glVertex3f(2.41421, -1.0, -1.41421);
   glEnd();
   //glFlush();
   //glDisable(GL_TEXTURE_2D);
}
void render_text_texture() {
    display();
    return ;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    //glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    //printf("tex=%d\n",tex);
    //glBindTexture(GL_TEXTURE_2D, 3);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.f, 0.f);	glTexCoord2f(0.f, 0.f);
    glVertex2f(5.f, 0.f);	glTexCoord2f(1.f, 0.f);
    glVertex2f(0.f, 5.f);	glTexCoord2f(0.f, 1.f);
    glEnd();

    checkError();

    //glDisable(GL_TEXTURE_2D);
    //glBindTexture(GL_TEXTURE_2D, 0);
    return;
    //glColor4f(.5, .5, .5, 1);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);
    glBegin(GL_QUADS);
    glTexCoord2f(0,0);
    glVertex2f(0,0);
    glTexCoord2f(1.0,0);
    glVertex2f(10,0);
    glTexCoord2f(1.0,1.0);
    glVertex2f(10,10);
    glTexCoord2f(0,1.0);
    glVertex2f(0,10);
    glEnd();

}

void render_text(const char *text, float x, float y, float sx, float sy) {
   glBindTexture(GL_TEXTURE_2D, tex);
   glBegin(GL_QUADS);

   for(const char *p = text; *p; p++) {
    //glTexCoord2i();
    //glVertex2f();
   }
   glEnd();

}
void render_text_x(const char *text, float x, float y, float sx, float sy) {

  struct point {
     GLfloat x;
     GLfloat y;
     GLfloat s;
     GLfloat t;
   } coords[6 * strlen(text)];

   int n = 0;
//ifdef GL_VERSION_4_6
   for(const char *p = text; *p; p++) {
     float x2 =  x + c[*p].bl * sx;
     float y2 = -y - c[*p].bt * sy;
     float w = c[*p].bw * sx;
     float h = c[*p].bh * sy;

     // Advance the cursor to the start of the next character
     x += c[*p].ax * sx;
     y += c[*p].ay * sy;

     // Skip glyphs that have no pixels
     if(!w || !h)
       continue;

     coords[n++] = (point){x2,     -y2    , c[*p].tx,   0};
     coords[n++] = (point){x2 + w, -y2    , c[*p].tx + c[*p].bw / atlas_width,   0};
     coords[n++] = (point){x2,     -y2 - h, c[*p].tx, c[*p].bh / atlas_height}; //remember: each glyph occupies a different amount of vertical space
     coords[n++] = (point){x2 + w, -y2    , c[*p].tx + c[*p].bw / atlas_width,   0};
     coords[n++] = (point){x2,     -y2 - h, c[*p].tx,  c[*p].bh / atlas_height};
     coords[n++] = (point){x2 + w, -y2 - h, c[*p].tx + c[*p].bw / atlas_width,  c[*p].bh / atlas_height};
   }

   //glBufferData(GL_ARRAY_BUFFER, sizeof coords, coords, GL_DYNAMIC_DRAW);
   //glDrawArrays(GL_TRIANGLES, 0, n);
}

