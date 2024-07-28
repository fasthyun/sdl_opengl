//#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <stdio.h>
#define GLT_IMPLEMENTATION
#include "gltext.h"

// misc.cpp
void gltDrawMatrix4f(GLTtext *textid, float *mat, float x, float y )
{
    char str[64*4];
    sprintf(str,"%08.4f,%8.4f,%8.4f,%8.4f\n"
                "%08.4f,%8.4f,%8.4f,%8.4f\n"
                "%08.4f,%8.4f,%8.4f,%8.4f\n"
                "%08.4f,%8.4f,%8.4f,%8.4f\n",
            mat[0],mat[1],mat[2],mat[3],
            mat[4],mat[5],mat[6],mat[7],
            mat[8],mat[9],mat[10],mat[11],
            mat[12],mat[13],mat[14],mat[15]);
    gltSetText(textid, str);
    gltDrawText2D(textid, x,y,1.0);
}
