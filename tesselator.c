#include "tesselator.h"
#include "common.h"

Tesselator* Tesselator_create(void) {
    Tesselator* t = (Tesselator*)malloc(sizeof(Tesselator));
    if (t) {
        t->vertexBuffer = (float*)malloc(300000 * sizeof(float));
        t->texCoordBuffer = (float*)malloc(200000 * sizeof(float));
        t->colorBuffer = (float*)malloc(300000 * sizeof(float));
        t->vertices = 0;
        t->u = 0.0f;
        t->v = 0.0f;
        t->r = 1.0f;
        t->g = 1.0f;
        t->b = 1.0f;
        t->hasColor = false;
        t->hasTexture = false;
    }
    return t;
}

void Tesselator_destroy(Tesselator* self) {
    if (self) {
        free(self->vertexBuffer);
        free(self->texCoordBuffer);
        free(self->colorBuffer);
        free(self);
    }
}

void Tesselator_clear(Tesselator* self) {
    if (self) {
        self->vertices = 0;
    }
}

void Tesselator_flush(Tesselator* self) {
    if (!self || self->vertices == 0) return;

#ifdef ANDROID_PORT
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, self->vertexBuffer);

    if (self->hasTexture) {
        glEnableClientState(GL_TEXTURE_COORD_ARRAY);
        glTexCoordPointer(2, GL_FLOAT, 0, self->texCoordBuffer);
    } else {
        glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    }

    if (self->hasColor) {
        glEnableClientState(GL_COLOR_ARRAY);
        // OpenGL ES 1.1 glColorPointer usually expects 4 components (RGBA) for best performance, 
        // but let's see if 3 works or if we need to expand the buffer.
        glColorPointer(3, GL_FLOAT, 0, self->colorBuffer);
    } else {
        glDisableClientState(GL_COLOR_ARRAY);
    }

    // GL_QUADS is not supported in GLES. We must convert to GL_TRIANGLES.
    // The current buffer is stored as quads (4 vertices each).
    // We can use glDrawElements with an index buffer or just draw as triangles if we re-arranged.
    // Simplest fix: Draw as triangles by using an index buffer.
    int indexCount = (self->vertices / 4) * 6;
    unsigned short* indices = (unsigned short*)malloc(indexCount * sizeof(unsigned short));
    for (int i = 0, v = 0; i < indexCount; i += 6, v += 4) {
        indices[i + 0] = v + 0;
        indices[i + 1] = v + 1;
        indices[i + 2] = v + 2;
        indices[i + 3] = v + 0;
        indices[i + 4] = v + 2;
        indices[i + 5] = v + 3;
    }
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_SHORT, indices);
    free(indices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);
#else
    glBegin(GL_QUADS);
    for (int i = 0; i < self->vertices; i++) {
        if (self->hasColor) {
            glColor3f(self->colorBuffer[i * 3 + 0], self->colorBuffer[i * 3 + 1], self->colorBuffer[i * 3 + 2]);
        }
        if (self->hasTexture) {
            glTexCoord2f(self->texCoordBuffer[i * 2 + 0], self->texCoordBuffer[i * 2 + 1]);
        }
        glVertex3f(self->vertexBuffer[i * 3 + 0], self->vertexBuffer[i * 3 + 1], self->vertexBuffer[i * 3 + 2]);
    }
    glEnd();
#endif

    Tesselator_clear(self);
}

void Tesselator_init(Tesselator* self) {
    if (!self) return;
    Tesselator_clear(self);
    self->hasColor = false;
    self->hasTexture = false;
}

void Tesselator_tex(Tesselator* self, float u, float v) {
    if (!self) return;
    self->hasTexture = true;
    self->u = u;
    self->v = v;
}

void Tesselator_color(Tesselator* self, float r, float g, float b) {
    if (!self) return;
    self->hasColor = true;
    self->r = r;
    self->g = g;
    self->b = b;
}

void Tesselator_vertex(Tesselator* self, float x, float y, float z) {
    if (!self) return;

    self->vertexBuffer[self->vertices * 3 + 0] = x;
    self->vertexBuffer[self->vertices * 3 + 1] = y;
    self->vertexBuffer[self->vertices * 3 + 2] = z;

    if (self->hasTexture) {
        self->texCoordBuffer[self->vertices * 2 + 0] = self->u;
        self->texCoordBuffer[self->vertices * 2 + 1] = self->v;
    }

    if (self->hasColor) {
        self->colorBuffer[self->vertices * 3 + 0] = self->r;
        self->colorBuffer[self->vertices * 3 + 1] = self->g;
        self->colorBuffer[self->vertices * 3 + 2] = self->b;
    }

    self->vertices++;
    if (self->vertices == MAX_VERTICES) {
        Tesselator_flush(self);
    }
}
