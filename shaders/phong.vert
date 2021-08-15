
#version 320 es

layout (location = 0) in vec3 vposition;
layout (location = 1) in vec4 vcolor;
layout (location = 2) in vec3 vnormal;

uniform mat4 uProjMat;
uniform mat4 uViewMat;
uniform mat4 uModelMat;

out vec4 fcolor;
out vec3 fnormal;

void main()
{
    fcolor = vcolor;
    fnormal = vnormal;
    gl_Position = uProjMat * uViewMat * uModelMat * vec4(vposition, 1.0);
}

