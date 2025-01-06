// ----------------------------------------------------------------
// From Game Programming in C++ by Sanjay Madhav
// Copyright (C) 2017 Sanjay Madhav. All rights reserved.
// 
// Released under the BSD License
// See LICENSE in root directory for full details.
// ----------------------------------------------------------------

// Request GLSL
#version 330 core
// This corresponds to the output color
// to the color buffer

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

#uniform sampler2D ourTexture;
uniform Material material;

in vec4 ourColor;

out vec4 FragColor;


void main()
{
    //FragColor = texture2D(ourTexture, TexCoord); // 130
    //FragColor = texture(ourTexture, TexCoord); // 330
    FragColor = ourColor;
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
