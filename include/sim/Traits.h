#ifndef ENTITYTAGS_H
#define ENTITYTAGS_H
#include <type_traits>

namespace sim {
    template<typename Tag, typename EntityType>
    struct has_tag : std::false_type {};

    template<typename Tag, template<typename, typename...> class EntityType, typename... Components>
    struct has_tag<Tag, EntityType<Tag, Components...> > : std::true_type {};

    template<typename Tag, typename...>
    struct get_entity_type_of_tag {}; // todo: no def?

    template<typename Tag, typename Head, typename... Tail>
    struct get_entity_type_of_tag<Tag, Head, Tail...> {
        using type = std::conditional_t<
            has_tag<Tag, Head>::value,
            Head,
            get_entity_type_of_tag<Tag, Tail...> >;
    };

    template<typename Tag>
    struct get_entity_type_of_tag<Tag> {
        static_assert(sizeof(Tag) && false, "No entity with the specified tag found.");
    };

    template<typename From>
    struct extract_params {
        static_assert(sizeof(From) && false, "Not a valid type.");
    };

    template<template<typename...> class From, typename... Params>
    struct extract_params<From<Params...>> {
        template<template<typename...> class To>
        using to = To<Params...>;
    };

    template<typename From, template<typename...> class To>
    using extracted_to_t = typename extract_params<From>::template to<To>;
}

#endif //ENTITYTAGS_H
