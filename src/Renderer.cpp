#include "sim/systems/Renderer.h"
#include "sim/components/Transform.h"

#include "raylib-cpp.hpp"

namespace sim {
    void Renderer::start() {
        InitWindow(width_, height_, title_);
        SetTargetFPS(60);
    }

    // template<typename C, typename S>
    void Renderer::end() {
        CloseWindow();
    }

    // template<typename C, typename S>
    void Renderer::render(Context& context) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        context.view<Transform>().for_each([](const Transform& t) {
            DrawRectangle(t.x, t.y, 10, 10, BLUE);
        });

        EndDrawing();
    }

    // template<typename C, typename S>
    [[noreturn]] void Renderer::block() {
        while (true);
    }
}