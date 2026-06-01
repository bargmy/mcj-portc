#ifndef VEC3_H
#define VEC3_H

typedef struct {
    float x;
    float y;
    float z;
} Vec3;

Vec3 Vec3_create(float x, float y, float z);
Vec3 Vec3_interpolateTo(Vec3 self, Vec3 t, float p);
void Vec3_set(Vec3* self, float x, float y, float z);

#endif /* VEC3_H */
