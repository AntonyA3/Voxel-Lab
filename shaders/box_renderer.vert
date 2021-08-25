#version 320 es

layout (location = 0) in vec2 vPosition;
layout (location = 1) in vec3 vColor;
layout (location = 2) in vec3 iOffset;
layout (location = 3) in vec3 iExtents;
layout (location = 4) in vec3 iColor;

out vec3 fColor;

void main()
{
    gl_Position = vec4((vPosition * iExtents) + vOffset, 0.0, 1.0);
    fColor = vColor * iColor;
}  