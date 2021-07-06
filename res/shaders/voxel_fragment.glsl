#version 320 es

precision mediump float;

in vec3 fvPos;
out vec4 FragColor;
void main()
{ 
    bool x = mod(fvPos.x,1.0) < 0.1;
    bool y = mod(fvPos.y,1.0) < 0.1;
    bool z = mod(fvPos.z,1.0) < 0.1;
    float outline = float(x && y);
    FragColor = vec4(1.0 * outline,1.0,0.0,1.0);
}