#include "../include/raycast.h"
#define MAX_RAYCAST 0xFFFFFFF;



void ray_vs_aabb(Aabb box, Ray ray, int *didHit, vec3 hitpoint,  int *sideHit){
    *didHit = 0;
    float left = box.x;
    float right = box.x + box.width;
    float bottom = box.y;
    float top = box.y + box.height;
    float front = box.z ;
    float back = box.z + box.depth;
    vec3 rayStart = {ray.x, ray.y, ray.z};
    vec3 rayDirection = {ray.dx, ray.dy, ray.dz};
    int bestDistance = MAX_RAYCAST ;
    vec3 bestHitPoint;
    int leftTarget = ray.dx > 0;
    int rightTarget = ray.dx < 0;
    int bottomTarget = ray.dy > 0;
    int topTarget = ray.dy < 0;
    int frontTarget = ray.dz > 0; 
    int backTarget = ray.dz < 0; 

    if(leftTarget){
        vec3 hitPointLeft;        
        float d = (left - ray.x) / ray.dx;        
        vec3_scale(hitPointLeft, rayDirection, d);
        vec3_add(hitPointLeft,hitPointLeft, rayStart);
        float hitX = hitPointLeft[0];
        float hitY = hitPointLeft[1];
        float hitZ = hitPointLeft[2];
        int hit = hitY >= bottom && hitY < top && hitZ >= front && hitZ < back;
        if(hit){
            bestHitPoint[0] = hitX;
            bestHitPoint[1] = hitY;
            bestHitPoint[2] = hitZ;
            bestDistance = d;
            *didHit = 1;
            *sideHit = BOX_SIDE_LEFT;
        }
           
    }else if(rightTarget){
        vec3 hitPointRight;        
        float d = (right - ray.x) / ray.dx;
        vec3_scale(hitPointRight, rayDirection, d);
        vec3_add(hitPointRight, hitPointRight, rayStart);
        float hitX = hitPointRight[0];
        float hitY = hitPointRight[1];
        float hitZ = hitPointRight[2];
        int hit = hitY >= bottom && hitY < top && hitZ >= front && hitZ < back;
        if(hit){
            bestHitPoint[0] = hitX;
            bestHitPoint[1] = hitY;
            bestHitPoint[2] = hitZ;
            bestDistance = d;
            *didHit = 1;
            *sideHit = BOX_SIDE_RIGHT;
        }
    }
    
    
    if(frontTarget){
        vec3 hitPointFront;        
        float d = (front - ray.z) / ray.dz;
        if(d < bestDistance){
            vec3_scale(hitPointFront, rayDirection, d);
            vec3_add(hitPointFront, hitPointFront, rayStart);
            float hitX = hitPointFront[0];
            float hitY = hitPointFront[1];
            float hitZ = hitPointFront[2];
            int hit = hitX >= left && hitX < right && hitY >= bottom && hitY < top;
            if(hit){
                bestHitPoint[0] = hitX;
                bestHitPoint[1] = hitY;
                bestHitPoint[2] = hitZ;
                bestDistance = d;
                *didHit = 1;
                *sideHit = BOX_SIDE_FRONT;

            }
        }


    }else if(backTarget){
        vec3 hitPointBack;        
        float d = (back - ray.z) / ray.dz;
        if(d < bestDistance){
            vec3_scale(hitPointBack, rayDirection, d);
            vec3_add(hitPointBack, hitPointBack, rayStart);
            float hitX = hitPointBack[0];
            float hitY = hitPointBack[1];
            float hitZ = hitPointBack[2];
            int hit = hitX >= left && hitX < right && hitY >= bottom && hitY < top;
            if(hit){
                bestHitPoint[0] = hitX;
                bestHitPoint[1] = hitY;
                bestHitPoint[2] = hitZ;
                bestDistance = d;
                *didHit = 1;
                *sideHit = BOX_SIDE_BACK;
            }
        }
    }
   

    if(bottomTarget){
        vec3 hitPointBottom;        
        float d = (bottom - ray.y) / ray.dy;
        if(d <= bestDistance){
            vec3_scale(hitPointBottom, rayDirection, d);
            vec3_add(hitPointBottom, hitPointBottom, rayStart);
            float hitX = hitPointBottom[0];
            float hitY = hitPointBottom[1];
            float hitZ = hitPointBottom[2];
            int hit = hitX >= left && hitX < right && hitZ >= front && hitZ < back;
            if(hit){
                bestHitPoint[0] = hitX;
                bestHitPoint[1] = hitY;
                bestHitPoint[2] = hitZ;
                bestDistance = d;
                *didHit = 1;
                *sideHit = BOX_SIDE_BOTTOM;
            }
        }
    }else if(topTarget){
        vec3 hitPointTop;        
        float d = (top - ray.y) / ray.dy;
        if(d <= bestDistance){
            vec3_scale(hitPointTop, rayDirection, d);
            vec3_add(hitPointTop, hitPointTop, rayStart);
            float hitX = hitPointTop[0];
            float hitY = hitPointTop[1];
            float hitZ = hitPointTop[2];
            int hit = hitX >= left && hitX < right && hitZ >= front && hitZ < back;
            if(hit){
                bestHitPoint[0] = hitX;
                bestHitPoint[1] = hitY;
                bestHitPoint[2] = hitZ;
                bestDistance = d;
                *didHit = 1;
                *sideHit = BOX_SIDE_TOP;
            }
        }
    }
    if(*didHit){
        hitpoint[0] = bestHitPoint[0];
        hitpoint[1] = bestHitPoint[1];
        hitpoint[2] = bestHitPoint[2];
    }
}

void ray_vs_voxel_grid(Voxel* voxel, Ray ray, int *didHit, vec3 hitpoint, int *sideHit){
    if(voxel->size == 1){
        Aabb voxBox;
        vec3 hitpoint;
        voxBox = voxel_to_aabb(*voxel);
        int boxHit = 0;
        int boxSide;
        vec3 boxHitPoint;
        ray_vs_aabb(voxBox, ray, &boxHit, boxHitPoint, &boxSide);
        
        if(boxHit){
            vec3 rayStart = {ray.x, ray.y, ray.z};
            vec3 vectorToHitpoint, vectorToBox;
            vec3_sub(vectorToHitpoint, hitpoint, rayStart);
            vec3_sub(vectorToBox, boxHitPoint, rayStart);
            float distanceToBox = vec3_mul_inner(vectorToBox, vectorToBox);
            float distanceToHitpoint = vec3_mul_inner(vectorToHitpoint, vectorToHitpoint);
        
            if(distanceToBox < distanceToHitpoint || *didHit == 0){
                hitpoint[0] = boxHitPoint[0];
                hitpoint[1] = boxHitPoint[1];
                hitpoint[2] = boxHitPoint[2];
                *didHit = 1;
                *sideHit = boxSide;
                
            }
        }
    }

    int nullCount = 0;
    for(int i = 0; i < 8; i++){
        nullCount += voxel->children[i] == NULL;
    }

    if(nullCount == 8){ 
        Aabb voxBox;
        vec3 hitpoint;
        voxBox = voxel_to_aabb(*voxel);
        int boxHit;
        int boxSide;
        vec3 boxHitPoint;

        ray_vs_aabb(voxBox, ray, &boxHit, boxHitPoint, &boxSide);
        if(boxHit){
            vec3 rayStart = {ray.x, ray.y, ray.z};
            vec3 vectorToHitpoint, vectorToBox;
            vec3_sub(vectorToHitpoint, hitpoint, rayStart);
            vec3_sub(vectorToBox, boxHitPoint, rayStart);
            float distanceToBox = vec3_mul_inner(vectorToBox, vectorToBox);
            float distanceToHitpoint = vec3_mul_inner(vectorToHitpoint, vectorToHitpoint);
        
            if(distanceToBox < distanceToHitpoint || *didHit == 0){
                hitpoint[0] = boxHitPoint[0];
                hitpoint[1] = boxHitPoint[1];
                hitpoint[2] = boxHitPoint[2];
                *didHit = 1;
                *sideHit = boxSide;
                
            }
        }
     
    }else{
        for(int i = 0; i < 8; i++){
            if(voxel->children[i] != NULL){
                Aabb box;
                box = voxel_to_aabb(*(voxel->children[i]));
                if(aabb_contains_point(box, ray.x, ray.y, ray.z)){

                    ray_vs_voxel_grid(voxel->children[i], ray, didHit, hitpoint, sideHit);
                }else{
                    int hit;
                    int side;
                    ray_vs_aabb(box, ray,&hit, hitpoint, &side);
                    if(hit){
                        ray_vs_voxel_grid(voxel->children[i], ray, didHit, hitpoint, sideHit);
                    }
                }
            }
        }
    }

    
    
}

