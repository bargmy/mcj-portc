#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "textures.h"
#include "common.h"

#define MAX_CACHED_TEXTURES 32

typedef struct {
    char name[256];
    int id;
} TextureCacheEntry;

static TextureCacheEntry textureCache[MAX_CACHED_TEXTURES];
static int textureCacheCount = 0;

#ifdef ANDROID_PORT
#include <android/asset_manager.h>
static AAssetManager* assetManager = NULL;
void Textures_setAssetManager(AAssetManager* mgr) {
    assetManager = mgr;
}
#endif

#ifdef ANDROID_PORT
#include <android/log.h>
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "RubyDung", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "RubyDung", __VA_ARGS__))
#endif

int Textures_loadTexture(const char* resourceName, int mode) {
    // Check if texture is already loaded
    for (int i = 0; i < textureCacheCount; i++) {
        if (strcmp(textureCache[i].name, resourceName) == 0) {
            return textureCache[i].id;
        }
    }

    // Prepare filename by stripping leading slash
    const char* filename = resourceName;
    if (filename[0] == '/') {
        filename++;
    }

#ifdef ANDROID_PORT
    LOGI("Loading texture: %s", filename);
#endif

    int width, height, channels;
    unsigned char* pixels = NULL;

#ifdef ANDROID_PORT
    if (assetManager) {
        AAsset* asset = AAssetManager_open(assetManager, filename, AASSET_MODE_BUFFER);
        if (asset) {
            size_t size = AAsset_getLength(asset);
            unsigned char* buffer = (unsigned char*)malloc(size);
            AAsset_read(asset, buffer, size);
            AAsset_close(asset);
            pixels = stbi_load_from_memory(buffer, (int)size, &width, &height, &channels, 4);
            free(buffer);
            if (pixels) {
                LOGI("Loaded %s from assets (%dx%d)", filename, width, height);
            } else {
                LOGE("Failed to parse %s from memory", filename);
            }
        } else {
            LOGE("Failed to open %s from asset manager", filename);
        }
    } else {
        LOGE("Asset manager not set!");
    }
#endif

    if (!pixels) {
        pixels = stbi_load(filename, &width, &height, &channels, 4);
    }

    if (!pixels) {
        fprintf(stderr, "Failed to load texture: %s\n", filename);
        return 0;
    }

    GLuint textureId;
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mode);

#ifdef ANDROID_PORT
    if (mode != GL_NEAREST && mode != GL_LINEAR) {
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
#else
    // Build mipmaps (equivalent to GLU.gluBuild2DMipmaps)
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, width, height, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
#endif

    stbi_image_free(pixels);

    // Store in cache
    if (textureCacheCount < MAX_CACHED_TEXTURES) {
        strncpy(textureCache[textureCacheCount].name, resourceName, sizeof(textureCache[textureCacheCount].name) - 1);
        textureCache[textureCacheCount].id = (int)textureId;
        textureCacheCount++;
    }

    printf("%s -> %d\n", resourceName, textureId);
    return (int)textureId;
}

void Textures_cleanup(void) {
    for (int i = 0; i < textureCacheCount; i++) {
        GLuint id = textureCache[i].id;
        glDeleteTextures(1, &id);
    }
    textureCacheCount = 0;
}
