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
template <bool Match, std::meta::info Service, typename First,
          typename... Rest>
struct find_binding_impl;

template <bool IsBinding, std::meta::info Service, typename First,
          typename... Rest>
struct find_binding_kind;

template <std::meta::info Service, typename First, typename... Rest>
struct find_binding_kind<false, Service, First, Rest...> {
    using type = typename find_binding<Service, Rest...>::type;
};

template <std::meta::info Service, typename First, typename... Rest>
struct find_binding_kind<true, Service, First, Rest...> {
    using type = typename find_binding_impl<First::service_info == Service,
                                            Service, First, Rest...>::type;
};
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
    : find_binding_kind<is_binding_v<First>, Service, First, Rest...> {};
template <std::meta::info Service> struct find_binding<Service> {
    using service_type = [:Service:];
    using type = Binding<service_type, service_type>;
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

template <bool IsBinding, typename First, typename... Rest>
struct binding_validator_step;

template <typename First, typename... Rest>
struct binding_validator_step<false, First, Rest...> {
    static constexpr bool value = binding_validator<Rest...>::value;
};

template <typename First, typename... Rest>
struct binding_validator_step<true, First, Rest...> {
    static constexpr bool value =
        valid_binding<First>() &&
        !contains_binding<First::service_info, Rest...>() &&
        binding_validator<Rest...>::value;
};

template <> struct binding_validator<> {
    static constexpr bool value = true;
};

template <typename First, typename... Rest>
struct binding_validator<First, Rest...>
    : binding_validator_step<is_binding_v<First>, First, Rest...> {};

template <bool IsBinding, typename Config> struct binding_is_valid;

template <typename Config>
struct binding_is_valid<false, Config> : std::true_type {};

template <typename Config>
struct binding_is_valid<true, Config>
    : std::bool_constant<valid_binding<Config>()> {};

template <typename... Config>
consteval bool all_bindings_valid() {
    return (binding_is_valid<is_binding_v<Config>, Config>::value && ...);
}

template <typename... Bindings>
consteval bool validate_bindings() {
    return binding_validator<Bindings...>::value;
}

template <std::meta::info Service, typename... Config>
struct find_lifetime;

template <std::meta::info Service>
struct find_lifetime<Service> {
    static constexpr auto value = Lifetime::Singleton;
};

template <bool IsLifetime, std::meta::info Service, typename First,
          typename... Rest>
struct find_lifetime_impl;

template <std::meta::info Service, typename First, typename... Rest>
struct find_lifetime_impl<false, Service, First, Rest...> {
    static constexpr auto value = find_lifetime<Service, Rest...>::value;
};

template <std::meta::info Service, typename First, typename... Rest>
struct find_lifetime_impl<true, Service, First, Rest...> {
    static constexpr auto value =
        First::service_info == Service
            ? First::lifetime
            : find_lifetime<Service, Rest...>::value;
};

template <std::meta::info Service, typename First, typename... Rest>
struct find_lifetime<Service, First, Rest...>
    : find_lifetime_impl<is_lifetime_config_v<First>, Service, First, Rest...> {
};

template <std::meta::info Service, typename... Config>
consteval Lifetime lifetime_for() {
    return find_lifetime<Service, Config...>::value;
}

template <std::meta::info Type, typename BindingList>
struct graph_implementation;

template <std::meta::info Type, typename... Bindings>
struct graph_implementation<Type, binding_types<Bindings...>> {
    static constexpr auto value = implementation_for<Type, Bindings...>();
};

} // namespace smol_di::detail
