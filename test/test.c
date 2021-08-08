#include <stdio.h>
#include <stdlib.h>
#include "../include/linmath.h"
#include "../include/aabb.h"
#include "../include/voxel_model.h"
#include "../include/voxel.h"

void aabb_overlap_test(){
    
    Aabb aabb1 = {0, 0, 0, 10, 10, 10};
    Aabb aabb2 = {-5,3,3,10,10,10};
    vec3 overlap;
    aabb_get_overlaping_area(aabb1, aabb2, overlap);

    printf("overlap test %f %f %f\n", overlap[0], overlap[1], overlap[2]);    
}

void test_aabb_box_subtract(){
    Aabb box = {2,2,2,4,4,4};
    Aabb subBox = aabb_get_box_with_subtracted_corners(box, 0.1);
}

void test_aabb_from_voxel_child(){
    Voxel voxel;
    voxel.origin[0] = 16;
    voxel.origin[1] = 16;
    voxel.origin[2] = 16;
    voxel.size = 32;
    
    Aabb voxbox = voxel_to_aabb_from_child_i(voxel, 0);
    printf ("voxbox, %f, %f, %f, %f, %f, %f\n", voxbox.x, voxbox.y, voxbox.z, voxbox.w, voxbox.h, voxbox.d);
    voxbox = voxel_to_aabb_from_child_i(voxel, 1);
    printf ("voxbox, %f, %f, %f, %f, %f, %f\n", voxbox.x, voxbox.y, voxbox.z, voxbox.w, voxbox.h, voxbox.d);
    voxbox = voxel_to_aabb_from_child_i(voxel, 2);
    printf ("voxbox, %f, %f, %f, %f, %f, %f\n", voxbox.x, voxbox.y, voxbox.z, voxbox.w, voxbox.h, voxbox.d);
    voxbox = voxel_to_aabb_from_child_i(voxel, 3);
    printf ("voxbox, %f, %f, %f, %f, %f, %f\n", voxbox.x, voxbox.y, voxbox.z, voxbox.w, voxbox.h, voxbox.d);
    voxbox = voxel_to_aabb_from_child_i(voxel, 4);
    printf ("voxbox, %f, %f, %f, %f, %f, %f\n", voxbox.x, voxbox.y, voxbox.z, voxbox.w, voxbox.h, voxbox.d);
    voxbox = voxel_to_aabb_from_child_i(voxel, 5);
    printf ("voxbox, %f, %f, %f, %f, %f, %f\n", voxbox.x, voxbox.y, voxbox.z, voxbox.w, voxbox.h, voxbox.d);
    voxbox = voxel_to_aabb_from_child_i(voxel, 6);
    printf ("voxbox, %f, %f, %f, %f, %f, %f\n", voxbox.x, voxbox.y, voxbox.z, voxbox.w, voxbox.h, voxbox.d);
    voxbox = voxel_to_aabb_from_child_i(voxel, 7);
    printf ("voxbox, %f, %f, %f, %f, %f, %f\n", voxbox.x, voxbox.y, voxbox.z, voxbox.w, voxbox.h, voxbox.d);
  


}


void test_aabb_get_corners(){
    Aabb box = {2,2,2,4,4,4};
    vec3 corners[8];
    aabb_get_box_corners(box, corners);

    for(int i = 0; i < 8; i++){
        printf("corner %f %f %f\n", corners[i][0], corners[i][1], corners[i][2]);
    }


}
int main(int argc, char const *argv[])
{
    //aabb_overlap_test();
    //test_aabb_from_voxel_child();
    //test_aabb_get_corners();
    test_aabb_box_subtract();
    return 0;
}
