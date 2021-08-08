

#version 320 es
precision mediump float;
uniform vec3 uDirectionLight;
uniform float uAmbientLight;
uniform float uLightIntensity;
in vec4 fcolor;
in vec3 fnormal;
out vec4 color;

void main(){

    float diffuse = dot(uDirectionLight, fnormal);  
    vec4 ambientColor = vec4(fcolor.xyz * uAmbientLight, 1.0);
    vec4 diffuseColor = fcolor * diffuse * uLightIntensity;

    vec4 finalColor = max(diffuseColor, ambientColor);
    color = finalColor;
}