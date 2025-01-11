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
#version 330
uniform mat4 projView;  // from main.draw()
uniform mat4 model;  // from xObject.draw()
//layout (location = 0) in vec3 vPos;
//layout (location = 1) in vec2 vTexCoord;
in vec3 vPos;       // from  xObject.draw() glDrawElements()
in vec2 vTexCoord;  //
in vec4 vColor;     //
in vec3 vNormal;     // normal
in int  vType;      // 32bit testing

//out vec3 ourColor;
out vec2 TexCoord;
out vec4 Color;
out vec3 Normal;
out float Type;
out vec3 FragPos;

void main()
{
    gl_Position = projView * model * vec4(vPos, 1.0);
    TexCoord = vTexCoord; // ---> vec2(aTexCoord.x, aTexCoord.y);
    Color = vColor;
    Normal = vNormal;
    //Normal = vec3(model * vec4(vNormal, 1.0));
    //Normal = mat3(transpose(inverse(model))) * vNormal;

    if ( vType > 0 )
        Type=3.0; // color mode
    else
        Type=0.0; // texture mode

    FragPos = vec3(model * vec4(vPos, 1.0));
}
