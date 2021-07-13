
typedef struct Ray{
    float x,y,z,dx,dy,dz;
}Ray;

void init_ray(Ray* ray, float x, float y, float z, float dx, float dy, float dz);