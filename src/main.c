#include <raylib.h>

/**********/
/* MACROS */
/**********/

// g = -9.8m/s * 10px/m * 1s/60frames = -1.63px/frame
#define GRAVITY 1.63f
#define TOP_SPEED 8.5f

/*********/
/* TYPES */
/*********/

typedef signed char  s8;
typedef signed short s16;
typedef signed int   s32;
typedef signed long  s64;

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;
typedef unsigned long  u64;

typedef float  f32;
typedef double f64;

typedef struct {
    f32 h, v; // horizontal and vertical axis
    u8 jump;
} Input;

typedef struct {
    u16 x, y;
    u16 width, height;

    f32 speed;

    f32 vx, vy; // velocity
    f32 ax, ay; // acceleration

    bool grounded;
} Player;

typedef struct {
    u16 x, y;
    u16 width, height;
} Wall;

/******************/
/* FUNCTION DECLS */
/******************/

// helpers //
f32 clamp(f32 v, f32 min, f32 max);
f32 sign(f32 f);

// main event loop //
void init(void);
void update(void);
void draw(void);

/***********/
/* GLOBALS */
/***********/

const Wall level[] = {
    { 100, 400, 600, 10 },
    { 100, 300, 10, 100 },
    { 300, 350, 200, 10 },
};

static Input input;
static Player player;
static u32 tick;
static bool quit;

/******************/
/* FUNCTION IMPLS */
/******************/

// helpers //

f32 clamp(f32 v, f32 min, f32 max)
{
    if (v < min) v = min;
    if (v > max) v = max;
    
    return v;
}

f32 sign(f32 f)
{
    return f < 0.0 ? -1.0 : 1.0;
}

// main event loop //
void init(void)
{
    // set up globals
    input = (Input){0};

    player = (Player){
        .x = 400,
        .y = 300,
        .width = 10,
        .height = 10,
        .speed = 1.0,
        .ay = GRAVITY,
    };

    tick = 0;
    quit = false;
}

void update(void)
{
    if (tick++ % 120 == 0) {
        TraceLog(LOG_INFO, "Player pos:(%d, %d) vel:(%f, %f) acc:(%f, %f) grounded:%s", player.x, player.y, player.vx, player.vy, player.ax, player.ay, player.grounded ? "true" : "false");
    }

    // input update //
    input = (Input){0};

    if (IsKeyPressed(KEY_R)) init();
    if (IsKeyPressed(KEY_Q)) quit = true;

    if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))    input.h += 1.0;
    if (IsKeyDown(KEY_LEFT)  || IsKeyDown(KEY_A))    input.h -= 1.0;

    if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_SPACE)) input.jump = true;

    // player update //
    player.ax = player.speed * input.h;

    if (input.jump && player.grounded) player.vy = -player.speed * 15;
    player.grounded = false; // this gets updated later when we perform vertical collision checks

    player.vx = clamp(player.vx + player.ax, -TOP_SPEED, TOP_SPEED);
    player.vy = player.vy + player.ay;

    s16 dx_sign = sign(player.vx);
    u16 dx = player.vx * dx_sign;
    while (dx --> 0) {
        player.x += dx_sign;

        for (u64 i = 0; i < sizeof(level) / sizeof(*level); ++i) {
            Wall w = level[i];

            if (w.x + w.width <= player.x || w.x >= player.x + player.width) continue;
            if (w.y + w.height <= player.y || w.y >= player.y + player.height) continue;

            player.x -= dx_sign;
            dx = 0;
            break;
        }
    }

    s16 dy_sign = sign(player.vy);
    u16 dy = player.vy * dy_sign;
    while (dy --> 0) {
        player.y += dy_sign;

        for (u64 i = 0; i < sizeof(level) / sizeof(*level); ++i) {
            Wall w = level[i];

            if (w.x + w.width <= player.x || w.x >= player.x + player.width) continue;
            if (w.y + w.height <= player.y || w.y >= player.y + player.height) continue;

            player.grounded = true;

            player.y -= dy_sign;
            dy = 0;
            break;
        }
    }

    // Without this, turning around feels "slippery" since the player is accelerating from a speed < 0
    if (sign(player.vx) != sign(input.h) || input.h == 0.0) player.vx = 0.0;

    if (player.grounded) player.vy = 0.0;
}

void draw(void)
{
    ClearBackground(RAYWHITE);

    for (u64 i = 0; i < sizeof(level) / sizeof(*level); ++i) {
        Wall w = level[i];
        DrawRectangle(w.x, w.y, w.width, w.height, DARKGRAY);
    }

    DrawRectangle(player.x, player.y, player.width, player.height, RED);
}

/**************/
/* ENTRYPOINT */
/**************/

int main(void)
{
    init();

    InitWindow(800, 600, "game");
    SetTargetFPS(60);

    while (!WindowShouldClose() && !quit) {
        update();

        BeginDrawing();
            draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
