#ifndef TYPES_H
#define TYPES_H

namespace sim {
    using id_t = uint16_t; // Entity ID type

    constexpr id_t NO_ID = std::numeric_limits<id_t>::max(); // Sentinel value for no ID
}

#endif //TYPES_H
