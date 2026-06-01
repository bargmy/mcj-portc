#include "frustum.h"
#include "common.h"

static Frustum globalFrustum;

static void Frustum_normalizePlane(Frustum* self, int side) {
    float magnitude = sqrtf(
        self->m_Frustum[side][0] * self->m_Frustum[side][0] +
        self->m_Frustum[side][1] * self->m_Frustum[side][1] +
        self->m_Frustum[side][2] * self->m_Frustum[side][2]
    );
    self->m_Frustum[side][0] /= magnitude;
    self->m_Frustum[side][1] /= magnitude;
    self->m_Frustum[side][2] /= magnitude;
    self->m_Frustum[side][3] /= magnitude;
}

static void Frustum_calculateFrustum(Frustum* self) {
    glGetFloatv(GL_PROJECTION_MATRIX, self->proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, self->modl);

    self->clip[0] = self->modl[0] * self->proj[0] + self->modl[1] * self->proj[4] + self->modl[2] * self->proj[8] + self->modl[3] * self->proj[12];
    self->clip[1] = self->modl[0] * self->proj[1] + self->modl[1] * self->proj[5] + self->modl[2] * self->proj[9] + self->modl[3] * self->proj[13];
    self->clip[2] = self->modl[0] * self->proj[2] + self->modl[1] * self->proj[6] + self->modl[2] * self->proj[10] + self->modl[3] * self->proj[14];
    self->clip[3] = self->modl[0] * self->proj[3] + self->modl[1] * self->proj[7] + self->modl[2] * self->proj[11] + self->modl[3] * self->proj[15];
    self->clip[4] = self->modl[4] * self->proj[0] + self->modl[5] * self->proj[4] + self->modl[6] * self->proj[8] + self->modl[7] * self->proj[12];
    self->clip[5] = self->modl[4] * self->proj[1] + self->modl[5] * self->proj[5] + self->modl[6] * self->proj[9] + self->modl[7] * self->proj[13];
    self->clip[6] = self->modl[4] * self->proj[2] + self->modl[5] * self->proj[6] + self->modl[6] * self->proj[10] + self->modl[7] * self->proj[14];
    self->clip[7] = self->modl[4] * self->proj[3] + self->modl[5] * self->proj[7] + self->modl[6] * self->proj[11] + self->modl[7] * self->proj[15];
    self->clip[8] = self->modl[8] * self->proj[0] + self->modl[9] * self->proj[4] + self->modl[10] * self->proj[8] + self->modl[11] * self->proj[12];
    self->clip[9] = self->modl[8] * self->proj[1] + self->modl[9] * self->proj[5] + self->modl[10] * self->proj[9] + self->modl[11] * self->proj[13];
    self->clip[10] = self->modl[8] * self->proj[2] + self->modl[9] * self->proj[6] + self->modl[10] * self->proj[10] + self->modl[11] * self->proj[14];
    self->clip[11] = self->modl[8] * self->proj[3] + self->modl[9] * self->proj[7] + self->modl[10] * self->proj[11] + self->modl[11] * self->proj[15];
    self->clip[12] = self->modl[12] * self->proj[0] + self->modl[13] * self->proj[4] + self->modl[14] * self->proj[8] + self->modl[15] * self->proj[12];
    self->clip[13] = self->modl[12] * self->proj[1] + self->modl[13] * self->proj[5] + self->modl[14] * self->proj[9] + self->modl[15] * self->proj[13];
    self->clip[14] = self->modl[12] * self->proj[2] + self->modl[13] * self->proj[6] + self->modl[14] * self->proj[10] + self->modl[15] * self->proj[14];
    self->clip[15] = self->modl[12] * self->proj[3] + self->modl[13] * self->proj[7] + self->modl[14] * self->proj[11] + self->modl[15] * self->proj[15];

    self->m_Frustum[0][0] = self->clip[3] - self->clip[0];
    self->m_Frustum[0][1] = self->clip[7] - self->clip[4];
    self->m_Frustum[0][2] = self->clip[11] - self->clip[8];
    self->m_Frustum[0][3] = self->clip[15] - self->clip[12];
    Frustum_normalizePlane(self, 0);

    self->m_Frustum[1][0] = self->clip[3] + self->clip[0];
    self->m_Frustum[1][1] = self->clip[7] + self->clip[4];
    self->m_Frustum[1][2] = self->clip[11] + self->clip[8];
    self->m_Frustum[1][3] = self->clip[15] + self->clip[12];
    Frustum_normalizePlane(self, 1);

    self->m_Frustum[2][0] = self->clip[3] + self->clip[1];
    self->m_Frustum[2][1] = self->clip[7] + self->clip[5];
    self->m_Frustum[2][2] = self->clip[11] + self->clip[9];
    self->m_Frustum[2][3] = self->clip[15] + self->clip[13];
    Frustum_normalizePlane(self, 2);

    self->m_Frustum[3][0] = self->clip[3] - self->clip[1];
    self->m_Frustum[3][1] = self->clip[7] - self->clip[5];
    self->m_Frustum[3][2] = self->clip[11] - self->clip[9];
    self->m_Frustum[3][3] = self->clip[15] - self->clip[13];
    Frustum_normalizePlane(self, 3);

    self->m_Frustum[4][0] = self->clip[3] - self->clip[2];
    self->m_Frustum[4][1] = self->clip[7] - self->clip[6];
    self->m_Frustum[4][2] = self->clip[11] - self->clip[10];
    self->m_Frustum[4][3] = self->clip[15] - self->clip[14];
    Frustum_normalizePlane(self, 4);

    self->m_Frustum[5][0] = self->clip[3] + self->clip[2];
    self->m_Frustum[5][1] = self->clip[7] + self->clip[6];
    self->m_Frustum[5][2] = self->clip[11] + self->clip[10];
    self->m_Frustum[5][3] = self->clip[15] + self->clip[14];
    Frustum_normalizePlane(self, 5);
}

Frustum* Frustum_getFrustum(void) {
    Frustum_calculateFrustum(&globalFrustum);
    return &globalFrustum;
}

bool Frustum_pointInFrustum(Frustum* self, float x, float y, float z) {
    if (!self) return false;
    for (int i = 0; i < 6; i++) {
        if (self->m_Frustum[i][0] * x + self->m_Frustum[i][1] * y + self->m_Frustum[i][2] * z + self->m_Frustum[i][3] <= 0.0f) {
            return false;
        }
    }
    return true;
}

bool Frustum_sphereInFrustum(Frustum* self, float x, float y, float z, float radius) {
    if (!self) return false;
    for (int i = 0; i < 6; i++) {
        if (self->m_Frustum[i][0] * x + self->m_Frustum[i][1] * y + self->m_Frustum[i][2] * z + self->m_Frustum[i][3] <= -radius) {
            return false;
        }
    }
    return true;
}

bool Frustum_cubeFullyInFrustum(Frustum* self, float x1, float y1, float z1, float x2, float y2, float z2) {
    if (!self) return false;
    for (int i = 0; i < 6; i++) {
        if (!(self->m_Frustum[i][0] * x1 + self->m_Frustum[i][1] * y1 + self->m_Frustum[i][2] * z1 + self->m_Frustum[i][3] > 0.0f)) return false;
        if (!(self->m_Frustum[i][0] * x2 + self->m_Frustum[i][1] * y1 + self->m_Frustum[i][2] * z1 + self->m_Frustum[i][3] > 0.0f)) return false;
        if (!(self->m_Frustum[i][0] * x1 + self->m_Frustum[i][1] * y2 + self->m_Frustum[i][2] * z1 + self->m_Frustum[i][3] > 0.0f)) return false;
        if (!(self->m_Frustum[i][0] * x2 + self->m_Frustum[i][1] * y2 + self->m_Frustum[i][2] * z1 + self->m_Frustum[i][3] > 0.0f)) return false;
        if (!(self->m_Frustum[i][0] * x1 + self->m_Frustum[i][1] * y1 + self->m_Frustum[i][2] * z2 + self->m_Frustum[i][3] > 0.0f)) return false;
        if (!(self->m_Frustum[i][0] * x2 + self->m_Frustum[i][1] * y1 + self->m_Frustum[i][2] * z2 + self->m_Frustum[i][3] > 0.0f)) return false;
        if (!(self->m_Frustum[i][0] * x1 + self->m_Frustum[i][1] * y2 + self->m_Frustum[i][2] * z2 + self->m_Frustum[i][3] > 0.0f)) return false;
        if (!(self->m_Frustum[i][0] * x2 + self->m_Frustum[i][1] * y2 + self->m_Frustum[i][2] * z2 + self->m_Frustum[i][3] > 0.0f)) return false;
    }
    return true;
}

bool Frustum_cubeInFrustum(Frustum* self, float x1, float y1, float z1, float x2, float y2, float z2) {
    if (!self) return false;
    float mx = (x1 + x2) * 0.5f;
    float my = (y1 + y2) * 0.5f;
    float mz = (z1 + z2) * 0.5f;
    float ex = fabsf(x2 - x1) * 0.5f;
    float ey = fabsf(y2 - y1) * 0.5f;
    float ez = fabsf(z2 - z1) * 0.5f;

    for (int i = 0; i < 6; i++) {
        float dist = self->m_Frustum[i][0] * mx + 
                     self->m_Frustum[i][1] * my + 
                     self->m_Frustum[i][2] * mz + 
                     self->m_Frustum[i][3];
        float r = ex * fabsf(self->m_Frustum[i][0]) + 
                  ey * fabsf(self->m_Frustum[i][1]) + 
                  ez * fabsf(self->m_Frustum[i][2]);
        if (dist < -r) {
            return false;
        }
    }
    return true;
}

bool Frustum_cubeInFrustumAABB(Frustum* self, AABB aabb) {
    return Frustum_cubeInFrustum(self, aabb.x0, aabb.y0, aabb.z0, aabb.x1, aabb.y1, aabb.z1);
}
