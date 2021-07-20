#include "../include/voxel.h"

void voxel_set_children_to_null(Voxel* voxel){
    for(int i = 0; i < 8; i++){
        voxel->child[i] = NULL;
    }
}
int voxel_contains_point(Voxel voxel, unsigned int x, unsigned int y, unsigned int z){
    if(voxel.size == 1){
        return (x >= voxel.origin[0]) && (x < voxel.origin[0] + 1) &&
            (y >= voxel.origin[1]) && (x < voxel.origin[1] + 1) &&
            (z >= voxel.origin[2]) && (x < voxel.origin[2] + 1);
    }
    int hs = voxel.size / 2;
    int minX = voxel.origin[0] - hs;
    int maxX = voxel.origin[0] + hs;
    int minY = voxel.origin[1] - hs;
    int maxY = voxel.origin[1] + hs;
    int minZ = voxel.origin[2] - hs;
    int maxZ = voxel.origin[2] + hs;
    return (x >= minX) && (x < maxX) &&
        (y >= minY) && (y < maxY) &&
        (z >= minZ) && (z < maxZ);


}
Aabb voxel_to_aabb(Voxel voxel){
    
    Aabb aabb;
    if(voxel.size == 1){
        aabb.x = voxel.origin[0];
        aabb.y = voxel.origin[1];
        aabb.z = voxel.origin[2];
        aabb.width = 1;
        aabb.height = 1;
        aabb.depth = 1;
    }else{
        int hs = voxel.size / 2;
        aabb.x = voxel.origin[0] - hs;
        aabb.y = voxel.origin[1] - hs;
        aabb.z = voxel.origin[2] - hs;
        aabb.width = voxel.origin[0] + hs;
        aabb.height = voxel.origin[1] + hs;
        aabb.depth = voxel.origin[2] + hs;
    }
    return aabb;
    
}

int voxel_is_point_inside_model(Voxel *voxel, float x, float y, float z){
    if(voxel == NULL){
        return 0;
    }
    Aabb voxBox = voxel_to_aabb(*voxel);
    if(voxel_is_leaf(voxel)){
        if(aabb_contains_point(voxBox, x, y, z)){
            return 1;
        }
    }
    if(aabb_contains_point(voxBox, x, y, z)){
        for(int i = 0; i < 8; i++){
            if(voxel->child[i] != NULL){
                if(voxel_is_point_inside_model(voxel->child[i], x, y, z)){
                    return 1;
                }
            }
        }
    }
    
    return 0;
}

void voxel_vs_ray (Voxel *voxel, Ray ray, int *entityType, int *hit, int *side, float* hitDistance)
{
    if(voxel != NULL){
        Aabb voxBox = voxel_to_aabb(*voxel);
        if(voxel_is_leaf(voxel)){
            int boxHit = *hit, boxSide;
            float boxDistance = *hitDistance;
            aabb_vs_ray(voxBox, ray, &boxHit, &boxSide, &boxDistance);
            
            if(boxHit){
                if((boxDistance < (*hitDistance)) && (boxDistance > 0)){
                    *hitDistance = boxDistance;
                    *side = boxSide;
                    *hit = 1;
                    *entityType = ENTITY_TYPE_VOXEL_MODEL;       
                }
            }
        }else{
            if(aabb_contains_point(voxBox, ray.x, ray.y, ray.z)){
                for(int i = 0; i < 8; i++){
                    if(voxel->child[i] != NULL){
                        voxel_vs_ray(voxel->child[i], ray, entityType, hit, side, hitDistance);
                    }
                }
            }else{
                int boxHit = 0, boxSide;
                float boxDistance;
                aabb_vs_ray(voxBox, ray, &boxHit, &boxSide, &boxDistance);
                if(boxHit){
                    if((boxDistance < (*hitDistance)) && (boxDistance > 0)){
                        for(int i = 0; i < 8; i++){
                            if(voxel->child[i] != NULL){
                                voxel_vs_ray(voxel->child[i], ray, entityType, hit, side, hitDistance);
                            }
                        }
                    }
                }
            }
        }
    }
}

int voxel_is_inside_larger_voxel(Voxel voxel, unsigned int x, unsigned int y, unsigned int z){
    
    int halfSize = voxel.size / 2;
    int minX = voxel.origin[0] - halfSize;
    int maxX = voxel.origin[0] + halfSize;
    int minY = voxel.origin[1] - halfSize;
    int maxY = voxel.origin[1] + halfSize;
    int minZ = voxel.origin[2] - halfSize;
    int maxZ = voxel.origin[2] + halfSize;
    return (x >= minX) && (x < maxX) && 
        (y >= minY) && (y < maxY) && 
        (z >= minZ) && (z < maxZ);
}

int voxel_is_leaf(Voxel *voxel){
    if(voxel == NULL){
        return 0;
    }
    if(voxel->size == 1){
        return 1;
    }
    int nullcount = 0;
    for(int i = 0; i < 8; i++){
        nullcount += voxel->child[i] == NULL;
    }
    return (nullcount == 8);
}

void voxel_get_leaf_voxels(Voxel *voxel, Voxel **voxelArray, int *index){
    if(voxel != NULL){
        if(voxel_is_leaf(voxel)){
            voxelArray[*index] = voxel;
            (*index)+= 1;    
        }else{
            for(int i = 0; i < 8; i++){
                if(voxel->child[i] != NULL){
                    voxel_get_leaf_voxels(voxel->child[i], voxelArray, index);
                }
            }
        }
    }
}

int voxel_count_voxels(Voxel* voxel){
    if(voxel == NULL){
        return 0;
    }

    if(voxel_is_leaf(voxel)){
        return 1;
    }

    int childCount = 0;
    for(int i = 0; i < 8; i++){
        if(voxel->child[i] != NULL){
            childCount += voxel_count_voxels(voxel->child[i]);
        }
    }
    return childCount;
}

void voxel_combine(Voxel** voxel){
    if ((*voxel)->size == 2){

        int childcount = 0;
        for(int i = 0; i < 8; i++){
            childcount += (*voxel)->child[i] != NULL;
        }
        if(childcount == 8){
            for(int i = 0; i < 8; i++){
                free((*voxel)->child[i]);
                (*voxel)->child[i] = NULL;
            }
        }
    }else{
        for(int i = 0; i < 8; i++){
            if((*voxel)->child[i] != NULL){
                voxel_combine(&(*voxel)->child[i]);
            }
        }
        int childcount = 0;
        for(int i = 0; i < 8; i++){
            childcount += (*voxel)->child[i] != NULL;
        }

        if(childcount == 8){
            int nullcount = 0;
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                   nullcount += (*voxel)->child[i]->child[j] == NULL ;
                }
            }
            if(nullcount == 64){
                for(int i = 0; i < 8; i++){
                    free((*voxel)->child[i]);
                    (*voxel)->child[i] = NULL;
                }
            }
        }
    }
}
void voxel_compress_voxel(Voxel** voxel){
    if((*voxel) != NULL){
        if( (*voxel)->size != 1){
            if((*voxel)->child[0] != NULL){
                int nullcount = 0;
                for(int i = 1; i < 8; i++){
                    nullcount += (*voxel)->child[i] == NULL;
                }
                if(nullcount == 7){
                    *voxel = (*voxel)->child[0];
                    voxel_compress_voxel(&(*voxel));
                }
            }  
        }  
    }
}
void voxel_delete_voxel(Voxel** voxel, unsigned int x, unsigned int y, unsigned int z){
    voxel_delete_voxel_do(voxel,x,y,z);
    voxel_compress_voxel(voxel);
}

int voxel_is_left_from_index(int i){
    return (i < 4);
}
int voxel_is_bottom_from_index(int i){
    return (i == 0) || (i == 1) || (i == 4) || (i == 5);
}
int voxel_is_front_from_index(int i){
    return (i == 0) || (i == 2) || (i == 4) || (i == 6);
}
void voxel_delete_voxel_do(Voxel** voxel, unsigned int x, unsigned int y, unsigned int z){
    if(*voxel != NULL){
        if((*voxel)->size == 1){
            if(x == (*voxel)->origin[0] &&
                y == (*voxel)->origin[1] &&
                z == (*voxel)->origin[2]){
                    free(*voxel);
                    *voxel = NULL;
            }
        }else if((*voxel)->size == 2){  
            if(voxel_contains_point(**voxel, x, y, z)){
                if(voxel_is_leaf(*voxel)){
                    printf("reconstruct the voxel before deleting \n");
                    for(int i = 0; i < 8; i++){

                        (*voxel)->child[i] = (Voxel*)malloc(sizeof(Voxel));
                        (*voxel)->child[i]->size = 1;
                        (*voxel)->child[i]->origin[0] = (*voxel)->origin[0] - (i < 4);
                        (*voxel)->child[i]->origin[1] = (*voxel)->origin[1] - 
                        ((i == 0) || (i == 1) || (i == 4) || (i == 5));
                        (*voxel)->child[i]->origin[2] = (*voxel)->origin[2] - (
                            (i == 0) || (i == 2) || (i == 4) || (i == 6)
                        );
                        voxel_set_children_to_null((*voxel)->child[i]);
                    }
                }
                int px = x >= (*voxel)->origin[0];
                int py = y >= (*voxel)->origin[1];
                int pz = z >= (*voxel)->origin[2];
                int n = 4 * px + 2 * py + pz;
                printf("delete voxel at %d\n", n);
                if((*voxel)->child[n] != NULL){
                    free((*voxel)->child[n]);
                    (*voxel)->child[n] = NULL;
                }
                
                int nullCount = 0;
                for(int i = 0; i < 8; i++){
                    nullCount += (*voxel)->child[i] == NULL;
                }
                printf("nullcount  %d\n", nullCount);
                if (nullCount == 8)
                {
                    free(*voxel);
                    *voxel = NULL;
                }
                
            }
        }else{
            if(voxel_contains_point(**voxel, x, y, z)){
                if(voxel_is_leaf(*voxel)){
                    for(int i = 0; i < 8; i++){
                        (*voxel)->child[i] = (Voxel*)malloc(sizeof(Voxel));
                        int qs = (*voxel)->size / 4;
                        (*voxel)->child[i]->size = (*voxel)->size / 2;
                        (*voxel)->child[i]->origin[0] = (*voxel)->origin[0] + (-qs * (i < 4)) + (qs * (i >= 4));
                        int pred = voxel_is_bottom_from_index(i);
                         
                        (*voxel)->child[i]->origin[1] = (*voxel)->origin[1] + (-qs * pred) + (qs * !pred);
    
                        pred = voxel_is_front_from_index(i);
                    
                        (*voxel)->child[i]->origin[2] = (*voxel)->origin[2] + (-qs * pred) + (qs * !pred);
                        voxel_set_children_to_null((*voxel)->child[i]);
                    }
                }
                int px = x >= (*voxel)->origin[0];
                int py = y >= (*voxel)->origin[1];
                int pz = z >= (*voxel)->origin[2];
                int n = 4 * px + 2 * py + pz;
                voxel_delete_voxel(&(*voxel)->child[n], x, y, z);
                
                int nullcount = 0;
                for(int i = 0; i < 8; i++){
                    nullcount += (*voxel)->child[i] == NULL;
                }
                if(nullcount == 8){
                    free(*voxel);
                    *voxel = NULL;
                }
            }
        }
    }
}

void voxel_add_voxel_from_aabb(Voxel** voxel, Aabb aabb){

}
void voxel_delete_from_aabb(Voxel** voxel, Aabb aabb){
    
}
void voxel_init_origin_voxel(Voxel **voxel){
    *voxel = (Voxel*) malloc(sizeof(Voxel));
    (*voxel)->size = 1;
    (*voxel)->origin[0] = 0;
    (*voxel)->origin[1] = 0;
    (*voxel)->origin[2] = 0; 
}

int voxel_at_origin(unsigned int x, unsigned int y,unsigned int z){
    return (x + y + z) == 0;
}
void voxel_init_voxel_size_2(Voxel **voxel){
    (*voxel)->size = 2;
    (*voxel)->origin[0] = 1;
    (*voxel)->origin[1] = 1;
    (*voxel)->origin[2] = 1;
}
void voxel_double_voxel_size(Voxel **voxel){
    (*voxel)->size = (*voxel)->size * 2;
    (*voxel)->origin[0] = (*voxel)->origin[0] * 2;            
    (*voxel)->origin[1] = (*voxel)->origin[1] * 2;
    (*voxel)->origin[2] = (*voxel)->origin[2] * 2;
}

void voxel_double_voxel_head(Voxel **voxel){
    Voxel* newHead = (Voxel*) malloc(sizeof(Voxel));
    newHead->size = (*voxel)->size * 2;
    newHead->origin[0] = (*voxel)->origin[0] * 2;
    newHead->origin[1] = (*voxel)->origin[1] * 2;
    newHead->origin[2] = (*voxel)->origin[2] * 2;
    voxel_set_children_to_null(newHead);   
    newHead->child[0] = *voxel;
    *voxel = newHead;
}

void voxel_compress_new_voxel_at_size_2(Voxel **voxel){
    int childcount = 0;
    for(int i = 0; i < 8; i++){
        childcount += (*voxel)->child[i] != NULL;
    }
    if(childcount == 8){
        for(int i = 0; i < 8; i++){
            free((*voxel)->child[i]);
            (*voxel)->child[i] = NULL;
        }
    }
}
void voxel_compress_new_voxels(Voxel ** voxel){
    int childcount = 0;
    for(int i = 0; i < 8; i++){
        childcount += (*voxel)->child[i] != NULL;
    }
    if(childcount == 8){
        int nullcount = 0;
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                nullcount += (*voxel)->child[i]->child[j] == NULL;
            }
        }
        if(nullcount == 64){
            for(int i = 0; i < 8; i++){
                free((*voxel)->child[i]);
                (*voxel)->child[i] = NULL;
            }
        }
    }
}
void voxel_add_voxel_node(Voxel **voxel, int px, int py, int pz, int n, 
    unsigned int x, unsigned int y, unsigned int z){
    
}

int voxel_contains_sphere(Voxel voxel, Sphere sphere){
    vec3 sphereMax = {sphere.x + sphere.r, 
        sphere.y + sphere.r, 
        sphere.z + sphere.r
    };
    Aabb voxBox = voxel_to_aabb(voxel);
    if(aabb_contains_point(voxBox, sphereMax[0], sphereMax[1], sphereMax[2])){
        return 1;
    }
    return 0;
}

Aabb voxel_aabb_from_parent(unsigned int index, unsigned int size, int origin[3]){
    Aabb box;
    int isLeft = voxel_is_left_from_index(index);
    int isBottom = voxel_is_bottom_from_index(index);
    int isFront = voxel_is_front_from_index(index);
    int hs = size / 2;
    if(hs == 1){
        box.x = (float)(origin[0]) - isLeft;
        box.y = (float)(origin[1]) - isBottom;
        box.z = (float)(origin[2]) - isFront;
        box.width = 1;
        box.height = 1;
        box.depth = 1;
    }else{  

        box.x = (float)(origin[0]) + (-isLeft * hs);
        box.y = (float)(origin[1]) + (-isBottom * hs);
        box.z = (float)(origin[2]) + (-isFront * hs);
        box.width = hs;
        box.height = hs;
        box.depth = hs;
    }
    return box;
}
void voxel_expand_empty_until_sphere_fits(Voxel** voxel, Sphere sphere){
    *voxel = (Voxel*) malloc(sizeof(Voxel));
    (*voxel)->size = 2;
    (*voxel)->origin[0] = 1;
    (*voxel)->origin[1] = 1;
    (*voxel)->origin[2] = 1;
    voxel_set_children_to_null(*voxel);

    int i = 0;
    while(!voxel_contains_sphere(**voxel, sphere) && i < 100){
        voxel_double_voxel_size(voxel);    
        i++;
    }
}
void voxel_expand_head_until_sphere_fits(Voxel** voxel, Sphere sphere){
    int i = 0;
    while(!voxel_contains_sphere(**voxel, sphere) && i < 100){
        voxel_double_voxel_head(voxel);    
        i++;
    }
    

}
void voxel_add_voxel_from_sphere_s2(Voxel* voxel, Sphere sphere){
    if(voxel->size == 2)
    {
        for(int i = 0; i < 8; i++){
            //vec3 midpoint;
            int isLeft = voxel_is_left_from_index(i);
            int isBottom = voxel_is_bottom_from_index(i);
            int isFront = voxel_is_front_from_index(i);
            vec3 min;
            min[0] = voxel->origin[0] + (-isLeft * 1);
            min[1] = voxel->origin[1] + (-isBottom * 1);
            min[2] = voxel->origin[2] + (-isFront * 1);
            Aabb box;
            box.x = min[0];
            box.y = min[1];
            box.z = min[2];
            box.width = 1;
            box.height = 1;
            box.depth = 1;
            int aabbIntersects = aabb_intersects_sphere(box, sphere);
            if(aabbIntersects){
                 if(voxel->child[i] == NULL){
                    voxel->child[i] = (Voxel*) malloc(sizeof(Voxel));
                    Voxel* child = voxel->child[i];
                    child->size = 1;
                    int qs = voxel->size / 4;
                    int isleft = voxel_is_left_from_index(i);
                    int isBottom = voxel_is_bottom_from_index(i);
                    int isFront = voxel_is_front_from_index(i);
                    child->origin[0] = voxel->origin[0] - isleft;
                    child->origin[1] = voxel->origin[1] - isBottom;
                    child->origin[2] = voxel->origin[2] - isFront;
                    voxel_set_children_to_null(child);
                }

            }
        }
    }else{
        for(int i = 0; i < 8; i++){
            Aabb voxBox = voxel_aabb_from_parent(i, voxel->size, voxel->origin);
            int aabbInside = aabb_inside_sphere(voxBox, sphere);
            int aabbIntersects = aabb_intersects_sphere(voxBox, sphere);
            if(aabbInside || aabbIntersects){
                if(voxel->child[i] == NULL){
                    voxel->child[i] = (Voxel*) malloc(sizeof(Voxel));
                    Voxel* child = voxel->child[i];
                    child->size = voxel->size / 2;
                    int qs = voxel->size / 4;
                    int isleft = voxel_is_left_from_index(i);
                    int isBottom = voxel_is_bottom_from_index(i);
                    int isFront = voxel_is_front_from_index(i);
                    child->origin[0] = voxel->origin[0] + (-isleft * qs) + (!isleft * qs);
                    child->origin[1] = voxel->origin[1] + (-isBottom * qs) + (!isBottom * qs);
                    child->origin[2] = voxel->origin[2] + (-isFront *qs) + (!isFront * qs);
                    voxel_set_children_to_null(child);
                }
                if(aabbIntersects && !aabbInside){
                    voxel_add_voxel_from_sphere_s2(voxel->child[i], sphere);
                } 
            }
        }                     
    } 
}
void voxel_add_voxel_from_sphere(Voxel** voxel, Sphere sphere){
    if(*voxel == NULL){
        voxel_expand_empty_until_sphere_fits(voxel, sphere);
    }else{
        voxel_expand_head_until_sphere_fits(voxel, sphere);
    }
    voxel_add_voxel_from_sphere_s2(*voxel, sphere);    
    voxel_combine(voxel);
}

void voxel_expand_empty_until_point_fits(Voxel** voxel, unsigned int x, unsigned int y, unsigned int z){
    *voxel = (Voxel*) malloc(sizeof(Voxel));
    (*voxel)->size = 2;
    (*voxel)->origin[0] = 1;
    (*voxel)->origin[1] = 1;
    (*voxel)->origin[2] = 1;
    voxel_set_children_to_null(*voxel);
    int i = 0;
    while (!voxel_is_inside_larger_voxel(**voxel,x,y,z) && i < 100)  
    {
        voxel_double_voxel_size(voxel);
        i++;   
    }
}
void voxel_expand_head_until_point_fits(Voxel** voxel, unsigned int x, unsigned int y, unsigned int z){
    int i = 0;
    while (!voxel_is_inside_larger_voxel(**voxel,x,y,z) && i < 100)
    {
        voxel_double_voxel_head(voxel);
        i++;
    }

}
void voxel_add_voxel_s2(Voxel *voxel, unsigned int x, unsigned int y, unsigned int z){
    int px, py, pz, n;  
    px = x >= voxel->origin[0];
    py = y >= voxel->origin[1];
    pz = z >= voxel->origin[2];
    n = 4 * px + 2 * py + pz;
    if(voxel->size == 2)
    {
        if(voxel->child[n] == NULL)
        {
            voxel->child[n] = (Voxel*) malloc(sizeof(Voxel));
            voxel->child[n]->size = 1;
            voxel->child[n]->origin[0] = x;
            voxel->child[n]->origin[1] = y;
            voxel->child[n]->origin[2] = z;
            voxel_set_children_to_null(voxel->child[n]);
        }   
    }else
    {
        if(voxel->child[n] == NULL)
        {
            voxel->child[n] = (Voxel*) malloc(sizeof(Voxel));
            voxel->child[n]->size = voxel->size / 2;
            int qs = voxel->size / 4;
            voxel->child[n]->origin[0] = voxel->origin[0] + (-!px * qs) + (px * qs);
            voxel->child[n]->origin[1] = voxel->origin[1] + (-!py * qs) + (py * qs);
            voxel->child[n]->origin[2] = voxel->origin[2] + (-!pz * qs) + (pz * qs);
            voxel_set_children_to_null(voxel->child[n]);
        }
        voxel_add_voxel_s2(voxel->child[n], x, y, z);
    } 
}
void voxel_add_voxel(Voxel **voxel, unsigned int x, unsigned int y, unsigned int z){
    if(*voxel == NULL){
        voxel_expand_empty_until_point_fits(voxel, x, y, z);
    }else{
        voxel_expand_head_until_point_fits(voxel, x, y, z);
    }
    voxel_add_voxel_s2(*voxel, x, y, z);
    voxel_combine(voxel);



}

void voxel_generate_cube(Voxel *voxel,int index, float *verts, unsigned int *elements)
{
    unsigned int e[36] = 
    {
        4,5,1, 4,1,0,
        1,5,6, 1,6,2,
        0,1,2, 0,2,3,
        4,0,3, 4,3,7,
        3,2,6, 3,6,7,
        7,6,5, 7,5,4
    };
    int shift  = index * 8;
    for(int i = 0; i < 36; i++)
    {        
        e[i] = e[i] + shift;
    }
    memcpy(elements, e, sizeof(unsigned int) * 36);

    if(voxel->size == 1)
    {
        int x = voxel->origin[0];
        int y = voxel->origin[1];
        int z = voxel->origin[2];
        float v[24] = {
            x, y, z,
            x, y + 1, z,
            x + 1, y + 1, z,
            x + 1, y, z,
            x, y, z + 1,
            x, y + 1, z + 1,
            x + 1, y + 1, z + 1,
            x + 1, y, z + 1
        };
        memcpy(verts, v, sizeof(float) * 24);
    }else
    {
        int x = voxel->origin[0];
        int y = voxel->origin[1];
        int z = voxel->origin[2];
        int hs = voxel->size / 2;
        float v[24] = 
        {
            x - hs, y - hs, z - hs,
            x - hs, y + hs, z - hs,
            x + hs, y +  hs, z - hs,
            x + hs, y - hs, z - hs,
            x - hs, y - hs, z + hs,
            x - hs, y + hs, z + hs,
            x + hs, y + hs, z + hs,
            x + hs, y - hs, z + hs
        };
        memcpy(verts, v, sizeof(float) * 24);
    } 
}

void voxel_get_verticies_and_indicies(Voxel *voxel, float *vertexArray ,unsigned int *elementArray)
{
    
    int voxelCount = voxel_count_voxels(voxel);
    if(voxelCount > 0)
    {    
        Voxel *leafVoxels[voxelCount];
        int index = 0;
        voxel_get_leaf_voxels(voxel, leafVoxels, &index);          
        int elementArrayIndex = 0;
        int vertexArrayIndex = 0;

        for(int i = 0; i < voxelCount; i++)
        {
            vertexArrayIndex = i * 24;
            elementArrayIndex = i * 36;
            float verts[24];
            unsigned int elements[36];
            voxel_generate_cube(leafVoxels[i], i, verts, elements);
            memcpy(&vertexArray[vertexArrayIndex], verts, sizeof(float) * 24);
            memcpy(&elementArray[elementArrayIndex], elements, sizeof(unsigned int) * 36);
            
        }
    }
}