#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H //<freetype/freetype.h>
#include <iostream>
#include <string>
#include <algorithm> // std::max
#include "texture.h"

FT_Library ft;

GLuint tex_font_map;
GLuint tex_check;
GLuint font_vertexbuffer;
int atlas_width;
int atlas_height;
bool flag_font_init=false;

struct character_info {
  float ax; // advance.x
  float ay; // advance.y
  float bw; // bitmap.width;
  float bh; // bitmap.rows;
  float bl; // bitmap_left;
  float bt; // bitmap_top;
  float tx; // x offset of glyph in texture coordinates
  float ty; // y offset of glyph in texture coordinates
  float tw;
  float th;
} c[128];

//std::map<char, Character> Characters;

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


int init_font_freetype()
{    
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, "../font/andalemo.ttf", 0, &face))
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
      max_w = std::max<int>(max_w, g->bitmap.width);
      max_h = std::max<int>(max_h, g->bitmap.rows);
    }

    printf("max_h=%d  max_w=%d \n",max_h,max_w);
    /* you might as well save this value as it is needed later on */
    //atlas_width = w;
    //tex_font_map =;
    texture *_tex= new texture(512,512);
    SDL_Surface *surfce=_tex->getSurface(512,512);

    if (1)
   {

    }
     else
    {
      glActiveTexture(GL_TEXTURE0);
      //texture *_tex= new texture(512,512);
      glBindTexture(GL_TEXTURE_2D,  _tex->getTextureName());
      glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      // 비디오버퍼에 보낼게 없음???
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    }

    /*
        GLuint tex;
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // 비디오버퍼에 보낼게 없음???
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 512, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);
    */

    int x = 0;    
    int err_code;
    for(int i = 32; i < 38 ; i++) {
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
        fprintf(stderr,"idx=%d width=%d \n",i,g->bitmap.width);
        // draw character to texture
        //glTexSubImage2D(GL_TEXTURE_2D, 0 /*level*/ , x, 0, g->bitmap.width, g->bitmap.rows,
        //                GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        //for(int j=0; j<);

        x += g->bitmap.width;
        h = std::max<int>(h, g->bitmap.rows);
    }

    glBindTexture(GL_TEXTURE_2D, 0);
    atlas_width = x;
    atlas_height =h;
    return 0;    
}

//tex = new texture("check.bmp");
//tex_check=tex->getTextureName();

void init_font(){
    if (flag_font_init)
        return;
    texture *_tex = new texture("font-map.bmp"); // 16x6
    atlas_width=_tex->d_width;
    atlas_height=_tex->d_height;
    float cw=atlas_width/16;
    float ch=atlas_height/6;
    tex_font_map = _tex->getTextureName();

    float x=0,y=0;
    for(int i = 32; i < 127; i++) {
        // from space to ~
        c[i].ax = cw;
        c[i].ay = ch;
        c[i].bw = cw;
        c[i].bh = ch;
        c[i].bl = 0;
        c[i].bt = 0;
        c[i].tx = x/atlas_width;
        c[i].ty = y/atlas_height;
        c[i].tw = (float)cw /atlas_width;
        c[i].th = (float)ch /atlas_height;
        x+=cw;
        if (x>=atlas_width)
        {
            x=0;
            y+=ch;
        }
         //glGenBuffers(1, &font_vertexbuffer);
        // fprintf(stderr, "Loading character %d %c  %f,%f,%f,%f\n", i,i,c[i].tx,c[i].ty,c[i].tw,c[i].th);
        // glGenBuffers(1, &vertices_name);
        // glBindBuffer(GL_ARRAY_BUFFER, vertices_name);
        // glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vertex), data, GL_STATIC_DRAW);
    }
    printf("init_font()\n");
    flag_font_init=true;
    checkError();
}


struct point {
   GLfloat x;
   GLfloat y;
   GLfloat s;
   GLfloat t;
 } coords[6*128]; //

void render_text(const char *text, float x, float y, float scale) {
   // using vertex array : 과연 빨라질까?
   // 기본 폰트 출력 !!!
   scale=0.05; // basic
   float wscale=scale*0.5;
   glBindTexture(GL_TEXTURE_2D, tex_font_map);

   glBegin(GL_TRIANGLES);

   //char *stx="pq";
   for(const char *p = text; *p; p++) {
       int idx=(int) *p;
       float vx =  x ; // + c[idx].bl * wscale;
       float vy = -y; //- c[idx].bt * scale;
       float w = wscale; //c[idx].bw * scale;
       float h = scale;//c[idx].bh * scale;

       // Advance the cursor to the start of the next character
       x += wscale; //c[idx].bw * scale;
       //y += c[idx].ay * sy;
       // Skip glyphs that have no pixels
       if(!w || !h)
           continue;
       /*
       coords[n++] = (point){vx,     -vy    , c[idx].tx,                c[idx].ty};
       coords[n++] = (point){vx + w, -vy    , c[idx].tx + c[idx].tw ,   c[idx].ty};
       coords[n++] = (point){vx,     -vy - h, c[idx].tx,                c[idx].ty + c[idx].th };
       //remember: each glyph occupies a different amount of vertical space
       coords[n++] = (point){vx + w, -vy    , c[idx].tx + c[idx].tw ,   c[idx].ty};
       coords[n++] = (point){vx,     -vy - h, c[idx].tx,                c[idx].ty +  c[idx].th };
       coords[n++] = (point){vx + w, -vy - h, c[idx].tx + c[idx].tw ,   c[idx].ty +  c[idx].th };
       */
       // texcoord is first!
       // first triangle
       glTexCoord2f(c[idx].tx              , c[idx].ty ); glVertex3f(vx    , -vy , 0);
       glTexCoord2f(c[idx].tx + c[idx].tw  , c[idx].ty);  glVertex3f(vx + w, -vy , 0);
       glTexCoord2f(c[idx].tx + c[idx].tw  , c[idx].ty + c[idx].th); glVertex3f(vx + w, -vy-h, 0);

       // second triangle
       glTexCoord2f(c[idx].tx              , c[idx].ty ); glVertex3f(vx    , -vy , 0);
       glTexCoord2f(c[idx].tx + c[idx].tw  , c[idx].ty + c[idx].th); glVertex3f(vx + w, -vy-h, 0);
       glTexCoord2f(c[idx].tx              , c[idx].ty + c[idx].th);glVertex3f(vx    , -vy-h, 0);
   }
   glEnd();
   glBindTexture(GL_TEXTURE_2D, 0);

}

void render_text_vb(const char *text, float x, float y, float scale) {
  // NOT Yet !using vertex buffer
  struct point {
     GLfloat x;
     GLfloat y;
     GLfloat s;
     GLfloat t;
   } coords[6 * strlen(text)]; //
   int n = 0;
   scale=0.05;

   glBindTexture(GL_TEXTURE_2D, tex_font_map);
   glBindBuffer(GL_ARRAY_BUFFER, 0); // 새로은 버퍼 사용하지 않고 바로 사용!
   glVertexPointer(4, GL_FLOAT, 0, NULL); //??
   //glEnableClientState(GL_VERTEX_ARRAY);
   //glEnableClientState(GL_TEXTURE_COORD_ARRAY);

   for(const char *p = text; *p; p++) {
       int idx=(int) *p;
       float x2 =  x + c[idx].bl * scale;
       float y2 = -y - c[idx].bt * scale;
       float w = c[idx].bw * scale;
       float h = c[idx].bh * scale;

       // Advance the cursor to the start of the next character
       x += c[idx].bw * scale;
       //y += c[idx].ay * sy;

       // Skip glyphs that have no pixels
       if(!w || !h)
           continue;
       coords[n++] = (point){x2,     -y2    , c[idx].tx,   0};
       coords[n++] = (point){x2 + w, -y2    , c[idx].tx + c[idx].tw ,   0};
       coords[n++] = (point){x2,     -y2 - h, c[idx].tx,  c[idx].th };
       //remember: each glyph occupies a different amount of vertical space
       coords[n++] = (point){x2 + w, -y2    , c[idx].tx + c[idx].tw ,   0};
       coords[n++] = (point){x2,     -y2 - h, c[idx].tx,  c[idx].th };
       coords[n++] = (point){x2 + w, -y2 - h, c[idx].tx + c[idx].tw ,  c[idx].bh };

   }

  /*
   glEnableClientState(GL_VERTEX_ARRAY);
   glVertexPointer(2, GL_FLOAT, 2, coords);
   glEnableClientState(GL_TEXTURE_COORD_ARRAY);
   glTexCoordPointer(2, GL_FLOAT, 2, coords + sizeof(GLfloat)*2 );
   glDrawArrays(GL_TRIANGLES, 0, n);

   glDisableClientState(GL_VERTEX_ARRAY);
   glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  */
   //glBufferData(GL_ARRAY_BUFFER, sizeof (coords), coords, GL_DYNAMIC_DRAW);
   //glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDrawArrays(GL_TRIANGLES, 0, n);
   //glDisableClientState(GL_VERTEX_ARRAY);
   //glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void render_text(string text, float x, float y, float scale)
{
    render_text(text.c_str(),x,y,scale);
}


void render_text_tmp(const char *text, float x, float y, float sx, float sy) {
   //glBindTexture(GL_TEXTURE_2D, tex);
   //glBegin(GL_QUADS);

   for(const char *p = text; *p; p++) {
    //glTexCoord2i();
    //glVertex3f();
   }
   glEnd();
}
