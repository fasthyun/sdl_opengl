#version 330
uniform mat4 projView;  // from main.draw()
uniform mat4 modelView;  // from xObject.draw()

in vec3 vPos;      // from  xObject.draw() glDrawElements()
in vec4 vColor;   // RGB

out vec4 ourColor;
//out vec2 TexCoord;

void main()
{
    gl_Position = projView * modelView * vec4(vPos, 1.0);
    ourColor = vColor; // RGB
}
