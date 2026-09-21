#pragma once

#include "smol_di/types.hpp"

namespace smol_di {
template <std::meta::info Target, typename... Registrations>
struct find_registration;
template <bool Match, std::meta::info Target, typename First, typename... Rest>
struct find_registration_impl;
template <std::meta::info Target, typename First, typename... Rest>
struct find_registration_impl<true, Target, First, Rest...> {
    using type = First;
};
template <std::meta::info Target, typename First, typename... Rest>
struct find_registration_impl<false, Target, First, Rest...> {
    using type = typename find_registration<Target, Rest...>::type;
};
template <std::meta::info Target, typename First, typename... Rest>
struct find_registration<Target, First, Rest...>
    : find_registration_impl<First::service_info == Target, Target, First,
                             Rest...> {};
template <std::meta::info Target> struct find_registration<Target> {
    static_assert(Target != Target, "Service is not registered");
};

template <std::meta::info Service, typename... Bindings> struct find_binding;
template <bool Match, std::meta::info Service, typename First, typename... Rest>
struct find_binding_impl;
template <std::meta::info Service, typename First, typename... Rest>
struct find_binding_impl<true, Service, First, Rest...> {
    using type = First;
};
template <std::meta::info Service, typename First, typename... Rest>
struct find_binding_impl<false, Service, First, Rest...> {
    using type = typename find_binding<Service, Rest...>::type;
};
template <std::meta::info Service, typename First, typename... Rest>
struct find_binding<Service, First, Rest...>
    : find_binding_impl<First::service_info == Service, Service, First,
                        Rest...> {};
template <std::meta::info Service> struct find_binding<Service> {
    using type = Binding<Service, Service>;
};
template <std::meta::info Service, typename... Bindings>
consteval auto implementation_for() {
    using BindingType = typename find_binding<Service, Bindings...>::type;
    return BindingType::implementation_info;
}
} // namespace smol_di
