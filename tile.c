#include "tile.h"
#include "level.h"

Tile Tile_rock = { 0 };
Tile Tile_grass = { 1 };

void Tile_init(void) {
    Tile_rock.tex = 0;
    Tile_grass.tex = 1;
}

void Tile_render(Tile* self, Tesselator* t, Level* level, int layer, int x, int y, int z) {
    if (!self || !t || !level) return;

    float u0 = (float)self->tex / 16.0f;
    float u1 = u0 + 0.0624375f;
    float v0 = 0.0f;
    float v1 = v0 + 0.0624375f;

    float c1 = 1.0f;
    float c2 = 0.8f;
    float c3 = 0.6f;

    float x0 = (float)x + 0.0f;
    float x1 = (float)x + 1.0f;
    float y0 = (float)y + 0.0f;
    float y1 = (float)y + 1.0f;
    float z0 = (float)z + 0.0f;
    float z1 = (float)z + 1.0f;

    // Bottom face
    if (!Level_isSolidTile(level, x, y - 1, z)) {
        float br = Level_getBrightness(level, x, y - 1, z) * c1;
        if ((br == c1) ^ (layer == 1)) {
            Tesselator_color(t, br, br, br);
            Tesselator_tex(t, u0, v1);
            Tesselator_vertex(t, x0, y0, z1);
            Tesselator_tex(t, u0, v0);
            Tesselator_vertex(t, x0, y0, z0);
            Tesselator_tex(t, u1, v0);
            Tesselator_vertex(t, x1, y0, z0);
            Tesselator_tex(t, u1, v1);
            Tesselator_vertex(t, x1, y0, z1);
        }
    }

    // Top face
    if (!Level_isSolidTile(level, x, y + 1, z)) {
        float br = Level_getBrightness(level, x, y, z) * c1;
        if ((br == c1) ^ (layer == 1)) {
            Tesselator_color(t, br, br, br);
            Tesselator_tex(t, u1, v1);
            Tesselator_vertex(t, x1, y1, z1);
            Tesselator_tex(t, u1, v0);
            Tesselator_vertex(t, x1, y1, z0);
            Tesselator_tex(t, u0, v0);
            Tesselator_vertex(t, x0, y1, z0);
            Tesselator_tex(t, u0, v1);
            Tesselator_vertex(t, x0, y1, z1);
        }
    }

    // Back face (z-1)
    if (!Level_isSolidTile(level, x, y, z - 1)) {
        float br = Level_getBrightness(level, x, y, z - 1) * c2;
        if ((br == c2) ^ (layer == 1)) {
            Tesselator_color(t, br, br, br);
            Tesselator_tex(t, u1, v0);
            Tesselator_vertex(t, x0, y1, z0);
            Tesselator_tex(t, u0, v0);
            Tesselator_vertex(t, x1, y1, z0);
            Tesselator_tex(t, u0, v1);
            Tesselator_vertex(t, x1, y0, z0);
            Tesselator_tex(t, u1, v1);
            Tesselator_vertex(t, x0, y0, z0);
        }
    }

    // Front face (z+1)
    if (!Level_isSolidTile(level, x, y, z + 1)) {
        float br = Level_getBrightness(level, x, y, z + 1) * c2;
        if ((br == c2) ^ (layer == 1)) {
            Tesselator_color(t, br, br, br);
            Tesselator_tex(t, u0, v0);
            Tesselator_vertex(t, x0, y1, z1);
            Tesselator_tex(t, u0, v1);
            Tesselator_vertex(t, x0, y0, z1);
            Tesselator_tex(t, u1, v1);
            Tesselator_vertex(t, x1, y0, z1);
            Tesselator_tex(t, u1, v0);
            Tesselator_vertex(t, x1, y1, z1);
        }
    }

    // Left face (x-1)
    if (!Level_isSolidTile(level, x - 1, y, z)) {
        float br = Level_getBrightness(level, x - 1, y, z) * c3;
        if ((br == c3) ^ (layer == 1)) {
            Tesselator_color(t, br, br, br);
            Tesselator_tex(t, u1, v0);
            Tesselator_vertex(t, x0, y1, z1);
            Tesselator_tex(t, u0, v0);
            Tesselator_vertex(t, x0, y1, z0);
            Tesselator_tex(t, u0, v1);
            Tesselator_vertex(t, x0, y0, z0);
            Tesselator_tex(t, u1, v1);
            Tesselator_vertex(t, x0, y0, z1);
        }
    }

    // Right face (x+1)
    if (!Level_isSolidTile(level, x + 1, y, z)) {
        float br = Level_getBrightness(level, x + 1, y, z) * c3;
        if ((br == c3) ^ (layer == 1)) {
            Tesselator_color(t, br, br, br);
            Tesselator_tex(t, u0, v1);
            Tesselator_vertex(t, x1, y0, z1);
            Tesselator_tex(t, u1, v1);
            Tesselator_vertex(t, x1, y0, z0);
            Tesselator_tex(t, u1, v0);
            Tesselator_vertex(t, x1, y1, z0);
            Tesselator_tex(t, u0, v0);
            Tesselator_vertex(t, x1, y1, z1);
        }
    }
}

void Tile_renderFace(Tile* self, Tesselator* t, int x, int y, int z, int face) {
    if (!self || !t) return;

    float x0 = (float)x + 0.0f;
    float x1 = (float)x + 1.0f;
    float y0 = (float)y + 0.0f;
    float y1 = (float)y + 1.0f;
    float z0 = (float)z + 0.0f;
    float z1 = (float)z + 1.0f;

    if (face == 0) {
        Tesselator_vertex(t, x0, y0, z1);
        Tesselator_vertex(t, x0, y0, z0);
        Tesselator_vertex(t, x1, y0, z0);
        Tesselator_vertex(t, x1, y0, z1);
    }
    if (face == 1) {
        Tesselator_vertex(t, x1, y1, z1);
        Tesselator_vertex(t, x1, y1, z0);
        Tesselator_vertex(t, x0, y1, z0);
        Tesselator_vertex(t, x0, y1, z1);
    }
    if (face == 2) {
        Tesselator_vertex(t, x0, y1, z0);
        Tesselator_vertex(t, x1, y1, z0);
        Tesselator_vertex(t, x1, y0, z0);
        Tesselator_vertex(t, x0, y0, z0);
    }
    if (face == 3) {
        Tesselator_vertex(t, x0, y1, z1);
        Tesselator_vertex(t, x0, y0, z1);
        Tesselator_vertex(t, x1, y0, z1);
        Tesselator_vertex(t, x1, y1, z1);
    }
    if (face == 4) {
        Tesselator_vertex(t, x0, y1, z1);
        Tesselator_vertex(t, x0, y1, z0);
        Tesselator_vertex(t, x0, y0, z0);
        Tesselator_vertex(t, x0, y0, z1);
    }
    if (face == 5) {
        Tesselator_vertex(t, x1, y0, z1);
        Tesselator_vertex(t, x1, y0, z0);
        Tesselator_vertex(t, x1, y1, z0);
        Tesselator_vertex(t, x1, y1, z1);
    }
}
