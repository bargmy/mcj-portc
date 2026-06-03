#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#include <windows.h>
#endif

#ifdef ANDROID_PORT
#include <GLES/gl.h>
#include <GLES/glext.h>

// Shims for missing GL 1.1 functions in GLES 1.1
#define glColor3f(r, g, b) glColor4f(r, g, b, 1.0f)
#define glOrtho glOrthof
#define glFrustum glFrustumf
#define glClearDepth glClearDepthf

#define gluPerspective(fovy, aspect, zNear, zFar) \
    { \
        float f = 1.0f / tanf(fovy * (M_PI / 360.0f)); \
        float m[16] = { \
            f / aspect, 0, 0, 0, \
            0, f, 0, 0, \
            0, 0, (zFar + zNear) / (zNear - zFar), -1, \
            0, 0, (2.0f * zFar * zNear) / (zNear - zFar), 0 \
        }; \
        glMultMatrixf(m); \
    }
#else
#include <GLFW/glfw3.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

double Util_getTime(void);

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#endif /* COMMON_H */
