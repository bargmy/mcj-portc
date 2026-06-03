#include "aabb.h"

AABB AABB_create(float x0, float y0, float z0, float x1, float y1, float z1) {
    AABB box;
    box.epsilon = 0.0f;
    box.x0 = x0;
    box.y0 = y0;
    box.z0 = z0;
    box.x1 = x1;
    box.y1 = y1;
    box.z1 = z1;
    return box;
}

AABB AABB_expand(AABB self, float xa, float ya, float za) {
    float _x0 = self.x0;
    float _y0 = self.y0;
    float _z0 = self.z0;
    float _x1 = self.x1;
    float _y1 = self.y1;
    float _z1 = self.z1;
    if (xa < 0.0f) {
        _x0 += xa;
    }
    if (xa > 0.0f) {
        _x1 += xa;
    }
    if (ya < 0.0f) {
        _y0 += ya;
    }
    if (ya > 0.0f) {
        _y1 += ya;
    }
    if (za < 0.0f) {
        _z0 += za;
    }
    if (za > 0.0f) {
        _z1 += za;
    }
    return AABB_create(_x0, _y0, _z0, _x1, _y1, _z1);
}

AABB AABB_grow(AABB self, float xa, float ya, float za) {
    float _x0 = self.x0 - xa;
    float _y0 = self.y0 - ya;
    float _z0 = self.z0 - za;
    float _x1 = self.x1 + xa;
    float _y1 = self.y1 + ya;
    float _z1 = self.z1 + za;
    return AABB_create(_x0, _y0, _z0, _x1, _y1, _z1);
}

float AABB_clipXCollide(AABB self, AABB c, float xa) {
    if (c.y1 <= self.y0 || c.y0 >= self.y1) {
        return xa;
    } else if (!(c.z1 <= self.z0) && !(c.z0 >= self.z1)) {
        if (xa > 0.0f && c.x1 <= self.x0) {
            float max = self.x0 - c.x1 - self.epsilon;
            if (max < xa) {
                xa = max;
            }
        }
        if (xa < 0.0f && c.x0 >= self.x1) {
            float max = self.x1 - c.x0 + self.epsilon;
            if (max > xa) {
                xa = max;
            }
        }
        return xa;
    } else {
        return xa;
    }
}

float AABB_clipYCollide(AABB self, AABB c, float ya) {
    if (c.x1 <= self.x0 || c.x0 >= self.x1) {
        return ya;
    } else if (!(c.z1 <= self.z0) && !(c.z0 >= self.z1)) {
        if (ya > 0.0f && c.y1 <= self.y0) {
            float max = self.y0 - c.y1 - self.epsilon;
            if (max < ya) {
                ya = max;
            }
        }
        if (ya < 0.0f && c.y0 >= self.y1) {
            float max = self.y1 - c.y0 + self.epsilon;
            if (max > ya) {
                ya = max;
            }
        }
        return ya;
    } else {
        return ya;
    }
}

float AABB_clipZCollide(AABB self, AABB c, float za) {
    if (c.x1 <= self.x0 || c.x0 >= self.x1) {
        return za;
    } else if (!(c.y1 <= self.y0) && !(c.y0 >= self.y1)) {
        if (za > 0.0f && c.z1 <= self.z0) {
            float max = self.z0 - c.z1 - self.epsilon;
            if (max < za) {
                za = max;
            }
        }
        if (za < 0.0f && c.z0 >= self.z1) {
            float max = self.z1 - c.z0 + self.epsilon;
            if (max > za) {
                za = max;
            }
        }
        return za;
    } else {
        return za;
    }
}

bool AABB_intersects(AABB self, AABB c) {
    if (c.x1 <= self.x0 || c.x0 >= self.x1) {
        return false;
    } else {
        return (c.y1 <= self.y0 || c.y0 >= self.y1) ? false : (!(c.z1 <= self.z0) && !(c.z0 >= self.z1));
    }
}

void AABB_move(AABB* self, float xa, float ya, float za) {
    if (self) {
        self->x0 += xa;
        self->y0 += ya;
        self->z0 += za;
        self->x1 += xa;
        self->y1 += ya;
        self->z1 += za;
    }
}
