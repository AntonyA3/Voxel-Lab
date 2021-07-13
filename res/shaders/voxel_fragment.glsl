#version 320 es

precision mediump float;

in vec3 fvPos;
out vec4 FragColor;
void main()
{ 
    vec3 col = fvPos;
    col.r = mod(col.b, 0.5);
    col.g = mod(col.g, 0.5);
    col.b = mod(col.b, 0.5);
    
    FragColor = vec4(col, 1.0);
}