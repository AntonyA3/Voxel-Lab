#version 320 es
layout (location = 0) in vec3 vposition;
uniform mat4 uProjMat;
uniform mat4 uViewMat;

void main()
{
    gl_Position = uProjMat * uViewMat * vec4(vposition,1.0);
}
