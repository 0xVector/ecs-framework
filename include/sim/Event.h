#ifndef EVENTS_H
#define EVENTS_H

/// @brief The event namespace contains all the event tag types used in the simulation.
namespace sim::event {
    /// @brief Event fired at the start of the simulation.
    struct SimStart {};

    /// @brief Event fired at the end of the simulation.
    struct SimEnd {};

    /// @brief Event fired before a Cycle event.
    struct PreCycle {};

    /// @brief Event fired every simulation cycle.
    struct Cycle {};

    /// @brief Event fired after a Cycle event.
    struct PostCycle {};

    /// @brief Event fired when rendering is advised, usually after PostCycle.
    struct Render {};
}

#endif //EVENTS_H
