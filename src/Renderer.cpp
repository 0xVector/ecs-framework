#include "sim/systems/Renderer.h"
#include "sim/components/Transform.h"

#include "raylib-cpp.hpp"

namespace sim {
    constexpr char TITLE[] = "Simulation Renderer";
    constexpr int WIDTH = 800;
    constexpr int HEIGHT = 450;

    struct Renderer::State {
        raylib::Window window;

        State() : window(WIDTH, HEIGHT, TITLE) {
            window.SetTargetFPS(60);
        }
    };

    Renderer::Renderer() = default;
    Renderer::~Renderer() = default;

    void Renderer::start() {
        state_ = std::make_unique<State>();
    }

    void Renderer::end() const {
        wait();
    }

    void Renderer::render(Context& context) const {
        state_->window.BeginDrawing();

        state_->window.ClearBackground(RAYWHITE);

        context.view<Transform>().for_each([](const Transform& t) {
            DrawRectangle(t.x, t.y, 10, 10, BLUE);
        });

        state_->window.EndDrawing();
    }

    void Renderer::wait() const {
        while (!state_->window.ShouldClose()) {
            state_->window.BeginDrawing();
            state_->window.EndDrawing();
        }
    }
}
