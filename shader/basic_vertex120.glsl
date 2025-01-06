#version 130

uniform mat4 gl_ModelViewProjectionMatrix;
attribute vec4 gl_Vertex;
//in vec3 inPosition;
//layout(location=0) in vec4 vPosition;
//layout(location=1) in vec4 vColor;
//in vec4 vColor;
//out vec4 outcolor;
varying vec4 vertexColor;
void main()
{
	// The vertex shader needs to output a 4D
	// coordinate.
	// For now set the 4th coordinate to 1.0
	//gl_Position = vec4(inPosition, 1.0);
	gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
        vertexColor = gl_Color;
        //gl_FrontColor = gl_Color;
}
/*
#version 130
//uniform mat4 gl_ModelViewProjectionMatrix;
attribute vec4 vertex;
///attribute vec4 qt_MultiTexCoord0;
//varying vec4 qt_TexCoord0;
//layout (location = 0) in vec4 vertex;

void main(void)
{
    gl_Position = gl_ModelViewProjectionMatrix * vertex;
    gl_Position =  vertex;
}
*/
