#include "../include/ray.h"

void ray_point_at_distance(Ray ray, float distance, vec3 point){
    vec3_scale(point, ray.direction, distance);
    vec3_add(point, point, ray.origin);
}
int ray_vs_aabb(Ray ray, Aabb aabb, float *distance, int *side){
    float x0 = aabb.x;
    float x1 = aabb.x + aabb.width;
    float y0 = aabb.y;
    float y1 = aabb.y + aabb.height;
    float z0 = aabb.z;
    float z1 = aabb.z + aabb.depth;

    float d = 0;
    vec3 point;
    int hit = 0;
    if(ray.x < x0 && ray.dx > 0){
        d = (x0 - ray.x) /ray.dx;
        if(d < *distance){
            ray_point_at_distance(ray, d, point);
            if(point[1] >= y0 && point[1] < y1 && point[2] >= z0 && point[2] < z1){
                *distance = d;
                hit = 1;
                *side = RAY_HIT_SIDE_LEFT;
            }    
        }
        
    }

    if(ray.x > x1 && ray.dx < 0){
        d = (x1 - ray.x) /ray.dx;
        if(d < *distance){
            ray_point_at_distance(ray, d, point);
            if(point[1] >= y0 && point[1] < y1 && point[2] >= z0 && point[2] < z1){
                *distance = d;
                hit = 1;
                *side = RAY_HIT_SIDE_RIGHT;
            }  
        }

    }

    if(ray.y < y0 && ray.dy > 0){
        d = (y0 - ray.y) /ray.dy;
        if(d < *distance){
            ray_point_at_distance(ray, d, point);
            if(point[0] >= x0 && point[0] < x1 && point[2] >= z0 && point[2] < z1){
                *distance = d;
                hit = 1;
                *side = RAY_HIT_SIDE_BOTTOM;
            }  
        }

    }

    if(ray.y > y1 && ray.dy < 0){
        d = (y1 - ray.y) /ray.dy;
        if(d < *distance){
            ray_point_at_distance(ray, d, point);
            if(point[0] >= x0 && point[0] < x1 && point[2] >= z0 && point[2] < z1){
                *distance = d;
                hit = 1;
                *side = RAY_HIT_SIDE_TOP;
            }  
        }

    }

    if(ray.z < z0 && ray.dz > 0){
        d = (z0 - ray.z) /ray.dz;
        if(d < *distance){
            ray_point_at_distance(ray, d, point);
            if(point[0] >= x0 && point[0] < x1 && point[1] >= y0 && point[1] < y1){
                *distance = d;
                hit = 1;
                *side = RAY_HIT_SIDE_FRONT;
            }  
        }

    }

    if(ray.z > z1 && ray.dz < 0){
        d = (z1 - ray.z) /ray.dz;
        if(d < *distance){
            ray_point_at_distance(ray, d, point);
            if(point[0] >= x0 && point[0] < x1 && point[1] >= y0 && point[1] < y1){
                *distance = d;
                hit = 1;
                *side = RAY_HIT_SIDE_BACK;
            }  
        }
    }
    return hit;
}

int ray_vs_voxel_tree(Ray ray, VoxelTree *voxelTree, float *distance, int *side){
    
    if(voxelTree->head != NULL){
        return ray_vs_voxel(ray, voxelTree->head, distance, side);
    }
    return 0;
}

int ray_vs_voxel(Ray ray, Voxel *voxel, float *distance, int *side){
    Aabb voxBox = voxel_to_aabb(*voxel);
    switch (voxel->size)
    {
    case 1:
        return ray_vs_aabb(ray, voxel_to_aabb(*voxel), distance, side);
        break;
    default:
        {
            float voxDistance = *distance;
            int voxSide;
            int hit = ray_vs_aabb(ray, voxBox, &voxDistance, &voxSide);
            int contains = aabb_contains_point(voxBox, ray.x, ray.y, ray.z);
            printf("voxel at size %d hit is %d \n", voxel->size, hit);
            
            if(hit || contains){
                switch (voxel_is_leaf(voxel))
                {
                case 1:
                    if(hit && !contains){
                        *distance = voxDistance;
                        *side = voxSide;
                    }
                    return hit && !contains;
                default:   
                    {                  
                        int hit = 0;
                        for(int i = 0; i < VOXEL_CHILD_COUNT; i++){
                            if(voxel->child[i] != NULL){
                                int localHit = ray_vs_voxel(ray, voxel->child[i], distance, side);
                                hit = localHit || hit;
                            }
                        }
                        return hit;
                    }
                }
            }   
            
        }
        break;
    }
}

int ray_vs_y0(Ray ray, float *distance, int *side){
    int pred = ((ray.direction[1] > 0) && (ray.origin[1] < 0)) ||
        ((ray.direction[1]) < 0  && (ray.origin[1] > 0));
    *side = RAY_HIT_SIDE_BOTTOM * ((ray.direction[1] > 0) && (ray.origin[1] < 0)) +
        RAY_HIT_SIDE_TOP *  ((ray.direction[1]) < 0  && (ray.origin[1] > 0));
    if(pred){
        float d = -ray.origin[1] / ray.direction[1];
        if(d < *distance){
            *distance = d;
            return 1;
        }
    }
    return 0;
}