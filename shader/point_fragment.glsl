// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

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

out vec4 FragColor;

void main()
{
    FragColor = vec4(0.9,0.9,0.9,1);
}

