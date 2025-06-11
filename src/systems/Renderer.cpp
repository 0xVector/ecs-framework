#include "raylib-cpp.hpp"

#include "sim/lib/systems/Renderer.h"
#include "sim/lib/components/Transform.h"
#include "sim/lib/components/Sprite.h"

static constexpr int MARGIN = 10; // Margin around the screen

namespace sim::lib {
    struct Renderer::State {
        static constexpr char TITLE[] = "Simulation Renderer";
        static constexpr int WIDTH = 1000 + 2 * MARGIN;
        static constexpr int HEIGHT = 1000 + 2 * MARGIN;
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
                const int x = t.x - sprite.width / 2 + MARGIN;
                const int y = t.y - sprite.height / 2 + MARGIN;
                DrawRectangle(x, y, sprite.width, sprite.height, toRaylibColor(sprite.color));
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

    void Renderer::render(Context context) const {
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
            // TODO - handle events properly
            PollInputEvents();
            WaitTime(0.01);
        }
    }
}
