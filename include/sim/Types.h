#ifndef TYPES_H
#define TYPES_H

namespace sim {
    /// @brief Entity ID type
    using id_t = uint16_t;

    /// @brief Sentinel value for no ID
    constexpr id_t NO_ID = std::numeric_limits<id_t>::max();
}

#endif //TYPES_H
