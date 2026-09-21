#pragma once

#include "smol_di/detail/container.hpp"

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

template <typename Root, typename... Bindings> auto create_container() {
    return detail::make_container<Root, Bindings...>();
}

} // namespace smol_di
