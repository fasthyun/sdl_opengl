#version 330

uniform mat4 projView;  // from main.draw()
uniform mat4 modelView;  // from xObject.draw()

in vec3 vertex;
//layout (location = 0) in vec3 vertex;

void main(void)
{
    //gl_Position = gl_ModelViewProjectionMatrix * vertex;
    //gl_Position =  vertex;
    gl_Position = projView * modelView * vec4(vertex, 1.0);
//    qt_TexCoord0 = qt_MultiTexCoord0;
}

