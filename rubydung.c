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
static bool useVulkan = false;
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
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    int viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    gluPickMatrix((float)x, (float)y, 5.0f, 5.0f, viewport);
    gluPerspective(70.0f, (float)width / (float)height, 0.05f, 1000.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    moveCameraToPlayer(a);
}

static void pick(float a) {
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
}

// Mouse button handler
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (action == GLFW_PRESS && hitResult != NULL) {
        // Right button (button == 1 in LWJGL) -> GLFW_MOUSE_BUTTON_RIGHT
        if (button == GLFW_MOUSE_BUTTON_RIGHT) {
            Level_setTile(level, hitResult->x, hitResult->y, hitResult->z, 0);
        }
        // Left button (button == 0 in LWJGL) -> GLFW_MOUSE_BUTTON_LEFT
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
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
        }
    }
}

// Keyboard key handler
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
        // Enter key (key == 28 in LWJGL) -> GLFW_KEY_ENTER
        if (key == GLFW_KEY_ENTER) {
            Level_save(level);
            printf("Level saved successfully.\n");
        }
        // Escape key to quit
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

void initGame(GLFWwindow* window) {
    int col = 920330;
    float fr = 0.5f;
    float fg = 0.8f;
    float fb = 1.0f;

    fogColor[0] = (float)(col >> 16 & 0xFF) / 255.0f;
    fogColor[1] = (float)(col >> 8 & 0xFF) / 255.0f;
    fogColor[2] = (float)(col & 0xFF) / 255.0f;
    fogColor[3] = 1.0f;

    glfwGetFramebufferSize(window, &width, &height);
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

    level = Level_create(256, 256, 64);
    levelRenderer = LevelRenderer_create(level);
    player = Player_create(level);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    for (int i = 0; i < 100; i++) {
        zombies[zombieCount++] = Zombie_create(level, 128.0f, 0.0f, 128.0f);
    }

    // Force immediate pre-rebuild of all chunks to eliminate gameplay loading lag and rendering holes
    printf("Pre-building chunks for seamless rendering... ");
    fflush(stdout);
    for (int i = 0; i < levelRenderer->chunkCount; i++) {
        Chunk_render(levelRenderer->chunks[i], 0);
    }
    printf("Done!\n");
}

// Retro arcade vector font rendering
static void draw_segment(float x1, float y1, float x2, float y2, float x, float y, float size) {
    glVertex2f(x + x1 * size, y + y1 * size);
    glVertex2f(x + x2 * size, y + y2 * size);
}

static void draw_char(char c, float x, float y, float size) {
    bool s[7] = {0};
    switch (c) {
        case '0': s[0]=s[1]=s[2]=s[4]=s[5]=s[6]=1; break;
        case '1': s[2]=s[5]=1; break;
        case '2': s[0]=s[2]=s[3]=s[4]=s[6]=1; break;
        case '3': s[0]=s[2]=s[3]=s[5]=s[6]=1; break;
        case '4': s[1]=s[2]=s[3]=s[5]=1; break;
        case '5': s[0]=s[1]=s[3]=s[5]=s[6]=1; break;
        case '6': s[0]=s[1]=s[3]=s[4]=s[5]=s[6]=1; break;
        case '7': s[0]=s[2]=s[5]=1; break;
        case '8': s[0]=s[1]=s[2]=s[3]=s[4]=s[5]=s[6]=1; break;
        case '9': s[0]=s[1]=s[2]=s[3]=s[5]=s[6]=1; break;
        case 'f': s[0]=s[1]=s[3]=1; break;
        case 'p': s[0]=s[1]=s[2]=s[3]=s[4]=1; break;
        case 's': s[0]=s[1]=s[3]=s[5]=s[6]=1; break;
    }
    glBegin(GL_LINES);
    if (s[0]) draw_segment(0, 0, 2, 0, x, y, size);
    if (s[1]) draw_segment(0, 0, 0, 2, x, y, size);
    if (s[2]) draw_segment(2, 0, 2, 2, x, y, size);
    if (s[3]) draw_segment(0, 2, 2, 2, x, y, size);
    if (s[4]) draw_segment(0, 2, 0, 4, x, y, size);
    if (s[5]) draw_segment(2, 2, 2, 4, x, y, size);
    if (s[6]) draw_segment(0, 4, 2, 4, x, y, size);
    glEnd();
}

static void draw_string(const char* str, float x, float y, float size) {
    while (*str) {
        draw_char(*str, x, y, size);
        x += 3.5f * size;
        str++;
    }
}

static void drawFPS() {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, width, height, 0.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_FOG);

    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(2.0f);

    char fpsText[64];
    if (useVulkan) {
        sprintf(fpsText, "%d fps (Vulkan)", currentFPS);
    } else {
        sprintf(fpsText, "%d fps", currentFPS);
    }
    draw_string(fpsText, 15.0f, 15.0f, 4.0f);

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
    Player_tick(player, glfwGetCurrentContext());
}

void renderGame(GLFWwindow* window, float a) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);
    if (firstMouse) {
        lastMouseX = mouseX;
        lastMouseY = mouseY;
        firstMouse = false;
    }
    float dx = (float)(mouseX - lastMouseX);
    float dy = (float)(lastMouseY - mouseY); // Inverted GLFW screen Y coords to match game rotation space
    lastMouseX = mouseX;
    lastMouseY = mouseY;

    Entity_turn(&player->base, dx, dy);

    pick(a);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    setupCamera(a);

    glEnable(GL_CULL_FACE);
    glEnable(GL_FOG);
    glFogi(GL_FOG_MODE, GL_EXP);
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

    // Draw FPS overlay in the corner
    drawFPS();
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

int main(int argc, char* argv[]) {
    // Seed random number generator
    srand((unsigned int)time(NULL));

    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    // Create Legacy/Compatibility OpenGL context
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(1024, 768, "RubyDung (C Port)", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // Enable VSync to lock framerate and eliminate motion lag
    glfwSwapInterval(1);

    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--vulkan") == 0) {
            useVulkan = true;
        }
    }

    initGame(window);

    gameTimer = Timer_create(60.0f);

    long lastTime = (long)(glfwGetTime() * 1000.0);
    int frames = 0;

    while (!glfwWindowShouldClose(window)) {
        Timer_advanceTime(&gameTimer);

        for (int i = 0; i < gameTimer.ticks; i++) {
            tickGame();
        }

        renderGame(window, gameTimer.a);
        frames++;

        long now = (long)(glfwGetTime() * 1000.0);
        while (now >= lastTime + 1000L) {
            printf("%d fps, %d updates\n", frames, Chunk_updates);
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
