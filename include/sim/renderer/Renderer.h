#ifndef RENDERER_H
#define RENDERER_H
#include "sim/Simulation.h"
#include "sim/components/Transform.h"
#include "raylib.h"

namespace sim {
    // template<typename C, typename S>
    class Renderer {
        // using sim_t = Simulation<C, S>;
        // sim_t* sim_;
        static constexpr int width_ = 800;
        static constexpr int height_ = 450;
        static constexpr char title_[] = "Simulation Renderer";

    public:
        // explicit Renderer(sim_t* sim);

        template<typename Event>
        void operator()(const Event& event, Context& context) const;

        static void start();

        static void end();

        static void render(Context& context);

        [[noreturn]] static void block();
    };

    template<typename Event>
    void Renderer::operator()(const Event&, Context& context) const {
        if constexpr (std::same_as<Event, event::SimStart>) {
            start();
        } else if constexpr (std::same_as<Event, event::SimEnd>) {
            // block();
            end();
        } else if constexpr (std::same_as<Event, event::Cycle>) {
            render(context);
        }
    }

    // template<typename C, typename S>
    // Renderer<C, S>::Renderer(sim_t* sim): sim_(sim) {}

    // template<typename C, typename S>
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

#endif //RENDERER_H
