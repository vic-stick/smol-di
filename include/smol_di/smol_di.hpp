#pragma once

#include "smol_di/detail/container.hpp"

#include <type_traits>

namespace smol_di {

template <std::meta::info Service, typename... Bindings>
consteval bool contains_binding() {
    return detail::contains_binding<Service, Bindings...>();
}

template <typename Binding>
consteval bool valid_binding() {
    return detail::valid_binding<Binding>();
}

template <typename... Bindings>
consteval bool validate_bindings() {
    return detail::validate_bindings<Bindings...>();
}

template <typename Root, typename... Config>
auto create_container(Config...) {
    return detail::make_container<Root, std::remove_cvref_t<Config>...>();
}

} // namespace smol_di
