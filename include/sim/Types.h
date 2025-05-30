#ifndef TYPES_H
#define TYPES_H

namespace sim {
    using id_t = uint16_t; // Entity ID type
    using dim_t = int; // Dimension type for coordinates and sizes

    constexpr id_t NO_ID = std::numeric_limits<id_t>::max(); // Sentinel value for no ID
}

#endif //TYPES_H
