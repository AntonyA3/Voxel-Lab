#version 320 es
precision mediump float;
in vec4 fcolor;
in vec3 fposition;

out vec4 diffuseColor;
void main()
{
    vec4 color = fcolor;
    float intensity = 0.4 * float(fposition.x < 0.0 || fposition.y < 0.0 || fposition.z < 0.0) + 1.0 * float(fposition.x >= 0.0 && fposition.y >= 0.0 && fposition.z >= 0.0);

    color.r = intensity * color.r;
    color.g = intensity * color.g;
    color.b = intensity * color.b;
    diffuseColor = color;
}
