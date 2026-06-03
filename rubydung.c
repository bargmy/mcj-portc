#include "common.h"
#include <time.h>
#include "timer.h"
#include "textures.h"
#include "tile.h"
#include "level.h"
#include "level_renderer.h"
#include "player.h"
#include "character.h"
#include "hit_result.h"
#include "input.h"
#include "font.h"
#include "mobile_ui.h"

// Force high-performance GPU driver selection on AMD and NVIDIA laptops
#ifdef _WIN32
#include <windows.h>
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
#endif

// Global/static variables
static int width = 1024;
static int height = 768;
static float fogColor[4];
static Timer gameTimer;
static int currentFPS = 0;
static Level* level = NULL;
static LevelRenderer* levelRenderer = NULL;
static Player* player = NULL;

#define MAX_ZOMBIES 100
static Zombie* zombies[MAX_ZOMBIES];
static int zombieCount = 0;

static unsigned int selectBuffer[2000];
static HitResult* hitResult = NULL;
static bool hasHitResult = false;
static HitResult activeHitResult;

// Mouse callback variables
static double lastMouseX = 0.0;
static double lastMouseY = 0.0;
static bool firstMouse = true;

static void moveCameraToPlayer(float a) {
    glTranslatef(0.0f, 0.0f, -0.3f);
    glRotatef(player->base.xRot, 1.0f, 0.0f, 0.0f);
    glRotatef(player->base.yRot, 0.0f, 1.0f, 0.0f);
    float x = player->base.xo + (player->base.x - player->base.xo) * a;
    float y = player->base.yo + (player->base.y - player->base.yo) * a;
    float z = player->base.zo + (player->base.z - player->base.zo) * a;
    glTranslatef(-x, -y, -z);
}

static void setupCamera(float a) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70.0f, (float)width / (float)height, 0.05f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    moveCameraToPlayer(a);
}

static void setupPickCamera(float a, int x, int y) {
#ifdef ANDROID_PORT
    (void)a; (void)x; (void)y;
#else
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluPickMatrix((float)x, (float)y, 5.0f, 5.0f, viewport);
    gluPerspective(70.0f, (float)width / (float)height, 0.05f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    moveCameraToPlayer(a);
#endif
}

static void pick(float a) {
#ifdef ANDROID_PORT
    // GLES does not support GL_SELECT.
    hitResult = NULL;
    hasHitResult = false;
#else
    glSelectBuffer(2000, selectBuffer);
    glRenderMode(GL_SELECT);
    setupPickCamera(a, width / 2, height / 2);
    LevelRenderer_pick(levelRenderer, player);
    int hits = glRenderMode(GL_RENDER);

    unsigned int closest = 0xFFFFFFFF;
    int hitNameCount = 0;
    unsigned int names[10];

    int ptr = 0;
    for (int i = 0; i < hits; i++) {
        unsigned int nameCount = selectBuffer[ptr++];
        unsigned int minZ = selectBuffer[ptr++];
        ptr++; // maxZ
        if (minZ >= closest && i != 0) {
            ptr += nameCount;
        } else {
            closest = minZ;
            hitNameCount = nameCount;
            for (int j = 0; j < (int)nameCount; j++) {
                names[j] = selectBuffer[ptr++];
            }
        }
    }

    if (hitNameCount > 0) {
        activeHitResult = HitResult_create(names[0], names[1], names[2], names[3], names[4]);
        hitResult = &activeHitResult;
        hasHitResult = true;
    } else {
        hitResult = NULL;
        hasHitResult = false;
    }
#endif
}

#ifndef ANDROID_PORT
// Mouse button handler
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        MobileUI_handleTouch(button, (float)x, (float)y, true, width, height);
    } else if (action == GLFW_RELEASE) {
        double x, y;
        glfwGetCursorPos(window, &x, &y);
        MobileUI_handleTouch(button, (float)x, (float)y, false, width, height);
    }

    // Logic for breaking/placing blocks
    if (action == GLFW_PRESS && hitResult != NULL) {
        if (button == GLFW_MOUSE_BUTTON_RIGHT || GlobalInput.attack) {
            Level_setTile(level, hitResult->x, hitResult->y, hitResult->z, 0);
            GlobalInput.attack = false;
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT || GlobalInput.place) {
            int x = hitResult->x;
            int y = hitResult->y;
            int z = hitResult->z;
            if (hitResult->f == 0) y--;
            if (hitResult->f == 1) y++;
            if (hitResult->f == 2) z--;
            if (hitResult->f == 3) z++;
            if (hitResult->f == 4) x--;
            if (hitResult->f == 5) x++;
            Level_setTile(level, x, y, z, 1);
            GlobalInput.place = false;
        }
    }
}

// Keyboard key handler
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    bool pressed = (action == GLFW_PRESS || action == GLFW_REPEAT);
    
    if (key == GLFW_KEY_W) GlobalInput.forward = pressed;
    if (key == GLFW_KEY_S) GlobalInput.back = pressed;
    if (key == GLFW_KEY_A) GlobalInput.left = pressed;
    if (key == GLFW_KEY_D) GlobalInput.right = pressed;
    if (key == GLFW_KEY_SPACE) GlobalInput.jump = pressed;

    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ENTER) {
            Level_save(level);
            printf("Level saved successfully.\n");
        }
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }
    }
}

// Window size changed handler
static void framebuffer_size_callback(GLFWwindow* window, int w, int h) {
    width = w;
    height = h;
    glViewport(0, 0, w, h);
}
#endif

void initGame(void* window) {
    (void)window;
    int col = 920330;
    float fr = 0.5f;
    float fg = 0.8f;
    float fb = 1.0f;

    fogColor[0] = (float)(col >> 16 & 0xFF) / 255.0f;
    fogColor[1] = (float)(col >> 8 & 0xFF) / 255.0f;
    fogColor[2] = (float)(col & 0xFF) / 255.0f;
    fogColor[3] = 1.0f;

#ifndef ANDROID_PORT
    glfwGetFramebufferSize((GLFWwindow*)window, &width, &height);
#endif
    glViewport(0, 0, width, height);

    glEnable(GL_TEXTURE_2D);
    glShadeModel(GL_SMOOTH);
    glClearColor(fr, fg, fb, 0.0f);
    glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);

    Tile_init();
    Input_init();
    MobileUI_init();

    level = Level_create(256, 256, 64);
    levelRenderer = LevelRenderer_create(level);
    player = Player_create(level);

    for (int i = 0; i < 100; i++) {
        zombies[zombieCount++] = Zombie_create(level, 128.0f, 0.0f, 128.0f);
    }

    printf("Pre-building chunks... ");
    fflush(stdout);
    for (int i = 0; i < levelRenderer->chunkCount; i++) {
        Chunk_render(levelRenderer->chunks[i], 0);
    }
    printf("Done!\n");
}

static void drawFPS() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, (float)width, (float)height, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);

    glColor3f(1.0f, 1.0f, 0.0f); // Yellow for visibility
    glLineWidth(2.0f);

    char fpsText[64];
    sprintf(fpsText, "%d FPS", currentFPS);
    Font_drawString(fpsText, 15.0f, 15.0f, 4.0f);

    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void tickGame() {
    for (int i = 0; i < zombieCount; i++) {
        Zombie_tick(zombies[i]);
    }
#ifdef ANDROID_PORT
    Player_tick(player, NULL);
#else
    Player_tick(player, glfwGetCurrentContext());
#endif
    
    // Process mobile UI actions
    if (GlobalInput.attack || GlobalInput.place) {
        // Trigger pick if we haven't already
        pick(1.0f);
        if (hasHitResult) {
            if (GlobalInput.attack) Level_setTile(level, hitResult->x, hitResult->y, hitResult->z, 0);
            if (GlobalInput.place) {
                int x = hitResult->x, y = hitResult->y, z = hitResult->z;
                if (hitResult->f == 0) y--; if (hitResult->f == 1) y++;
                if (hitResult->f == 2) z--; if (hitResult->f == 3) z++;
                if (hitResult->f == 4) x--; if (hitResult->f == 5) x++;
                Level_setTile(level, x, y, z, 1);
            }
        }
        GlobalInput.attack = false;
        GlobalInput.place = false;
    }
}

void renderGame(void* window, float a) {
#ifndef ANDROID_PORT
    GLFWwindow* win = (GLFWwindow*)window;
    if (glfwGetInputMode(win, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) {
        double mouseX, mouseY;
        glfwGetCursorPos(win, &mouseX, &mouseY);
        if (firstMouse) {
            lastMouseX = mouseX;
            lastMouseY = mouseY;
            firstMouse = false;
        }
        float dx = (float)(mouseX - lastMouseX);
        float dy = (float)(lastMouseY - mouseY);
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        Entity_turn(&player->base, dx, dy);
    }
#else
    (void)window;
#endif

    pick(a);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setupCamera(a);

    glEnable(GL_CULL_FACE);
    glEnable(GL_FOG);
    glFogf(GL_FOG_MODE, (float)GL_EXP);
    glFogf(GL_FOG_DENSITY, 0.2f);
    glFogfv(GL_FOG_COLOR, fogColor);
    glDisable(GL_FOG);

    LevelRenderer_render(levelRenderer, player, 0);
    for (int i = 0; i < zombieCount; i++) {
        Zombie_render(zombies[i], a);
    }

    glEnable(GL_FOG);
    LevelRenderer_render(levelRenderer, player, 1);
    glDisable(GL_TEXTURE_2D);

    if (hitResult != NULL) {
        LevelRenderer_renderHit(levelRenderer, *hitResult);
    }

    glDisable(GL_FOG);

    drawFPS();
    MobileUI_render(width, height);
}

void destroyGame() {
    Level_save(level);
    Player_destroy(player);
    LevelRenderer_destroy(levelRenderer);
    Level_destroy(level);

    for (int i = 0; i < zombieCount; i++) {
        Zombie_destroy(zombies[i]);
    }
    Textures_cleanup();
}

#ifdef ANDROID_PORT
#include <android_native_app_glue.h>
#include <android/log.h>
#include <EGL/egl.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "RubyDung", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "RubyDung", __VA_ARGS__))

struct Engine {
    struct android_app* app;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    bool animating;
};

static void engine_draw_frame(struct Engine* engine) {
    if (engine->display == NULL) return;

    Timer_advanceTime(&gameTimer);
    for (int i = 0; i < gameTimer.ticks; i++) {
        tickGame();
    }
    renderGame(NULL, gameTimer.a);

    eglSwapBuffers(engine->display, engine->surface);
}

static void engine_term_display(struct Engine* engine) {
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) eglDestroyContext(engine->display, engine->context);
        if (engine->surface != EGL_NO_SURFACE) eglDestroySurface(engine->display, engine->surface);
        eglTerminate(engine->display);
    }
    engine->animating = false;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

static int engine_init_display(struct Engine* engine) {
    LOGI("Initializing EGL Display...");
    const EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_BLUE_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_RED_SIZE, 8,
        EGL_DEPTH_SIZE, 16,
        EGL_NONE
    };
    EGLint w, h, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (display == EGL_NO_DISPLAY) {
        LOGE("eglGetDisplay failed");
        return -1;
    }

    if (!eglInitialize(display, 0, 0)) {
        LOGE("eglInitialize failed");
        return -1;
    }

    if (!eglChooseConfig(display, attribs, &config, 1, &numConfigs) || numConfigs <= 0) {
        LOGE("eglChooseConfig failed or no configs found");
        return -1;
    }

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    if (surface == EGL_NO_SURFACE) {
        LOGE("eglCreateWindowSurface failed");
        return -1;
    }

    // Attempt to create GLES 1.1 context (explicitly or via default)
    context = eglCreateContext(display, config, NULL, NULL);
    if (context == EGL_NO_CONTEXT) {
        LOGE("eglCreateContext failed");
        return -1;
    }

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGE("eglMakeCurrent failed");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    
    width = w;
    height = h;

    LOGI("EGL Initialized. Size: %dx%d", w, h);
    LOGI("GL_VENDOR: %s", glGetString(GL_VENDOR));
    LOGI("GL_RENDERER: %s", glGetString(GL_RENDERER));
    LOGI("GL_VERSION: %s", glGetString(GL_VERSION));

    Textures_setAssetManager(engine->app->activity->assetManager);
    initGame(NULL);

    return 0;
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    struct Engine* engine = (struct Engine*)app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int action = AMotionEvent_getAction(event);
        int pointerIdx = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        int actionCode = action & AMOTION_EVENT_ACTION_MASK;
        int pointerId = AMotionEvent_getPointerId(event, pointerIdx);
        
        float x = AMotionEvent_getX(event, pointerIdx);
        float y = AMotionEvent_getY(event, pointerIdx);

        if (actionCode == AMOTION_EVENT_ACTION_DOWN || actionCode == AMOTION_EVENT_ACTION_POINTER_DOWN) {
            MobileUI_handleTouch(pointerId, x, y, true, engine->width, engine->height);
        } else if (actionCode == AMOTION_EVENT_ACTION_UP || actionCode == AMOTION_EVENT_ACTION_POINTER_UP) {
            MobileUI_handleTouch(pointerId, x, y, false, engine->width, engine->height);
        } else if (actionCode == AMOTION_EVENT_ACTION_MOVE) {
            int count = AMotionEvent_getPointerCount(event);
            for (int i = 0; i < count; i++) {
                MobileUI_handleTouch(AMotionEvent_getPointerId(event, i), AMotionEvent_getX(event, i), AMotionEvent_getY(event, i), true, engine->width, engine->height);
            }
        }
        return 1;
    }
    return 0;
}

static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct Engine* engine = (struct Engine*)app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            if (app->window != NULL) {
                engine_init_display(engine);
                engine->animating = true;
            }
            break;
        case APP_CMD_TERM_WINDOW:
            engine_term_display(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            engine->animating = true;
            break;
        case APP_CMD_LOST_FOCUS:
            engine->animating = false;
            break;
    }
}

void android_main(struct android_app* state) {
    struct Engine engine;
    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    gameTimer = Timer_create(60.0f);

    while (1) {
        int ident;
        int events;
        struct android_poll_source* source;

        while ((ident = ALooper_pollAll(engine.animating ? 0 : -1, NULL, &events, (void**)&source)) >= 0) {
            if (source != NULL) source->process(state, source);
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                destroyGame();
                return;
            }
        }

        if (engine.animating) {
            engine_draw_frame(&engine);
        }
    }
}
#else
int main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    srand((unsigned int)time(NULL));

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "RubyDung Android Port", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    
    glfwSwapInterval(1);

    initGame(window);

    gameTimer = Timer_create(60.0f);
    long lastTime = (long)(Util_getTime() * 1000.0);
    int frames = 0;

    while (!glfwWindowShouldClose(window)) {
        Timer_advanceTime(&gameTimer);
        for (int i = 0; i < gameTimer.ticks; i++) {
            tickGame();
        }
        renderGame(window, gameTimer.a);
        frames++;
        long now = (long)(Util_getTime() * 1000.0);
        while (now >= lastTime + 1000L) {
            currentFPS = frames;
            Chunk_updates = 0;
            lastTime += 1000L;
            frames = 0;
        }
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    destroyGame();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
#endif
