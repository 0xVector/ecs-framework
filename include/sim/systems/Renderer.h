#ifndef RENDERER_H
#define RENDERER_H
#include "sim/Simulation.h"

namespace sim {
    class Renderer {
        static constexpr int width_ = 800;
        static constexpr int height_ = 450;
        static constexpr char title_[] = "Simulation Renderer";

    public:
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
            end();
        } else if constexpr (std::same_as<Event, event::Cycle>) {
            render(context);
        }
    }
}

#endif //RENDERER_H
