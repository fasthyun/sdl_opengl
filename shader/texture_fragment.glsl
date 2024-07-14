// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL
#version 130

// This corresponds to the output color
// to the color buffer

uniform sampler2D ourTexture;

//in vec3 ourColor;
//in vec2 TexCoord;
//attribute vec3 ourColor;
varying vec2 TexCoord;

out vec4 FragColor;


void main()
{
    FragColor = texture2D(ourTexture, TexCoord);
    // FragColor = texture(ourTexture, TexCoord);
}

/*
uniform sampler2D tex;
uniform vec2 tex_size;

layout(location = 0) out vec4 out_color;

void main()
{
    vec4 in_color = texture(tex, gl_FragCoord.xy / tex_size);
    out_color = //do whatever you want with in_color;
}
*/
