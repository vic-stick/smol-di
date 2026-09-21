#pragma once

#include "smol_di/types.hpp"

#include <tuple>
#include <vector>

namespace smol_di {
template <std::meta::info Target, std::meta::info... Types>
consteval bool contains(type_list<Types...>) {
    return ((Target == Types) || ...);
}

template <std::meta::info... Acc, std::meta::info First,
          std::meta::info... Rest>
consteval auto unique_impl(type_list<Acc...> accumulated,
                           type_list<First, Rest...>) {
    if constexpr (contains<First>(accumulated))
        return unique_impl(accumulated, type_list<Rest...>{});
    else
        return unique_impl(type_list<Acc..., First>{}, type_list<Rest...>{});
}
template <std::meta::info... Acc>
consteval auto unique_impl(type_list<Acc...> accumulated, type_list<>) {
    return accumulated;
}
template <std::meta::info... Types>
consteval auto unique(type_list<Types...> list) {
    return unique_impl(type_list<>{}, list);
}
template <std::meta::info... A, std::meta::info... B>
consteval auto concat(type_list<A...>, type_list<B...>) {
    return type_list<A..., B...>{};
}
template <std::meta::info... A>
consteval auto concat_all(type_list<A...> list) {
    return list;
}
template <std::meta::info... A, std::meta::info... B, typename... Rest>
consteval auto concat_all(type_list<A...> a, type_list<B...> b, Rest... rest) {
    return concat_all(type_list<A..., B...>{}, rest...);
}

template <auto... Values> struct replicator_type {
    template <typename F> constexpr auto operator>>(F body) const {
        return std::forward_as_tuple(body.template operator()<Values>()...);
    }
};
template <auto... Values> replicator_type<Values...> replicator{};
template <auto... Values> struct type_list_replicator_type {
    template <typename F> constexpr auto operator>>(F body) const {
        return body.template operator()<Values...>();
    }
};
template <auto... Values>
type_list_replicator_type<Values...> type_list_replicator{};

template <typename R> consteval auto expand(R range) {
    std::vector<std::meta::info> args;
    for (auto r : range)
        args.push_back(std::meta::reflect_constant(r));
    return std::meta::substitute(^^replicator, args);
}
template <typename R> consteval auto expand_type_list(R range) {
    std::vector<std::meta::info> args;
    for (auto r : range)
        args.push_back(std::meta::reflect_constant(
            std::meta::remove_reference(std::meta::type_of(r))));
    return std::meta::substitute(^^type_list_replicator, args);
}
} // namespace smol_di
