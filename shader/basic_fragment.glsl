// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL 3.3
#version 330

// This corresponds to the output color
// to the color buffer
/*
out vec4 outColor;
uniform vec4 inColor;

void main()
{
	// RGBA of 100% blue, 100% opaque
    outColor = inColor;
}

*/
in vec4  vertexColor;
out vec4 fragColor;

void main()
{
    fragColor = vertexColor;
    //fragColor = gl_FrontColor;
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
