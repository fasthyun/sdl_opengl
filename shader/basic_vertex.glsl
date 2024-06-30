// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------
#version 120
// This should correspond to the data stored
// for each vertex in the vertex buffer.
// For now, just a position.
//uniform mat4 gl_ModelViewProjectionMatrix;
//attribute vec4 gl_Vertex;
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
// Request GLSL 3.3
#version 330

//layout(location = 0) in vec4 in_position;
in vec3 vec;
void main()
{
    gl_Position = vec4(vec,1.0);
} */
