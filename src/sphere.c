#include "../include/sphere.h"

int sphere_contains_point(Sphere sphere, float x, float y, float z){
    vec3 point = {x,y,z};
    vec3 spherePoint = {sphere.x, sphere.y, sphere.z};
    vec3 toPoint;
    vec3_sub(toPoint,point, spherePoint);
    return vec3_mul_inner(toPoint, toPoint) < (sphere.r * sphere.r);
}

int sphere_contains_point_inclusive(Sphere sphere, float x, float y, float z){
    vec3 point = {x,y,z};
    vec3 spherePoint = {sphere.x, sphere.y, sphere.z};
    vec3 toPoint;
    vec3_sub(toPoint,point, spherePoint);
    return vec3_mul_inner(toPoint, toPoint) <= (sphere.r * sphere.r);

}

void sphere_get_box_max_point(Sphere sphere, vec3 point){
    point[0] = sphere.x + sphere.r;
    point[1] = sphere.y + sphere.r;
    point[2] = sphere.z + sphere.r;
}

void sphere_get_overlapping_area(Sphere sphere, Aabb aabb, vec3 overlap){
    
}


int sphere_intersects_aabb(Sphere sphere, Aabb aabb){
    vec3 aabbCentre;
    aabb_get_centre(aabb, aabbCentre);
    if(sphere_contains_point_inclusive(sphere, aabbCentre[0], aabbCentre[1], aabbCentre[2])){
        return 1;
    }
    vec3 sphereCentre = {sphere.x, sphere.y, sphere.z};

    if(aabb_contains_point_inclusive(aabb, sphereCentre[0], sphereCentre[1], sphereCentre[2])){
        return 1;
    }
    vec3 sphereToBox;
    vec3_sub(sphereToBox, aabbCentre, sphereCentre);

    float d, x, y, z;
    if(sphereToBox[0] != 0){
        if(sphereToBox[0] > 0){
            d = (aabb.x - sphere.x) /sphereToBox[0];
            x = aabb.x;
        }
        if(sphereToBox[0] < 0){
            d = (aabb.x + aabb.w - sphere.x) /sphereToBox[0];
            x = aabb.x + aabb.w;
        } 
        y = clampf(
            aabb.y, 
            sphere.y + d * sphereToBox[1],
            aabb.y + aabb.h
        );
        z = clampf(aabb.z, 
            sphere.z + d * sphereToBox[2],
            aabb.z + aabb.d
        );
        if(sphere_contains_point_inclusive(sphere, x, y, z)){
            return 1;      
        }
    }

    if(sphereToBox[1] != 0){
        if(sphereToBox[1] > 0){
            d = (aabb.y - sphere.y) /sphereToBox[1];
            y = aabb.y;
        }
        if(sphereToBox[1] < 0){
            d = (aabb.y + aabb.h - sphere.y) /sphereToBox[1];
            y = aabb.y + aabb.h;
        } 
        x = clampf(
            aabb.x, 
            sphere.x + d * sphereToBox[0],
            aabb.x + aabb.w
        );
        z = clampf(aabb.z, 
            sphere.z + d * sphereToBox[2],
            aabb.z + aabb.d
        );
        if(sphere_contains_point_inclusive(sphere, x, y, z)){
            return 1;      
        }
    }

    if(sphereToBox[2] != 0){
        if(sphereToBox[2] > 0){
            d = (aabb.z - sphere.z) /sphereToBox[2];
            z = aabb.z;

        }
        if(sphereToBox[2] < 0){
            d = (aabb.z + aabb.d - sphere.z) /sphereToBox[2];
            z = aabb.z + aabb.d;
        }
        y = clampf(
            aabb.y, 
            sphere.y + d * sphereToBox[1],
            aabb.y + aabb.h
        );
        z = clampf(aabb.z, 
            sphere.z + d * sphereToBox[2],
            aabb.z + aabb.d
        );
        if(sphere_contains_point_inclusive(sphere, x, y, z)){
            return 1;      
        }

    }

    
    return 0;
}

Aabb sphere_to_aabb(Sphere sphere){
    float d = sphere.r * 2;
    float r = sphere.r;
    Aabb aabb = {
        sphere.x - r, sphere.y - r, sphere.z - r, 
        d,d,d
    };
    return aabb;
}