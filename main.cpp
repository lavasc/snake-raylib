#include <algorithm>
#include <glm/glm.hpp>
#include <raylib.h>
#include <vector>
using namespace std;
using namespace glm;

constexpr float gridSize{30.0f};
constexpr vec2 windowSize{gridSize * gridSize, gridSize * gridSize};

bool gameOver{false};

class Food {
    friend class Engine;

private:
    vec2 pos{11.0f, 15.0f};

    void respawn() {
        vec2 randomPos = {static_cast<float>(GetRandomValue(0, static_cast<int>(gridSize) - 1)), static_cast<float>(GetRandomValue(0, static_cast<int>(gridSize) - 1))};
        pos = randomPos;
    }
    void draw() const {
        DrawRectangleRec({pos.x * gridSize, pos.y * gridSize, gridSize, gridSize}, RED);
    }
};

class Snake {
    friend class Engine;

private:
    vector<vec2> pos{{12.0f, 10.0f}, {11.0f, 10.0f}, {10.0f, 10.0f}};
    vec2 dir{1.0f, 0.0f};
    float moveDelay{0.1f};
    float moveTimer{0.0f};
    bool canTurn{true};
    bool hasEaten{false};

    bool isEat(vec2 foodPos) {
        if (pos.front() == foodPos) {
            return true;
        }
        return false;
    }
    void update(float dt) {
        if (canTurn) {
            if (IsKeyPressed(KEY_A) && dir.x != 1.0f) {
                dir = {-1.0f, 0.0f};
                canTurn = false;
            }
            if (IsKeyPressed(KEY_D) && dir.x != -1.0f) {
                dir = {1.0f, 0.0f};
                canTurn = false;
            }
            if (IsKeyPressed(KEY_W) && dir.y != 1.0f) {
                dir = {0.0f, -1.0f};
                canTurn = false;
            }
            if (IsKeyPressed(KEY_S) && dir.y != -1.0f) {
                dir = {0.0f, 1.0f};
                canTurn = false;
            }
        }
        moveTimer += dt;
        if (moveTimer >= moveDelay) {
            moveTimer = 0.0f;

            vec2 nextHead{pos.front() + dir};
            if (nextHead.x < 0 || nextHead.x >= gridSize || nextHead.y < 0 || nextHead.y >= gridSize) {
                gameOver = true;
                return;
            }
            if (find(pos.begin(), pos.end(), nextHead) != pos.end()) {
                gameOver = true;
                return;
            }

            pos.insert(pos.begin(), nextHead);
            canTurn = true;

            if (hasEaten) {
                hasEaten = false;
            } else {
                pos.pop_back();
            }
        }
    }
    void draw() const {
        for (const auto p : pos) {
            DrawRectangleV({p.x * gridSize, p.y * gridSize}, {gridSize, gridSize}, GREEN);
        }
    }
};

class Engine {
public:
    Engine() {
        SetConfigFlags(FLAG_VSYNC_HINT);
        InitWindow(static_cast<int>(windowSize.x), static_cast<int>(windowSize.y), "snake game");

        food.respawn();
    }
    ~Engine() {
        CloseWindow();
    }

    void run() {
        while (!WindowShouldClose()) {
            update();
            draw();
        }
    }

private:
    Snake snake{};
    Food food{};

    void update() {
        const float deltaTime{GetFrameTime()};
        if (!gameOver) {
            snake.update(deltaTime);
            if (snake.isEat(food.pos)) {
                snake.hasEaten = true;
                food.respawn();
            }
        } else {
            if (IsKeyPressed(KEY_ENTER)) {
                snake = Snake();
                food.respawn();
                gameOver = false;
            }
        }
    }
    void draw() const {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (!gameOver) {
            food.draw();
            snake.draw();
        } else {
            DrawText("GAME OVER", windowSize.x / 2 - MeasureText("GAME OVER", 40) / 2, windowSize.y / 2 - 80, 40,
                     RED);
            DrawText("PRESS [ENTER] TO PLAY AGAIN",
                     windowSize.x / 2 - MeasureText("PRESS [ENTER] TO PLAY AGAIN", 20) / 2, windowSize.y / 2, 20,
                     GRAY);
        }

        EndDrawing();
    }
};

int main() {
    Engine engine{};
    engine.run();
}
