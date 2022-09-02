#include <qopengl.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <iostream>
#include <algorithm>

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

    FT_GlyphSlot g = face->glyph;
    int w = 0;
    unsigned int h = 0;


    for(int i = 32; i < 128; i++) {
    /*  if(FT_Load_Char(face, i, FT_LOAD_RENDER)) {
        fprintf(stderr, "Loading character %c failed!\n", i);
        continue;
      }

      w += g->bitmap.width;
      h = std::max(h, g->bitmap.rows); */
    }

    /* you might as well save this value as it is needed later on */
    //atlas_width = w;



    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, 1, 1024*4, 128, 0, GL_ALPHA, GL_UNSIGNED_BYTE, 0);

    int x = 0;
    for(int i = 32; i < 128; i++) {
        if(FT_Load_Char(face, i, FT_LOAD_RENDER))
            continue;

        c[i].ax = g->advance.x >> 6;
        c[i].ay = g->advance.y >> 6;

        c[i].bw = g->bitmap.width;
        c[i].bh = g->bitmap.rows;

        c[i].bl = g->bitmap_left;
        c[i].bt = g->bitmap_top;

        c[i].tx = (float)x / w;

        glTexSubImage2D(GL_TEXTURE_2D, 0 /*level*/ , x, 0, g->bitmap.width, g->bitmap.rows,
                        GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        x += g->bitmap.width;
        h = std::max(h, g->bitmap.rows);
    }
    atlas_width = x;
    atlas_height =h;
    return 0;
}

void render_text_texture() {
    //glActiveTexture(GL_TEXTURE0);
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
