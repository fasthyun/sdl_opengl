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
in vec2  TexCoord;
in vec4  Color;
in vec3  Normal;
in float Type;
in vec3  FragPos;


uniform sampler2D ourTexture; // how working???
//uniform Material material;
uniform vec3 lightPos;

void main()
{

    //vec3 lightPos = vec3(0,30000,0);
    vec3 lightColor = vec3(1.0,1.0,1.0);

    // ambient
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    //vec4 _color;
    vec4 result;
    ///FragColor = texture2D(ourTexture, TexCoord); // 130
    if (Type > 1.5 ){
        result =  Color;
        //result =  vec4(1,0,0,1);
    }
    else   if (Type >= 0.5 ) // color
    {
        //_color = vec4(Color, 1.0); // color mode
        result = vec4((ambient + diffuse),1.0) * Color;
    }    
    else if (Type < 0.5) // texture
    {
        result = vec4((ambient + diffuse),1.0) * texture(ourTexture, TexCoord) ;
    }

    FragColor = result;
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
