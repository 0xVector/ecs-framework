#ifndef RENDERER_H
#define RENDERER_H
#include <memory>
#include "sim/Simulation.h"

namespace sim {
    class Renderer {
        struct State;
        std::unique_ptr<State> state_;

    public:
        Renderer();
        ~Renderer();

        template<typename Event>
        void operator()(const Event& event, Context context);

        void start();

        void end() const;

        void render(Context context) const;

        void wait() const;
    };

    template<typename Event>
    void Renderer::operator()(const Event&, const Context context) {
        if constexpr (std::same_as<Event, event::SimStart>) {
            start();
        } else if constexpr (std::same_as<Event, event::SimEnd>) {
            end();
        } else if constexpr (std::same_as<Event, event::Render>) {
            render(context);
        }
    }
}

#endif //RENDERER_H
