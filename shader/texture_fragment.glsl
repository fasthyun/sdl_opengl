// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL

// This corresponds to the output color
// to the color buffer

#version 330 core

/* struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
*/

out vec4 FragColor;
in vec2 TexCoord;
uniform sampler2D ourTexture;

//uniform Material material;




void main()
{
    ///FragColor = texture2D(ourTexture, TexCoord); // 130
    FragColor = texture(ourTexture, TexCoord); // 330
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
