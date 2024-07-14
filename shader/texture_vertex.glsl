/*
#version 130
//uniform mat4 gl_ModelViewProjectionMatrix;
attribute vec4 vertex;
///attribute vec4 qt_MultiTexCoord0;
//varying vec4 qt_TexCoord0;
//layout (location = 0) in vec4 vertex;

void main(void)
{
    //gl_Position = gl_ModelViewProjectionMatrix * vertex;
    //gl_Position =  vertex;
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
//    qt_TexCoord0 = qt_MultiTexCoord0;
}

*/
#version 130
uniform mat4 projView;
uniform mat4 modelView;
//layout (location = 0) in vec3 vPos;
//layout (location = 1) in vec2 vTexCoord;
attribute vec3 vPos;
attribute vec2 vTexCoord;

//out vec3 ourColor;
out vec2 TexCoord;

void main()
{
    gl_Position = projView * modelView* vec4(vPos, 1.0);
    TexCoord = vTexCoord; // vec2(aTexCoord.x, aTexCoord.y);
}
