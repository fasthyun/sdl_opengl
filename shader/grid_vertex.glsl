#version 330

uniform mat4 projView;  // from main.draw()
uniform mat4 modelView;  // from xObject.draw()

//uniform mat4 gl_ModelViewProjectionMatrix;
in vec3 vertex;
///attribute vec4 qt_MultiTexCoord0;
//varying vec4 qt_TexCoord0;
//layout (location = 0) in vec4 vertex;

void main(void)
{
    //gl_Position = gl_ModelViewProjectionMatrix * vertex;
    //gl_Position =  vertex;
    gl_Position = projView * modelView * vec4(vertex, 1.0);
//    qt_TexCoord0 = qt_MultiTexCoord0;
}

