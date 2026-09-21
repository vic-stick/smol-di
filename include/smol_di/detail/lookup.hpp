#pragma once

#include "smol_di/detail/types.hpp"

#include <type_traits>

namespace smol_di::detail {
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

template <std::meta::info Service, typename... Bindings>
consteval bool contains_binding() {
    return ((Bindings::service_info == Service) || ...);
}

template <typename Binding>
consteval bool valid_binding() {
    using Service = typename Binding::service_type;
    using Implementation = typename Binding::implementation_type;
    return std::is_convertible_v<Implementation *, Service *>;
}

template <typename... Bindings> struct binding_validator;

template <> struct binding_validator<> {
    static constexpr bool value = true;
};

template <typename First, typename... Rest>
struct binding_validator<First, Rest...> {
    static constexpr bool value =
        valid_binding<First>() &&
        !contains_binding<First::service_info, Rest...>() &&
        binding_validator<Rest...>::value;
};

template <typename... Bindings>
consteval bool validate_bindings() {
    return binding_validator<Bindings...>::value;
}

template <std::meta::info Type, typename BindingList>
struct graph_implementation;

template <std::meta::info Type, typename... Bindings>
struct graph_implementation<Type, binding_types<Bindings...>> {
    static constexpr auto value = implementation_for<Type, Bindings...>();
};

} // namespace smol_di::detail
