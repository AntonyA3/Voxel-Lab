#version 320 es
layout (location = 0) in vec3 vposition;
layout (location = 1) in vec4 vcolor;


uniform mat4 uProjMat;
uniform mat4 uViewMat;
out vec4 fcolor;
out vec3 fposition;

void main()
{
    fcolor = vcolor;
    fposition = vposition;
    gl_Position = uProjMat * uViewMat * vec4(vposition,1.0);
}
