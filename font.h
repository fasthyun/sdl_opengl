#ifndef FONT_H
#define FONT_H

//#include <string>

void render_text_texture();
void render_text_texture_vb();
void render_text(const char *text, float x, float y, float scale=0.05) ;
void render_text(std::string text, float x, float y, float scale=0.05) ;

void init_font();
int init_font_freetype();
#endif // FONT_H
