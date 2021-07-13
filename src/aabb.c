#include "../include/aabb.h"

int aabb_contains_point(Aabb box, float x, float y, float z){
    int minX = box.x;
    int maxX = box.x + box.width;
    int minY = box.y;
    int maxY = box.y + box.height;
    int minZ = box.z;
    int maxZ = box.z + box.depth;
    return x >= minX && x < maxX && y >= minY && y < maxY &&
        z >= minZ && z < maxZ;
}