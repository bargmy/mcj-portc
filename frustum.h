#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "aabb.h"
#include <stdbool.h>

typedef struct {
    float m_Frustum[6][4];
    float proj[16];
    float modl[16];
    float clip[16];
} Frustum;

Frustum* Frustum_getFrustum(void);
bool Frustum_pointInFrustum(Frustum* self, float x, float y, float z);
bool Frustum_sphereInFrustum(Frustum* self, float x, float y, float z, float radius);
bool Frustum_cubeFullyInFrustum(Frustum* self, float x1, float y1, float z1, float x2, float y2, float z2);
bool Frustum_cubeInFrustum(Frustum* self, float x1, float y1, float z1, float x2, float y2, float z2);
bool Frustum_cubeInFrustumAABB(Frustum* self, AABB aabb);

#endif /* FRUSTUM_H */
