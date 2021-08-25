#include "../include/grid.h"

void grid_init(Grid *grid, vec3 min, vec3 vecH, vec3 vecV, int hCount, int vCount){
    memcpy(grid->min, min, sizeof(vec3));
    memcpy(grid->vectorH, vecH, sizeof(vec3));
    memcpy(grid->vectorV, vecV, sizeof(vec3));
    grid->hCount = hCount;
    grid->vCount = vCount;
}