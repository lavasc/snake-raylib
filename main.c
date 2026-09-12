#include "raylib.h"
#include <stdlib.h>

#define GRID_SIZE 30
const int screen_width = GRID_SIZE * GRID_SIZE;
const int screen_height = GRID_SIZE * GRID_SIZE;

float move_timer = 0.0f;
float move_delay = 0.1f;

struct snake {
    Vector2 position[GRID_SIZE * GRID_SIZE];
    Vector2 speed;
    int length;
    bool can_turn;
};

struct state {
    struct snake snake;
    Vector2 food;
    float dt;
    bool game_over;
};

void update(struct state *state);
void draw(const struct state *state);

int main(void) {
    InitWindow(screen_width, screen_height, "snake game");
    SetTargetFPS(60);

    struct state state = {
        .snake = { .position = { [0] = { 20.0f, 15.0f }, [1] = { 19.0f, 15.0f }, [2] = { 18.0f, 15.0f } },
                   .speed = { 1.0f, 0.0f },
                   .length = 3,
                   .can_turn = true },
        .food = { 20.0f, 20.0f }
    };
    while (!WindowShouldClose()) {
        state.dt = GetFrameTime();
        update(&state);
        draw(&state);
    }
    CloseWindow();
    return 0;
}

void update(struct state *state) {
    if (!state->game_over) {
        move_timer += state->dt;
        Vector2 *head = &state->snake.position[0];
        // 키 입력
        if (IsKeyPressed(KEY_W) && state->snake.speed.y != 1.0f && state->snake.can_turn) {
            state->snake.speed = (Vector2){ 0.0f, -1.0f };
            state->snake.can_turn = false;
        }
        if (IsKeyPressed(KEY_A) && state->snake.speed.x != 1.0f && state->snake.can_turn) {
            state->snake.speed = (Vector2){ -1.0f, 0.0f };
            state->snake.can_turn = false;
        }
        if (IsKeyPressed(KEY_S) && state->snake.speed.y != -1.0f && state->snake.can_turn) {
            state->snake.speed = (Vector2){ 0.0f, 1.0f };
            state->snake.can_turn = false;
        }
        if (IsKeyPressed(KEY_D) && state->snake.speed.x != -1.0f && state->snake.can_turn) {
            state->snake.speed = (Vector2){ 1.0f, 0.0f };
            state->snake.can_turn = false;
        }
        // 뱀 움직임
        if (move_timer >= move_delay) {
            move_timer = 0.0f;
            for (size_t i = state->snake.length - 1; i > 0; --i) {
                state->snake.position[i] = state->snake.position[i - 1];
            }
            head->x += state->snake.speed.x;
            head->y += state->snake.speed.y;
            state->snake.can_turn = true;
        }
        // 벽 충돌
        if (head->x < 0 || head->x >= GRID_SIZE || head->y < 0 || head->y >= GRID_SIZE) {
            state->game_over = true;
        }
        // 몸 충돌
        for (size_t i = state->snake.length - 1; i > 0; --i) {
            if (head->x == state->snake.position[i].x && head->y == state->snake.position[i].y) {
                state->game_over = true;
            }
        }
        // 음식 충돌
        if (head->x == state->food.x && head->y == state->food.y) {
            state->snake.length++;
            state->snake.position[state->snake.length - 1] = state->snake.position[state->snake.length - 2];
            state->food = (Vector2){ (float)GetRandomValue(0, GRID_SIZE - 1), (float)GetRandomValue(0, GRID_SIZE - 1) };
        }
    } else {
        if (IsKeyPressed(KEY_ENTER)) {
            state->snake.length = 3;
            state->snake.position[0] = (Vector2){ 20.0f, 15.0f };
            state->snake.position[1] = (Vector2){ 19.0f, 15.0f };
            state->snake.position[2] = (Vector2){ 18.0f, 15.0f };
            state->snake.speed = (Vector2){ 1.0f, 0.0f };
            state->snake.can_turn = true;

            state->food = (Vector2){ (float)GetRandomValue(0, GRID_SIZE - 1), (float)GetRandomValue(0, GRID_SIZE - 1) };
            move_timer = 0.0f;
            state->game_over = false;
        }
    }
}

void draw(const struct state *state) {
    BeginDrawing();
    ClearBackground(BLACK);

    if (!state->game_over) {
        DrawRectangleV((Vector2){ (float)state->food.x * GRID_SIZE, state->food.y * (float)GRID_SIZE },
                       (Vector2){ (float)GRID_SIZE, (float)GRID_SIZE }, RED);
        for (size_t i = 0; i < state->snake.length; ++i) {
            Vector2 pos = { state->snake.position[i].x * GRID_SIZE, state->snake.position[i].y * GRID_SIZE };
            DrawRectangleV(pos, (Vector2){ (float)GRID_SIZE, (float)GRID_SIZE }, GREEN);
        }
    } else {
        DrawText("PRESS [ENTER] TO PLAY AGAIN", screen_width / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
                 screen_height / 2 - 50, 20, WHITE);
    }

    EndDrawing();
}
