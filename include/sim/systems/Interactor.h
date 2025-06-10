#ifndef INTERACTOR_H
#define INTERACTOR_H
#include "sim/Event.h"
#include "sim/View.h"
#include "sim/components/Interactions.h"
#include "sim/components/Transform.h"
#include "sim/utils/TransformUtils.h"

namespace sim {
    template<typename... Touchables>
    struct TouchableTargets {
        void operator()(const event::Cycle, const Context ctx) const {
            (process<Touchables>(ctx), ...);
        }

    private:
        template<typename Touchable>
        void process(Context ctx) const {
            ctx.view<Transform, DestroyByTouch<Touchable> >()
                    .for_each([&](const Entity& toucher, auto& t, auto& td) {
                            const dim_t min_dist_squared = td.min_distance * td.min_distance;
                            ctx.view<Touchable>().for_each([&](Entity& touched, auto&) {
                                if (toucher.id() != touched.id()
                                    && dist_squared(t, touched.get<Transform>()) < min_dist_squared) {
                                    ctx.remove_entity(touched);
                                }
                            });
                        }
                    );
        }
    };
}

#endif //INTERACTOR_H
