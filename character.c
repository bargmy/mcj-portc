#include "character.h"
#include "textures.h"
#include "common.h"

// Vertex
Vertex Vertex_create(float x, float y, float z, float u, float v) {
    Vertex vt;
    vt.pos = Vec3_create(x, y, z);
    vt.u = u;
    vt.v = v;
    return vt;
}

Vertex Vertex_remap(Vertex self, float u, float v) {
    Vertex vt;
    vt.pos = self.pos;
    vt.u = u;
    vt.v = v;
    return vt;
}

// ModelPolygon
ModelPolygon ModelPolygon_create(Vertex* vertices) {
    ModelPolygon poly;
    poly.vertexCount = 4;
    for (int i = 0; i < 4; i++) {
        poly.vertices[i] = vertices[i];
    }
    return poly;
}

ModelPolygon ModelPolygon_createTex(Vertex* vertices, int u0, int v0, int u1, int v1) {
    ModelPolygon poly = ModelPolygon_create(vertices);
    poly.vertices[0] = Vertex_remap(poly.vertices[0], (float)u1, (float)v0);
    poly.vertices[1] = Vertex_remap(poly.vertices[1], (float)u0, (float)v0);
    poly.vertices[2] = Vertex_remap(poly.vertices[2], (float)u0, (float)v1);
    poly.vertices[3] = Vertex_remap(poly.vertices[3], (float)u1, (float)v1);
    return poly;
}

void ModelPolygon_render(ModelPolygon* self) {
    if (!self) return;
    glColor3f(1.0f, 1.0f, 1.0f);
    for (int i = 3; i >= 0; i--) {
        Vertex v = self->vertices[i];
        glTexCoord2f(v.u / 64.0f, v.v / 32.0f);
        glVertex3f(v.pos.x, v.pos.y, v.pos.z);
    }
}

// Cube
Cube Cube_create(int xTexOffs, int yTexOffs) {
    Cube c;
    c.xTexOffs = xTexOffs;
    c.yTexOffs = yTexOffs;
    c.x = 0.0f;
    c.y = 0.0f;
    c.z = 0.0f;
    c.xRot = 0.0f;
    c.yRot = 0.0f;
    c.zRot = 0.0f;
    memset(c.vertices, 0, sizeof(c.vertices));
    memset(c.polygons, 0, sizeof(c.polygons));
    return c;
}

void Cube_setTexOffs(Cube* self, int xTexOffs, int yTexOffs) {
    if (!self) return;
    self->xTexOffs = xTexOffs;
    self->yTexOffs = yTexOffs;
}

void Cube_addBox(Cube* self, float x0, float y0, float z0, int w, int h, int d) {
    if (!self) return;
    float x1 = x0 + (float)w;
    float y1 = y0 + (float)h;
    float z1 = z0 + (float)d;

    Vertex u0 = Vertex_create(x0, y0, z0, 0.0f, 0.0f);
    Vertex u1 = Vertex_create(x1, y0, z0, 0.0f, 8.0f);
    Vertex u2 = Vertex_create(x1, y1, z0, 8.0f, 8.0f);
    Vertex u3 = Vertex_create(x0, y1, z0, 8.0f, 0.0f);
    Vertex l0 = Vertex_create(x0, y0, z1, 0.0f, 0.0f);
    Vertex l1 = Vertex_create(x1, y0, z1, 0.0f, 8.0f);
    Vertex l2 = Vertex_create(x1, y1, z1, 8.0f, 8.0f);
    Vertex l3 = Vertex_create(x0, y1, z1, 8.0f, 0.0f);

    self->vertices[0] = u0;
    self->vertices[1] = u1;
    self->vertices[2] = u2;
    self->vertices[3] = u3;
    self->vertices[4] = l0;
    self->vertices[5] = l1;
    self->vertices[6] = l2;
    self->vertices[7] = l3;

    Vertex face0[4] = { l1, u1, u2, l2 };
    self->polygons[0] = ModelPolygon_createTex(face0, self->xTexOffs + d + w, self->yTexOffs + d, self->xTexOffs + d + w + d, self->yTexOffs + d + h);

    Vertex face1[4] = { u0, l0, l3, u3 };
    self->polygons[1] = ModelPolygon_createTex(face1, self->xTexOffs + 0, self->yTexOffs + d, self->xTexOffs + d, self->yTexOffs + d + h);

    Vertex face2[4] = { l1, l0, u0, u1 };
    self->polygons[2] = ModelPolygon_createTex(face2, self->xTexOffs + d, self->yTexOffs + 0, self->xTexOffs + d + w, self->yTexOffs + d);

    Vertex face3[4] = { u2, u3, l3, l2 };
    self->polygons[3] = ModelPolygon_createTex(face3, self->xTexOffs + d + w, self->yTexOffs + 0, self->xTexOffs + d + w + w, self->yTexOffs + d);

    Vertex face4[4] = { u1, u0, u3, u2 };
    self->polygons[4] = ModelPolygon_createTex(face4, self->xTexOffs + d, self->yTexOffs + d, self->xTexOffs + d + w, self->yTexOffs + d + h);

    Vertex face5[4] = { l0, l1, l2, l3 };
    self->polygons[5] = ModelPolygon_createTex(face5, self->xTexOffs + d + w + d, self->yTexOffs + d, self->xTexOffs + d + w + d + w, self->yTexOffs + d + h);
}

void Cube_setPos(Cube* self, float x, float y, float z) {
    if (!self) return;
    self->x = x;
    self->y = y;
    self->z = z;
}

void Cube_render(Cube* self) {
    if (!self) return;
    float c = (float)(180.0 / M_PI);
    glPushMatrix();
    glTranslatef(self->x, self->y, self->z);
    glRotatef(self->zRot * c, 0.0f, 0.0f, 1.0f);
    glRotatef(self->yRot * c, 0.0f, 1.0f, 0.0f);
    glRotatef(self->xRot * c, 1.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
    for (int i = 0; i < 6; i++) {
        ModelPolygon_render(&self->polygons[i]);
    }
    glEnd();
    glPopMatrix();
}

// Zombie
Zombie* Zombie_create(Level* level, float x, float y, float z) {
    Zombie* zb = (Zombie*)malloc(sizeof(Zombie));
    if (zb) {
        Entity_init(&zb->base, level);
        zb->base.x = x;
        zb->base.y = y;
        zb->base.z = z;
        zb->timeOffs = ((float)rand() / (float)RAND_MAX) * 1239813.0f;
        zb->rot = ((float)rand() / (float)RAND_MAX) * (float)M_PI * 2.0f;
        zb->speed = 1.0f;
        zb->rotA = (((float)rand() / (float)RAND_MAX) + 1.0f) * 0.01f;

        zb->head = Cube_create(0, 0);
        Cube_addBox(&zb->head, -4.0f, -8.0f, -4.0f, 8, 8, 8);

        zb->body = Cube_create(16, 16);
        Cube_addBox(&zb->body, -4.0f, 0.0f, -2.0f, 8, 12, 4);

        zb->arm0 = Cube_create(40, 16);
        Cube_addBox(&zb->arm0, -3.0f, -2.0f, -2.0f, 4, 12, 4);
        Cube_setPos(&zb->arm0, -5.0f, 2.0f, 0.0f);

        zb->arm1 = Cube_create(40, 16);
        Cube_addBox(&zb->arm1, -1.0f, -2.0f, -2.0f, 4, 12, 4);
        Cube_setPos(&zb->arm1, 5.0f, 2.0f, 0.0f);

        zb->leg0 = Cube_create(0, 16);
        Cube_addBox(&zb->leg0, -2.0f, 0.0f, -2.0f, 4, 12, 4);
        Cube_setPos(&zb->leg0, -2.0f, 12.0f, 0.0f);

        zb->leg1 = Cube_create(0, 16);
        Cube_addBox(&zb->leg1, -2.0f, 0.0f, -2.0f, 4, 12, 4);
        Cube_setPos(&zb->leg1, 2.0f, 12.0f, 0.0f);
    }
    return zb;
}

void Zombie_destroy(Zombie* self) {
    if (self) {
        free(self);
    }
}

void Zombie_tick(Zombie* self) {
    if (!self) return;
    self->base.xo = self->base.x;
    self->base.yo = self->base.y;
    self->base.zo = self->base.z;

    float xa = 0.0f;
    float ya = 0.0f;

    self->rot += self->rotA;
    self->rotA = (float)((double)self->rotA * 0.99);
    self->rotA += (float)((((double)rand() / RAND_MAX) - ((double)rand() / RAND_MAX)) * ((double)rand() / RAND_MAX) * ((double)rand() / RAND_MAX) * 0.01);

    xa = sinf(self->rot);
    ya = cosf(self->rot);

    if (self->base.onGround && ((float)rand() / (float)RAND_MAX) < 0.01f) {
        self->base.yd = 0.12f;
    }

    Entity_moveRelative(&self->base, xa, ya, self->base.onGround ? 0.02f : 0.005f);
    self->base.yd = (float)((double)self->base.yd - 0.005);
    Entity_move(&self->base, self->base.xd, self->base.yd, self->base.zd);

    self->base.xd *= 0.91f;
    self->base.yd *= 0.98f;
    self->base.zd *= 0.91f;

    if (self->base.y > 100.0f) {
        Entity_resetPos(&self->base);
    }

    if (self->base.onGround) {
        self->base.xd *= 0.8f;
        self->base.zd *= 0.8f;
    }
}

void Zombie_render(Zombie* self, float a) {
    if (!self) return;

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, Textures_loadTexture("/char.png", GL_NEAREST));
    glPushMatrix();

    double time = glfwGetTime() * 10.0 * (double)self->speed + (double)self->timeOffs;
    float size = 0.058333334f;
    float yy = (float)(-fabs(sin(time * 0.6662)) * 5.0 - 23.0);

    glTranslatef(
        self->base.xo + (self->base.x - self->base.xo) * a,
        self->base.yo + (self->base.y - self->base.yo) * a,
        self->base.zo + (self->base.z - self->base.zo) * a
    );
    glScalef(1.0f, -1.0f, 1.0f);
    glScalef(size, size, size);
    glTranslatef(0.0f, yy, 0.0f);

    float c = (float)(180.0 / M_PI);
    glRotatef(self->rot * c + 180.0f, 0.0f, 1.0f, 0.0f);

    self->head.yRot = (float)sin(time * 0.83) * 1.0f;
    self->head.xRot = (float)sin(time) * 0.8f;
    self->arm0.xRot = (float)sin(time * 0.6662 + M_PI) * 2.0f;
    self->arm0.zRot = (float)(sin(time * 0.2312) + 1.0) * 1.0f;
    self->arm1.xRot = (float)sin(time * 0.6662) * 2.0f;
    self->arm1.zRot = (float)(sin(time * 0.2812) - 1.0) * 1.0f;
    self->leg0.xRot = (float)sin(time * 0.6662) * 1.4f;
    self->leg1.xRot = (float)sin(time * 0.6662 + M_PI) * 1.4f;

    Cube_render(&self->head);
    Cube_render(&self->body);
    Cube_render(&self->arm0);
    Cube_render(&self->arm1);
    Cube_render(&self->leg0);
    Cube_render(&self->leg1);

    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}
