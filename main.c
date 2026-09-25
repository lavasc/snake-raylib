#include <raylib.h>
#include <raymath.h>

#define GRID_SIZE 30
#define SCREEN_WIDTH (GRID_SIZE * GRID_SIZE)
#define SCREEN_HEIGHT (GRID_SIZE * GRID_SIZE)
#define MAX_LENGTH (GRID_SIZE * GRID_SIZE)
#define HEAD 0

typedef struct {
    Vector2 pos[MAX_LENGTH];
    Vector2 dir;
    int length;
} snake_t;

typedef struct {
    snake_t snake;
    Vector2 food;
    float move_delay;
    float move_timer;
    bool can_turn;
    bool game_over;
} Context;

static Context default_context = {};

static void context_update(Context *ctx)
{
    if (!ctx->game_over) {
        // Handle input
        if (ctx->can_turn) {
            if (IsKeyPressed(KEY_W) && ctx->snake.dir.y != 1.0f) {
                ctx->snake.dir = (Vector2){ 0.0f, -1.0f };
                ctx->can_turn = false;
            }
            if (IsKeyPressed(KEY_S) && ctx->snake.dir.y != -1.0f) {
                ctx->snake.dir = (Vector2){ 0.0f, 1.0f };
                ctx->can_turn = false;
            }
            if (IsKeyPressed(KEY_A) && ctx->snake.dir.x != 1.0f) {
                ctx->snake.dir = (Vector2){ -1.0f, 0.0f };
                ctx->can_turn = false;
            }
            if (IsKeyPressed(KEY_D) && ctx->snake.dir.x != -1.0f) {
                ctx->snake.dir = (Vector2){ 1.0f, 0.0f };
                ctx->can_turn = false;
            }
        }

        // Update snake
        ctx->move_timer += GetFrameTime();
        if (ctx->move_timer > ctx->move_delay) {
            ctx->move_timer = 0.0f;
            ctx->can_turn = true;
            for (int i = ctx->snake.length - 1; i > HEAD; --i) {
                ctx->snake.pos[i] = ctx->snake.pos[i - 1];
            }
            ctx->snake.pos[HEAD] = Vector2Add(ctx->snake.pos[HEAD], ctx->snake.dir);
        }

        // Eat Food
        if (Vector2Equals(ctx->snake.pos[HEAD], ctx->food)) {
            ctx->food = (Vector2){ GetRandomValue(0, GRID_SIZE - 1), GetRandomValue(0, GRID_SIZE - 1) };
            ctx->snake.pos[ctx->snake.length] = ctx->snake.pos[ctx->snake.length - 1];
            ctx->snake.length++;
        }
        // Check self collision
        for (int i = HEAD + 1; i < ctx->snake.length; ++i) {
            if (Vector2Equals(ctx->snake.pos[HEAD], ctx->snake.pos[i])) {
                ctx->game_over = true;
                break;
            }
        }

        // Check wall collision
        if (ctx->snake.pos[HEAD].y < 0 || ctx->snake.pos[HEAD].y >= GRID_SIZE || ctx->snake.pos[HEAD].x < 0 || ctx->snake.pos[HEAD].x >= GRID_SIZE) {
            ctx->game_over = true;
        }
    } else {
        if (IsKeyPressed(KEY_ENTER)) {
            *ctx = default_context;
        }
    }
}

static void context_draw(Context *ctx)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);

    if (!ctx->game_over) {
        // Draw snake
        for (int i = 0; i < ctx->snake.length; ++i) {
            Color color = (i == HEAD) ? DARKGREEN : GREEN;
            DrawRectangleRec((Rectangle){ ctx->snake.pos[i].x * GRID_SIZE, ctx->snake.pos[i].y * GRID_SIZE, GRID_SIZE - 1, GRID_SIZE - 1 }, color);
        }

        // Draw Food
        DrawRectangleRec((Rectangle){ ctx->food.x * GRID_SIZE, ctx->food.y * GRID_SIZE, GRID_SIZE, GRID_SIZE }, RED);
    } else {
        DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 40) / 2, SCREEN_HEIGHT / 2 - 80, 40, RED);
        DrawText("PRESS [ENTER] TO PLAY AGAIN", SCREEN_WIDTH / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, SCREEN_HEIGHT / 2, 20, GRAY);
    }
    DrawText(TextFormat("SCORE: %d", ctx->snake.length), SCREEN_WIDTH - MeasureText(TextFormat("SCORE: %d", ctx->snake.length), 20) - 20, 20, 20, BLACK);

    EndDrawing();
}

int main(void)
{
    Context context = {
        .snake = {
            .pos = {
                [0] = { 15.0f, 15.0f },
                [1] = { 14.0f, 15.0f },
                [2] = { 13.0f, 15.0f } },
            .dir = { 1.0f, 0.0f },
            .length = 3 },
        .food = { 18.0f, 18.0f },
        .move_delay = 0.075f,
        .move_timer = 0.0f,
        .can_turn = true,
        .game_over = false
    };

    default_context = context;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "snake game");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        context_update(&context);
        context_draw(&context);
    }
    CloseWindow();
    return 0;
}
