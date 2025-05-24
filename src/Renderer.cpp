#include "sim/systems/Renderer.h"
#include "sim/components/Transform.h"

#include "raylib-cpp.hpp"
#include "sim/components/Sprite.h"

namespace sim {
    struct Renderer::State {
        static constexpr char TITLE[] = "Simulation Renderer";
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 450;
        static constexpr int ENTITY_SIZE = 10;
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

        context.view<Transform>().for_each([](EntityHandle entity, const Transform& t) {
            const Color color = entity.has<Sprite>() ? entity.get<Sprite>().color : Color{0, 0, 0, 255};
            DrawRectangle(t.x, t.y, 10, 10, detail::Helper::toRaylibColor(color));
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
