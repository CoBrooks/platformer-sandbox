#include <stdlib.h>
#include <raylib.h>

/**********/
/* MACROS */
/**********/

// g = 9.8m/s * 10px/m * 1s/60frames = 1.63px/frame
#define GRAVITY 1.63f
#define TOP_SPEED 8.5f

#define da_append(array, x) do { \
    if (array.len >= array.cap) { \
        if (array.cap == 0) array.cap = 256; \
        else array.cap *= 2; \
        array.ptr = realloc(array.ptr, array.cap * sizeof(*array.ptr)); \
    } \
    array.ptr[array.len++] = x; \
} while (0)

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
} Actor;

typedef struct {
    Actor *ptr;
    u32    len;
    u32    cap;
} Actors;

typedef struct {
    u16 x, y;
    u16 width, height;

    bool collidable;
} Solid;

typedef struct {
    Solid *ptr;
    u32    len;
    u32    cap;
} Solids;

typedef struct {
    Actor actor;

    f32 vx, vy; // velocity
    f32 ax, ay; // acceleration
    bool grounded;

    f32 speed;
    f32 jump_speed;
} Player;

/******************/
/* FUNCTION DECLS */
/******************/

// helpers //
f32 clamp(f32 v, f32 min, f32 max);
f32 sign(f32 f);

// physics //
bool actor_move_x(Actor *actor, s16 dx, Solids solids);
bool actor_move_y(Actor *actor, s16 dy, Solids solids);

void solid_move(Solid *solid, s16 dx, s16 dy, Actors actors);

// main event loop //
void init(void);
void update(void);
void draw(void);
void deinit(void);

/***********/
/* GLOBALS */
/***********/

Player player;
Input input = {0};

Actors actors = {0};
Solids solids = {0};

u32 tick = 0;
bool quit = false;

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

// physics //
bool actor_move_x(Actor *actor, s16 dx, Solids solids)
{
    s16 dx_sign = dx < 0 ? -1 : 1;
    u16 x_steps = dx * dx_sign;
    while (x_steps --> 0) {
        actor->x += dx_sign;

        for (u64 i = 0; i < solids.len; ++i) {
            Solid solid = solids.ptr[i];
            if (!solid.collidable) continue;

            if (solid.x + solid.width <= actor->x || solid.x >= actor->x + actor->width) continue;
            if (solid.y + solid.height <= actor->y || solid.y >= actor->y + actor->height) continue;

            actor->x -= dx_sign;
            return true;
        }
    }

    return false;
}

bool actor_move_y(Actor *actor, s16 dy, Solids solids)
{
    s16 dy_sign = dy < 0 ? -1 : 1;
    u16 y_steps = dy * dy_sign;
    while (y_steps --> 0) {
        actor->y += dy_sign;

        for (u64 i = 0; i < solids.len; ++i) {
            Solid solid = solids.ptr[i];
            if (!solid.collidable) continue;

            if (solid.x + solid.width <= actor->x || solid.x >= actor->x + actor->width) continue;
            if (solid.y + solid.height <= actor->y || solid.y >= actor->y + actor->height) continue;

            actor->y -= dy_sign;
            return true;
        }
    }

    return false;
}

void solid_move(Solid *solid, s16 dx, s16 dy, Actors actors)
{
    // TODO
}

// main event loop //
void init(void)
{
    player = (Player){
        .actor = (Actor){
            .x = 400,
            .y = 300,
            .width = 10,
            .height = 10,
        },
        .speed = 1.0,
        .jump_speed = 15.0,
        .ay = GRAVITY,
    };

    const Solid level[] = {
        { 100, 400, 600, 10, true },
        { 100, 300, 10, 100, true },
        { 300, 350, 200, 10, true },
    };

    for (u64 i = 0; i < sizeof(level) / sizeof(*level); ++i) {
        da_append(solids, level[i]);
    }
}

void update(void)
{
    if (tick++ % 120 == 0) {
        TraceLog(
            LOG_INFO,
            "Player pos:(%d, %d) vel:(%f, %f) acc:(%f, %f) grounded:%s",
            player.actor.x,  player.actor.y,
            player.vx, player.vy,
            player.ax, player.ay,
            player.grounded ? "true" : "false"
        );
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

    if (input.jump && player.grounded) player.vy = -player.jump_speed;

    player.vx = clamp(player.vx + player.ax, -TOP_SPEED, TOP_SPEED);
    actor_move_x(&player.actor, player.vx, solids);

    // Without this, turning around feels "slippery" since the player is accelerating from a speed < 0
    if (sign(player.vx) != sign(input.h) || input.h == 0.0) player.vx = 0.0;

    player.vy = player.vy + player.ay;
    player.grounded = actor_move_y(&player.actor, player.vy, solids);

    if (player.grounded) player.vy = 0.0;
}

void draw(void)
{
    ClearBackground(RAYWHITE);

    for (u64 i = 0; i < solids.len; ++i) {
        Solid solid = solids.ptr[i];
        DrawRectangle(solid.x, solid.y, solid.width, solid.height, DARKGRAY);
    }

    DrawRectangle(player.actor.x, player.actor.y, player.actor.width, player.actor.height, RED);
}

void deinit(void)
{
    if (solids.ptr) free(solids.ptr);
    if (actors.ptr) free(actors.ptr);
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
    deinit();
    return 0;
}
