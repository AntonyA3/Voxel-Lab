#version 320 es
layout (location = 0) in vec3 vPos;
uniform mat4 uProjMat;
uniform mat4 uViewMat;

out vec3 fvPos;
void main()
{
    fvPos = vPos;
    gl_Position = uProjMat * uViewMat * vec4(vPos, 1.0); 
}  