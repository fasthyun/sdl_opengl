
#version 330
uniform mat4 projView;  // from main.draw()
uniform mat4 model;  // from xObject.draw()

layout (location = 0) in vec3 vPos;       //layout (location = 0) in vec3 vPos;
layout (location = 1) in vec2 vTexCoord;  //layout (location = 1) in vec2 vTexCoord;
layout (location = 2) in vec4 vColor;     //layout (location = 2)
layout (location = 3) in vec3 vNormal;    //layout (location = 3) vertex normal
layout (location = 4) in int  vType;      //layout (location = 4) int32  works!!! color, texture, nolight

out vec2 TexCoord;
out vec4 Color;
out vec3 Normal;
out vec3 FragPos;
flat out int Type;   // int ?

void main()
{
    gl_Position = projView * model * vec4(vPos, 1.0);
    TexCoord = vTexCoord; // ---> vec2(vTexCoord.x, vTexCoord.y);

    Normal = vNormal;
    //Normal = vec3(model * vec4(vNormal, 1.0));
    //Normal = mat3(transpose(inverse(model))) * vNormal;
    Type = vType ;//* 1.0; // color mode
    if (vType==0) // texture!!
    {
        Color = vec4(1.0,0.0,0.0,0.5);  //texture!!!
    }else
    {
        Color = vColor;    //Color = vec4(1.0,0.0,0.0,0.5);
    }
    //Color = vec4(1.0,0.0,0.0,0.5);  //texture!!!
    FragPos = vec3(model * vec4(vPos, 1.0));
}
