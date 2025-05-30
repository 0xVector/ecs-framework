#ifndef EVENTS_H
#define EVENTS_H

namespace sim::event {
    struct SimStart {};
    struct SimEnd {};
    struct PreCycle {};
    struct Cycle {};
    struct PostCycle {};
}

#endif //EVENTS_H
