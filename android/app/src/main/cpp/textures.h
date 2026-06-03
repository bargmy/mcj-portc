#ifndef TEXTURES_H
#define TEXTURES_H

int Textures_loadTexture(const char* resourceName, int mode);
void Textures_cleanup(void);

#ifdef ANDROID_PORT
#include <android/asset_manager.h>
void Textures_setAssetManager(AAssetManager* mgr);
#endif

#endif /* TEXTURES_H */
