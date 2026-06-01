#ifndef HIT_RESULT_H
#define HIT_RESULT_H

typedef struct {
    int x;
    int y;
    int z;
    int o;
    int f;
} HitResult;

inline HitResult HitResult_create(int x, int y, int z, int o, int f) {
    HitResult h;
    h.x = x;
    h.y = y;
    h.z = z;
    h.o = o;
    h.f = f;
    return h;
}

#endif /* HIT_RESULT_H */
