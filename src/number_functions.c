#include "../include/number_functions.h"

float minf(float a, float b){
    return a * (a <= b) + b * (b < a); 
}
float maxf(float a, float b){
    return a * (a >= b) + b * (b > a); 
}

float clampf(float min, float value, float max){
    return minf(maxf(value, min), max);
}

float degToRad(float deg){
    return deg / 180 * M_PI;
}

float radToDeg(float rad){
    return rad / M_PI * 180;
}