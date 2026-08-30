#include "raylib.h"

#define GRID_SIZE 30
#define GRID_COUNT 30

#define MAX_SIZE (GRID_COUNT * GRID_COUNT)

const Vector2 screen = { (float)(GRID_SIZE * GRID_COUNT), (float)(GRID_SIZE * GRID_COUNT) };

float move_counter = 0.0f;
float move_speed = 0.1f;
bool game_over = false;

typedef struct {
    Vector2 position[MAX_SIZE];
    Vector2 speed;
    int length;
} snake_t;

typedef struct {
    Vector2 position;
} food_t;

void update_snake(snake_t *snake, food_t *food);
void draw_snake(const snake_t *snake);
void draw_food(const food_t *food);

int main(void) {
    InitWindow((int)screen.x, (int)screen.y, "Snake Game");
    SetTargetFPS(60);

    snake_t snake = { .length = 3,
                      .speed = { 1.0f, 0.0f },
                      .position = { [0] = { 20.0f, 15.0f }, [1] = { 19.0f, 15.0f }, [2] = { 18.0f, 15.0f } } };
    food_t food = { .position = { 25.0f, 10.0f } };

    while (!WindowShouldClose()) {
        if (!game_over) {
            update_snake(&snake, &food);
        } else {
            if (IsKeyPressed(KEY_ENTER)) {
                snake = (snake_t){ .length = 3,
                                   .speed = { 1.0f, 0.0f },
                                   .position = { { 20, 15 }, { 19, 15 }, { 18, 15 } } };
                food = (food_t){ .position = { 25.0f, 10.0f } };
                move_counter = 0.0f;
                game_over = false;
            }
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!game_over) {
            draw_snake(&snake);
            draw_food(&food);
        } else {
            DrawText("PRESS [ENTER] TO PLAY AGAIN", screen.x / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
                     screen.y / 2 - 50, 20, GRAY);
        }

        EndDrawing();
    }
    CloseWindow();
    return 0;
}

void update_snake(snake_t *snake, food_t *food) {
    if (IsKeyPressed(KEY_UP) && snake->speed.y == 0.0f)
        snake->speed = (Vector2){ 0.0f, -1.0f };
    if (IsKeyPressed(KEY_DOWN) && snake->speed.y == 0.0f)
        snake->speed = (Vector2){ 0.0f, 1.0f };
    if (IsKeyPressed(KEY_LEFT) && snake->speed.x == 0.0f)
        snake->speed = (Vector2){ -1.0f, 0.0f };
    if (IsKeyPressed(KEY_RIGHT) && snake->speed.x == 0.0f)
        snake->speed = (Vector2){ 1.0f, 0.0f };

    move_counter += GetFrameTime();
    if (move_counter >= move_speed) {
        move_counter = 0.0f;
        Vector2 last_tail_pos = snake->position[snake->length - 1];
        for (int i = snake->length - 1; i > 0; i--) {
            snake->position[i] = snake->position[i - 1];
        }
        snake->position[0].x += snake->speed.x;
        snake->position[0].y += snake->speed.y;

        if (snake->position[0].x == food->position.x && snake->position[0].y == food->position.y) {
            if (snake->length < MAX_SIZE) {
                snake->position[snake->length] = last_tail_pos;
                snake->length++;
            }

            food->position.x = (float)GetRandomValue(0, GRID_COUNT - 1);
            food->position.y = (float)GetRandomValue(0, GRID_COUNT - 1);
        }

        for (int i = 1; i < snake->length; i++) {
            if ((snake->position[0].x == snake->position[i].x && snake->position[0].y == snake->position[i].y) ||
                (snake->position[0].x < 0.0f || snake->position[0].x >= GRID_COUNT || snake->position[0].y < 0.0f ||
                 snake->position[0].y >= GRID_COUNT)) {
                game_over = true;
            }
        }
    }
}

void draw_snake(const snake_t *snake) {
    for (int i = 0; i < snake->length; i++) {
        Vector2 p = { snake->position[i].x * (float)GRID_SIZE, snake->position[i].y * (float)GRID_SIZE };
        Color color = (i == 0) ? DARKGREEN : GREEN;

        DrawRectangleV(p, (Vector2){ GRID_SIZE - 1, GRID_SIZE - 1 }, color);
    }
}

void draw_food(const food_t *food) {
    Vector2 p = { food->position.x * (float)GRID_SIZE, food->position.y * (float)GRID_SIZE };

    DrawRectangleV(p, (Vector2){ GRID_SIZE, GRID_SIZE }, RED);
}
