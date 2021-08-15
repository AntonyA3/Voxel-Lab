#version 320 es

layout (location = 0) in vec3 vposition;
layout (location = 1) in vec4 vcolor;

uniform mat4 uProjMat;
uniform mat4 uViewMat;
uniform mat4 uModelMat;

out vec4 fcolor;

void main()
{

    fcolor = vcolor;
    gl_Position = uProjMat * uViewMat * uModelMat * vec4(vposition, 1.0);
}
