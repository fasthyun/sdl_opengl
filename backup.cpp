#include <GL/gl.h>

void render_text_texture() {
    return;
    //glEnable(GL_TEXTURE_2D);
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    /*
    glTexCoord2f(0.0, 0.0); glVertex3f(1.0, -1.0, 0.0);
    glTexCoord2f(0.0, 1.0); glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2f(1.0, 1.0); glVertex3f(2.41421, 1.0, -1.41421);
    glTexCoord2f(1.0, 0.0); glVertex3f(2.41421, -1.0, -1.41421);
    */
    glBindTexture(GL_TEXTURE_2D, 0); // ???
    return ;
    //glFlush();
    //glActiveTexture(GL_TEXTURE0);
    //printf("tex=%d\n",tex);
    //glBindTexture(GL_TEXTURE_2D, 3);
    glBegin(GL_TRIANGLES);
    glVertex2f(0.f, 0.f);	glTexCoord2f(0.f, 0.f);
    glVertex2f(5.f, 0.f);	glTexCoord2f(1.f, 0.f);
    glVertex2f(0.f, 5.f);	glTexCoord2f(0.f, 1.f);
    glEnd();
    return;
    //glColor4f(.5, .5, .5, 1);
    //glEnable(GL_BLEND);
    //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_CULL_FACE);
    glEnable(GL_TEXTURE_2D);
}
