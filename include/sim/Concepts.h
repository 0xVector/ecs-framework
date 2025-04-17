#ifndef CONCEPTS_H
#define CONCEPTS_H

template<typename Item, typename... Collection>
concept OneOf = (std::same_as<std::remove_cvref_t<Item>, Collection> || ...);

#endif //CONCEPTS_H
