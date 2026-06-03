#include "vec3.h"

Vec3 Vec3_create(float x, float y, float z) {
    Vec3 v;
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

Vec3 Vec3_interpolateTo(Vec3 self, Vec3 t, float p) {
    float xt = self.x + (t.x - self.x) * p;
    float yt = self.y + (t.y - self.y) * p;
    float zt = self.z + (t.z - self.z) * p;
    return Vec3_create(xt, yt, zt);
}

void Vec3_set(Vec3* self, float x, float y, float z) {
    if (self) {
        self->x = x;
        self->y = y;
        self->z = z;
    }
}
