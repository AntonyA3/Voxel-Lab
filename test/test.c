#include <stdlib.h>
#include <stdio.h>
#include "linmath.h"
#include "../include/prim_fact.h"
#include "../include/ray.h"

#define RAY_UNION_EXAMPLE

void test_cube_generator(){
    vec3 start = {0,0,-10};
    vec3 end = {0,0,100};
    Vertex boxVertex[8];
    unsigned int boxElements[36];
    generate_cube_from_line(start, end, 1, boxVertex, boxElements);

    for(int i = 0; i < 8; i++){
        Vertex vertex = boxVertex[i];
        printf("vertex index: i: %d x: %f, y: %f, z: %f\n", i, vertex.x, vertex.y, vertex.z);
        
    }
    
}

void test_ray_union(){
    Ray ray; 
    init_ray(&ray, 5,5,5,1,1,1);

    printf("from ray.ray x: %f, y: %f, z: %f, dx: %f, dy: %f, dz: %f\n",  
        ray.ray[0], ray.ray[1], ray.ray[2], ray.ray[3], ray.ray[4], ray.ray[5]);
    printf("from ray.xyzdxdydz  x: %f, y: %f, z: %f, dx: %f, dy: %f, dz: %f\n",
    ray.x, ray.y, ray.z, ray.dx, ray.dy, ray.dz);
    printf("from ray.origin/dir  x: %f, y: %f, z: %f, dx: %f, dy: %f, dz: %f\n",
        ray.origin[0], ray.origin[1], ray.origin[2], 
        ray.direction[0], ray.direction[1], ray.direction[2]
    );
    printf("\nevaluate example\n");


}

int main(int argc, char const *argv[])
{
    #ifdef TEST_CUBE_GENERATOR
        test_cube_generator();
    #endif
    #ifdef RAY_UNION_EXAMPLE
        test_ray_union();
    #endif
    return 0;
}
