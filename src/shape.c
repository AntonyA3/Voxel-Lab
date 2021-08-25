#include "../include/shape.h"

float AABB_VERTEX_ARRAY[FLOATS_PER_CUBE] = {
    //left
    -1.0f, -1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
    //top
    -1.0f, 1.0f, -1.0f,
    -1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, 1.0f, -1.0f,
    //front    
    -1.0f, -1.0f, -1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    //bottom
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,
    //right
    1.0f, -1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
    1.0f, -1.0f, 1.0f,
    //back
    1.0f, -1.0f, 1.0f,
    1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, 1.0f,
    -1.0f, -1.0f, 1.0f
};

float AABB_NORMAL_ARRAY[FLOATS_PER_CUBE] = {
    //left
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    -1.0f, 0.0f, 0.0f,
    //top
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    0.0f, 1.0f, 0.0f,
    //front
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    0.0f, 0.0f, -1.0f,
    //bottom
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    0.0f, -1.0f, 0.0f,
    //right
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    //back
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f
};

unsigned AABB_ELEMENTS_ARRAY[ELEMENTS_PER_CUBE] = {
    0, 1, 2,        0, 2, 3,
    4, 5, 6,        4, 6, 7,
    8, 9, 10,       8, 10, 11,
    12, 13, 14,     12, 14, 15,
    16, 17, 18,     16, 18, 19,
    20, 21, 22,     20, 22, 23
};

Rect rect_init(float x, float y, float width, float height){
    Rect rect;
    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;
    return rect;
}

int rect_contains_point(Rect rect, float x, float y){
    return (x >= rect.x) && (x <= (rect.x + rect.width)) &&
        (y >= rect.y) && (y <= (rect.y + rect.height));      
}

int shape_intersects_aabb(int shape, void* shapeData, Aabb aabb){
    switch (shape)
    {
    case SHAPE_AABB:
        {
        Aabb aabb1;
        memcpy(&aabb1, shapeData, sizeof(Aabb));
        return aabb_intersects_aabb(aabb, aabb1);
        }
    case SHAPE_SPHERE:
        {
            Sphere sphere;
            memcpy(&sphere, shapeData, sizeof(Sphere));
            return sphere_intersects_aabb(sphere, aabb);
        }
    }
    return 0;
}

int shape_contains_point(int shape, void* shapeData, float x, float y, float z){
    switch (shape)
    {
    case SHAPE_AABB:
        {            
            Aabb aabb;
            memcpy(&aabb, shapeData, sizeof(Aabb));
            return aabb_contains_point(aabb, x, y, z);
        }
    case SHAPE_SPHERE:
        {
            Sphere sphere;
            memcpy(&sphere, shapeData, sizeof(Sphere));
            return sphere_contains_point(sphere, x, y, z);
        }
        return 0;
    }
    return 0;
}

Aabb shape_to_aabb(int shape, void* shapeData){
    switch (shape)
    {
    case SHAPE_AABB:
        {    
            Aabb aabb;
            memcpy(&aabb, shapeData, sizeof(Aabb));
            return aabb;
        }
        break;      
    case SHAPE_SPHERE:
        {
            Sphere sphere;
            memcpy(&sphere, shapeData, sizeof(Sphere));
            return sphere_to_aabb(sphere);
        }
        break;
    }
}





int aabb_intersects_aabb(Aabb aabb1, Aabb aabb2){
    vec3 aabb1Center;
    aabb_get_origin(aabb1, aabb1Center);
    vec3 aabb2Center;
    aabb_get_origin(aabb2, aabb2Center);

    float dx = fabsf(aabb1Center[0] - aabb2Center[0]);
    float dy = fabsf(aabb1Center[1] - aabb2Center[1]);
    float dz = fabsf(aabb1Center[2] - aabb2Center[2]);

    return (dx <= ((aabb1.width * 0.5) + (aabb2.width * 0.5))) && 
        (dy <= ((aabb1.height * 0.5) + (aabb2.height * 0.5))) &&
        (dz <= ((aabb1.depth * 0.5) + (aabb2.depth * 0.5)));
}
int aabb_contains_point(Aabb aabb, float x, float y, float z){
    return (x >= aabb.x) && (x <= (aabb.x + aabb.width)) &&
        (y >= aabb.y) && (y <= (aabb.y + aabb.height)) &&
        (z >= aabb.z) && (z <= (aabb.z + aabb.depth));
}

Aabb aabb_from_origin_halfExtents(vec3 origin, vec3 halfExtents){
    Aabb aabb;
    vec3_sub(aabb.min, origin, halfExtents);
    vec3_scale(aabb.size, halfExtents, 2.0);
    return aabb;
}
Aabb aabb_from_expand(Aabb aabb, float expansion){
    float expanse2 = expansion * 2;
    Aabb aabb1 ={
        aabb.x - expansion,
        aabb.y - expansion,
        aabb.z - expansion,
        aabb.width + expanse2,
        aabb.height + expanse2,
        aabb.depth + expanse2
    };
    return aabb1;
}

void aabb_get_corners(Aabb aabb, vec3 corners[8]){
    for(int i = 0; i < 8; i++){
        memcpy(&corners[i], aabb.min, sizeof(vec3));
    }
    corners[1][1] += aabb.height;

    corners[2][0] += aabb.width;
    corners[2][1] += aabb.height;

    corners[3][0] += aabb.width;

    corners[4][2] += aabb.depth;

    corners[5][1] += aabb.height;
    corners[5][2] += aabb.depth;

    corners[6][0] += aabb.width;
    corners[6][1] += aabb.height;
    corners[6][2] += aabb.depth;

    corners[7][0] += aabb.width;
    corners[7][2] += aabb.depth;    
}

void aabb_get_origin(Aabb aabb, vec3 origin){
    vec3 halfExtents;
    vec3_scale(halfExtents, aabb.size, 0.5);
    vec3_add(origin, aabb.min, halfExtents);
}

int sphere_intersects_aabb(Sphere sphere, Aabb aabb){
    vec3 aabbCentre;
    aabb_get_origin(aabb, aabbCentre);
    if(sphere_contains_point(sphere, aabbCentre[0], aabbCentre[1], aabbCentre[2])){
        return 1;
    }
    vec3 sphereCentre = {sphere.x, sphere.y, sphere.z};

    if(aabb_contains_point(aabb, sphereCentre[0], sphereCentre[1], sphereCentre[2])){
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
            d = (aabb.x + aabb.width - sphere.x) /sphereToBox[0];
            x = aabb.x + aabb.width;
        } 
        y = clampf(
            aabb.y, 
            sphere.y + d * sphereToBox[1],
            aabb.y + aabb.height
        );
        z = clampf(aabb.z, 
            sphere.z + d * sphereToBox[2],
            aabb.z + aabb.depth
        );
        if(sphere_contains_point(sphere, x, y, z)){
            return 1;      
        }
    }

    if(sphereToBox[1] != 0){
        if(sphereToBox[1] > 0){
            d = (aabb.y - sphere.y) /sphereToBox[1];
            y = aabb.y;
        }
        if(sphereToBox[1] < 0){
            d = (aabb.y + aabb.height - sphere.y) /sphereToBox[1];
            y = aabb.y + aabb.height;
        } 
        x = clampf(
            aabb.x, 
            sphere.x + d * sphereToBox[0],
            aabb.x + aabb.width
        );
        z = clampf(aabb.z, 
            sphere.z + d * sphereToBox[2],
            aabb.z + aabb.depth
        );
        if(sphere_contains_point(sphere, x, y, z)){
            return 1;      
        }
    }
    

    if(sphereToBox[2] != 0){
        if(sphereToBox[2] > 0){
            d = (aabb.z - sphere.z) /sphereToBox[2];
            z = aabb.z;

        }
        if(sphereToBox[2] < 0){
            d = (aabb.z + aabb.depth - sphere.z) /sphereToBox[2];
            z = aabb.z + aabb.depth;
        }
        y = clampf(
            aabb.y, 
            sphere.y + d * sphereToBox[1],
            aabb.y + aabb.height
        );
        z = clampf(aabb.z, 
            sphere.z + d * sphereToBox[2],
            aabb.z + aabb.depth
        );
        if(sphere_contains_point(sphere, x, y, z)){
            return 1;      
        }

    }
    return 0;
}

int sphere_contains_point(Sphere sphere, float x, float y, float z){
    vec3 point = {x, y, z};
    vec3 pointToSphere;
    vec3_sub(pointToSphere, point, sphere.position);
    return vec3_mul_inner(pointToSphere, pointToSphere) <= sphere.r * sphere.r;
}

Aabb sphere_to_aabb(Sphere sphere){
    float d = 2 * sphere.r;
    Aabb aabb = {sphere.x - sphere.r, sphere.y - sphere.r, sphere.z - sphere.r,
        d,d,d
    };
    return aabb;
}

int ovoid_contains_point(Ovoid ovoid, float x, float y, float z){
    vec3 vectorToPoint;
    vec3 directionToPoint;
    vec3 point = {x, y, z};
    vec3_sub(vectorToPoint, point, ovoid.origin);
    vec3_norm(directionToPoint, vectorToPoint);
    float r = 
        sin(abs(directionToPoint[0]) * M_PI_2) * ovoid.halfExtents[0] + 
        sin(abs(directionToPoint[1]) * M_PI_2) * ovoid.halfExtents[1] + 
        sin(abs(directionToPoint[2]) * M_PI_2) * ovoid.halfExtents[2]; 
    return vec3_mul_inner(vectorToPoint, vectorToPoint) <= r * r;
}

int ovoid_intersects_aabb(Ovoid ovoid, Aabb aabb){
    if(aabb_contains_point(aabb, ovoid.origin[0], ovoid.origin[1], ovoid.origin[2])){
        return 1;
    }

    vec3 aabbCentre;
    vec3 ovoidToBox;
    aabb_get_origin(aabb, aabbCentre);
    vec3_sub(ovoidToBox, aabbCentre, ovoid.origin);

    float d, x, y, z;
    if(ovoidToBox[0] != 0){
        if(ovoidToBox[0] > 0){
            d = (aabb.x - ovoid.origin[0]) /ovoidToBox[0];
            x = aabb.x;
        }
        if(ovoidToBox[0] < 0){
            d = (aabb.x + aabb.width - ovoid.origin[0]) /ovoidToBox[0];
            x = aabb.x + aabb.width;
        } 
        y = clampf(
            aabb.y, 
            ovoid.origin[1] + d * ovoidToBox[1],
            aabb.y + aabb.height
        );
        z = clampf(aabb.z, 
            ovoid.origin[2] + d * ovoidToBox[2],
            aabb.z + aabb.depth
        );
        if(ovoid_contains_point(ovoid, x, y, z)){
            return 1;      
        }
    }

    if(ovoidToBox[1] != 0){
        if(ovoidToBox[1] > 0){
            d = (aabb.y - ovoid.origin[1]) /ovoidToBox[1];
            y = aabb.y;
        }
        if(ovoidToBox[1] < 0){
            d = (aabb.y + aabb.height - ovoid.origin[1]) /ovoidToBox[1];
            y = aabb.y + aabb.height;
        } 
        x = clampf(
            aabb.x, 
            ovoid.origin[0] + d * ovoidToBox[0],
            aabb.x + aabb.width
        );
        z = clampf(aabb.z, 
            ovoid.origin[2] + d * ovoidToBox[2],
            aabb.z + aabb.depth
        );
        if(ovoid_contains_point(ovoid, x, y, z)){
            return 1;      
        }
    }
    

    if(ovoidToBox[2] != 0){
        if(ovoidToBox[2] > 0){
            d = (aabb.z - ovoid.origin[2]) /ovoidToBox[2];
            z = aabb.z;

        }
        if(ovoidToBox[2] < 0){
            d = (aabb.z + aabb.depth - ovoid.origin[2]) /ovoidToBox[2];
            z = aabb.z + aabb.depth;
        }
        y = clampf(
            aabb.y, 
            ovoid.origin[1] + d * ovoidToBox[1],
            aabb.y + aabb.height
        );
        z = clampf(aabb.z, 
            ovoid.origin[2] + d * ovoidToBox[2],
            aabb.z + aabb.depth
        );
        if(ovoid_contains_point(ovoid, x, y, z)){
            return 1;      
        }
    }
    return 0;    
}