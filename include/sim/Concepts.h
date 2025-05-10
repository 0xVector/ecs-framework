#ifndef CONCEPTS_H
#define CONCEPTS_H

template<typename Item, typename... Collection>
concept OneOf = (std::same_as<std::remove_cvref_t<Item>, Collection> || ...);

template<typename Ctx>
concept ContextC = requires(Ctx ctx) {
    {ctx.cycle()} -> std::same_as<size_t>;
};

#endif //CONCEPTS_H
