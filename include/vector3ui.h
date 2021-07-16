
#ifndef VECTOR3UI_H
#define VECTOR3UI_H
typedef unsigned int vec3ui[3];

typedef union{
    struct 
    {
        unsigned int x,y,z;
    };
    unsigned int vec[3];
}Vector3ui;
#endif