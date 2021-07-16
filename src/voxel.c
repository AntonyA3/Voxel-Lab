#include "../include/voxel.h"

void voxel_set_children_to_null(Voxel* voxel){
    for(int i = 0; i < 8; i++){
        voxel->child[i] = NULL;
    }
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

void voxel_add_voxel(Voxel** voxel, unsigned int x, unsigned int y, unsigned int z){
    if(*voxel == NULL)
    {
        *voxel = (Voxel*) malloc(sizeof(Voxel));
        (*voxel)->size = 1;
        (*voxel)->origin[0] = 0;
        (*voxel)->origin[1] = 0;
        (*voxel)->origin[2] = 0; 
        if((x + y + z) != 0 )
        {
            (*voxel)->size = 2;
            (*voxel)->origin[0] = 1;
            (*voxel)->origin[1] = 1;
            (*voxel)->origin[2] = 1;

            int i = 0;
            while (!voxel_is_inside_larger_voxel(**voxel,x,y,z) && i < 100)  
            {
                (*voxel)->size = (*voxel)->size * 2;
                (*voxel)->origin[0] = (*voxel)->origin[0] * 2;            
                (*voxel)->origin[1] = (*voxel)->origin[1] * 2;
                (*voxel)->origin[2] = (*voxel)->origin[2] * 2;
            }
            voxel_set_children_to_null(*voxel);
            voxel_add_voxel(voxel, x, y, z);
        }
    }else
    {
        if((*voxel)->size == 1)
        {
            Voxel* newHead = (Voxel*) malloc(sizeof(Voxel));
            newHead->size = 2;
            newHead->origin[0] = 1;
            newHead->origin[1] = 1;
            newHead->origin[2] = 1;
            voxel_set_children_to_null(newHead);
            
            newHead->child[0] = *voxel;
            *voxel = newHead;
        
        }
        int i = 0;
        while (!voxel_is_inside_larger_voxel(**voxel,x,y,z) && i < 100)
        {
            Voxel* newHead = (Voxel*) malloc(sizeof(Voxel));
            newHead->size = (*voxel)->size * 2;
            newHead->origin[0] = (*voxel)->origin[0] * 2;
            newHead->origin[1] = (*voxel)->origin[1] * 2;
            newHead->origin[2] = (*voxel)->origin[2] * 2;
            voxel_set_children_to_null(newHead);   
            
            newHead->child[0] = *voxel;
            *voxel = newHead;
            i++;
        }

        int px, py, pz, n;  
        px = x >= (*voxel)->origin[0];
        py = y >= (*voxel)->origin[1];
        pz = z >= (*voxel)->origin[2];
        n = 4 * px + 2 * py + pz;
    
        if((*voxel)->size == 2)
        {
            if((*voxel)->child[n] == NULL)
            {
                (*voxel)->child[n] = (Voxel*) malloc(sizeof(Voxel));
                (*voxel)->child[n]->size = 1;
                (*voxel)->child[n]->origin[0] = x;
                (*voxel)->child[n]->origin[1] = y;
                (*voxel)->child[n]->origin[2] = z;
                voxel_set_children_to_null((*voxel)->child[n]);
            }
            
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
           
        }else
        {
            if((*voxel)->child[n] == NULL)
            {
                (*voxel)->child[n] = (Voxel*) malloc(sizeof(Voxel));
                (*voxel)->child[n]->size = (*voxel)->size / 2;
                int qs = (*voxel)->size / 4;
                (*voxel)->child[n]->origin[0] = (*voxel)->origin[0] + (-!px * qs) + px * qs;
                (*voxel)->child[n]->origin[1] = (*voxel)->origin[1] + (-!py * qs) + py * qs;
                (*voxel)->child[n]->origin[2] = (*voxel)->origin[2] + (-!pz * qs) + pz * qs;
                voxel_set_children_to_null((*voxel)->child[n]);
            }
            voxel_add_voxel(&((*voxel)->child[n]), x, y, z);

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
    }

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