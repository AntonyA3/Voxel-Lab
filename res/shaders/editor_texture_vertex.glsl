#version 320 es
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    gl_Position = vec4(aPos.x/2.0, aPos.y/2.0, 0.0, 1.0); 
    TexCoords = aTexCoords;
}  