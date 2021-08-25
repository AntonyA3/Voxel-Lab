#include "../include/mathplus.h"

float clampf(float min, float value, float max){
    return fmaxf(fminf(value, max), min);
}