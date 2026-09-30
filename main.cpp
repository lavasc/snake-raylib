#include <algorithm>
#include <glm/glm.hpp>
#include <raylib.h>
#include <vector>

using namespace glm;
using namespace std;

constexpr ivec2 grid_size{30, 30};
constexpr int cell_size = 30;
constexpr ivec2 window_size{grid_size.x * cell_size, grid_size.y * cell_size};

bool game_over = false;

class snake {
    friend class engine;

public:
    void update(float dt)
    {
        if (can_turn) {
            if (IsKeyPressed(KEY_A) && dir.x != 1) {
                dir = {-1, 0};
                can_turn = false;
            }
            if (IsKeyPressed(KEY_D) && dir.x != -1) {
                dir = {1, 0};
                can_turn = false;
            }
            if (IsKeyPressed(KEY_W) && dir.y != 1) {
                dir = {0, -1};
                can_turn = false;
            }
            if (IsKeyPressed(KEY_S) && dir.y != -1) {
                dir = {0, 1};
                can_turn = false;
            }
        }

        move_timer += dt;

        if (move_timer >= move_delay) {
            move_timer = 0.0f;

            ivec2 next_head = pos.front() + dir;

            if (next_head.x < 0 || next_head.x >= grid_size.x || next_head.y < 0 || next_head.y >= grid_size.y) {
                game_over = true;
                return;
            }

            if (find(pos.begin(), pos.end(), next_head) != pos.end()) {
                game_over = true;
                return;
            }

            pos.insert(pos.begin(), next_head);
            can_turn = true;
            if (!has_eaten) {
                pos.pop_back();
            }
            else {
                has_eaten = false;
            }
        }
    }

    void draw() const
    {
        for (const auto& p : pos) {
            Color color = (&p == &pos.front()) ? DARKGREEN : GREEN;
            DrawRectangle(p.x * cell_size, p.y * cell_size, cell_size - 1, cell_size - 1, color);
        }
    }

private:
    vector<ivec2> pos{{9, 10}, {8, 10}, {7, 10}};
    ivec2 dir{1, 0};
    float move_delay = 0.1f;
    float move_timer = 0.0f;
    bool has_eaten = false;
    bool can_turn = true;
};

class engine {
public:
    engine()
    {
        InitWindow(window_size.x, window_size.y, "snake game");
        SetTargetFPS(60);
        spawn_food();
    }
    ~engine()
    {
        CloseWindow();
    }
    engine(const engine&) = delete;
    engine& operator=(const engine&) = delete;
    engine(engine&&) = delete;
    engine& operator=(engine&&) = delete;

    void run()
    {
        while (!WindowShouldClose()) {
            update();
            render();
        }
    }

private:
    snake player{};
    ivec2 food{};

    void update()
    {
        float dt = GetFrameTime();
        if (!game_over) {
            player.update(dt);
            if (player.pos.front() == food) {
                player.has_eaten = true;
                spawn_food();
            }
        }
        else if (IsKeyPressed(KEY_ENTER)) {
            player = snake{};
            spawn_food();
            game_over = false;
        }
    }
    void render() const
    {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!game_over) {
            player.draw();
            DrawRectangle(food.x * cell_size, food.y * cell_size, cell_size, cell_size, RED);
        }
        else {
            DrawText("GAME OVER", window_size.x / 2 - MeasureText("GAME OVER", 40) / 2, window_size.y / 2 - 80, 40,
                     RED);
            DrawText("PRESS [ENTER] TO PLAY AGAIN",
                     window_size.x / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, window_size.y / 2, 20,
                     GRAY);
        }

        EndDrawing();
    }
    void spawn_food()
    {
        ivec2 random_pos = {GetRandomValue(0, grid_size.x - 1), GetRandomValue(0, grid_size.y - 1)};
        food = random_pos;
    }
};

int main()
{
    engine game{};
    game.run();
}
