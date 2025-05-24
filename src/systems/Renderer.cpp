#include "sim/systems/Renderer.h"
#include "sim/components/Transform.h"

#include "raylib-cpp.hpp"
#include "sim/components/Sprite.h"

namespace sim {
    struct Renderer::State {
        static constexpr char TITLE[] = "Simulation Renderer";
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 450;
        raylib::Window window;

        State() : window(WIDTH, HEIGHT, TITLE) {
            window.SetTargetFPS(60);
        }
    };

    namespace detail {
        struct Helper {
            static raylib::Color toRaylibColor(const Color& color) {
                return {color.r, color.g, color.b, color.a};
            }

            static void draw_sprite(const Transform& t, const Sprite& sprite) {
                DrawRectangle(t.x, t.y, sprite.width, sprite.height, toRaylibColor(sprite.color));
            }
        };
    }

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

        state_->window.ClearBackground(raylib::RAYWHITE);

        context.view<Transform, Sprite>().for_each(
            [](const Transform& t, const Sprite& sprite) {
                detail::Helper::draw_sprite(t, sprite);
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
