#include "random.hpp"
#include "raylib.h"
#include "raymath.h"
#include <vector>

constexpr float grid_size{30.0f};
constexpr Vector2 screen{grid_size * grid_size, grid_size * grid_size};

bool game_over{false};

float move_timer{0.0f};
constexpr float move_delay{0.08f};

class Snake {
public:
    Snake(std::vector<Vector2> pos, Vector2 s) : position(std::move(pos)), speed(s), has_eaten(false) {};

    void update(float dt);
    void check_food_collision(class Food& food);
    void dead();
    void draw() const;
    void reset();

private:
    std::vector<Vector2> position;
    Vector2 speed;
    bool has_eaten;
};

class Food {
public:
    Food(Vector2 pos) : position(std::move(pos)) {};

    void draw() const;
    void respawn();
    Vector2 get_position() const
    {
        return position;
    }

private:
    Vector2 position;
};

int main()
{
    InitWindow(static_cast<int>(screen.x), static_cast<int>(screen.y), "Snake Game");
    SetTargetFPS(60);

    Snake snake{{{5, 5}, {4, 5}, {3, 5}}, {1.0f, 0.0f}};
    Food food{{6, 10}};

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (game_over) {
            if (IsKeyPressed(KEY_ENTER)) {
                snake.reset();
                food.respawn();
                game_over = false;
            }
        }
        else {
            snake.update(dt);
            snake.dead();
            snake.check_food_collision(food);
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!game_over) {
            food.draw();
            snake.draw();
        }
        else {
            DrawText("PRESS [ENTER] TO PLAY AGAIN", screen.x / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2,
                     screen.y / 2 - 50, 20, GRAY);
        }

        EndDrawing();
    }
    CloseWindow();
}

void Snake::update(float dt)
{
    const Vector2 head{position.front()};
    move_timer += dt;

    if (IsKeyPressed(KEY_W) && speed.y != 1.0f) {
        speed = {0.0f, -1.0f};
    }
    if (IsKeyPressed(KEY_A) && speed.x != 1.0f) {
        speed = {-1.0f, 0.0f};
    }
    if (IsKeyPressed(KEY_S) && speed.y != -1.0f) {
        speed = {0.0f, 1.0f};
    }
    if (IsKeyPressed(KEY_D) && speed.x != -1.0f) {
        speed = {1.0f, 0.0f};
    }

    if (move_timer >= move_delay) {
        move_timer = 0.0f;
        position.insert(position.begin(), head + speed);
        if (!has_eaten) {
            position.pop_back();
        }
        has_eaten = false;
    }
}

void Snake::check_food_collision(Food& food)
{
    const Vector2 head{position.front()};
    if (head == food.get_position()) {
        food.respawn();
        has_eaten = true;
    }
}

void Snake::dead()
{
    const Vector2 head = position.front();

    if (head.x < 0 || head.y < 0 || head.x >= grid_size || head.y >= grid_size) {
        game_over = true;
    }
    for (size_t i = 1; i < position.size(); ++i) {
        if (head.x == position[i].x && head.y == position[i].y) {
            game_over = true;
        }
    }
}

void Snake::draw() const
{
    for (const auto& pos : position) {
        DrawRectangleV({pos.x * grid_size, pos.y * grid_size}, {grid_size, grid_size}, GREEN);
    }
}

void Snake::reset()
{
    position = {{5, 5}, {4, 5}, {3, 5}};
    speed = {1.0f, 0.0f};
    has_eaten = false;
}

void Food::respawn()
{
    position.x = static_cast<float>(ls::random::range(0, static_cast<int>(grid_size) - 1));
    position.y = static_cast<float>(ls::random::range(0, static_cast<int>(grid_size) - 1));
}

void Food::draw() const
{
    DrawRectangleV({position.x * grid_size, position.y * grid_size}, {grid_size, grid_size}, RED);
}
