#pragma once

#include <meta>
#include <type_traits>

namespace smol_di {

enum class Lifetime { Singleton, Scoped };

template <typename Service, typename Implementation>
struct Binding {
    using service_type = Service;
    using implementation_type = Implementation;

    static constexpr auto service_info = ^^Service;
    static constexpr auto implementation_info = ^^Implementation;
    static constexpr bool is_binding = true;
};

template <typename Service, typename Implementation>
inline constexpr Binding<Service, Implementation> bind{};

template <typename Service, Lifetime LifetimeValue>
struct LifetimeConfig {
    using service_type = Service;

    static constexpr auto service_info = ^^Service;
    static constexpr auto lifetime = LifetimeValue;
    static constexpr bool is_lifetime_config = true;
};

template <typename Service>
inline constexpr LifetimeConfig<Service, Lifetime::Singleton> singleton{};

template <typename Service>
inline constexpr LifetimeConfig<Service, Lifetime::Scoped> scoped{};

template <typename T> struct is_lifetime_config : std::false_type {};

template <typename Service, Lifetime L>
struct is_lifetime_config<LifetimeConfig<Service, L>> : std::true_type {};

template <typename T>
inline constexpr bool is_lifetime_config_v =
    is_lifetime_config<std::remove_cv_t<T>>::value;

template <typename T> struct is_binding : std::false_type {};

template <typename Service, typename Implementation>
struct is_binding<Binding<Service, Implementation>> : std::true_type {};

template <typename T>
inline constexpr bool is_binding_v = is_binding<std::remove_cv_t<T>>::value;

} // namespace smol_di

namespace smol_di::detail {

enum class DependencyKind { Value, LValueReference };

template <std::meta::info Service, std::meta::info Implementation,
          Lifetime Value>
struct Registration {
    using service_type = [:Service:];
    using implementation_type = [:Implementation:];
    static constexpr auto service_info = Service;
    static constexpr auto implementation_info = Implementation;
    static constexpr auto lifetime = Value;
};

template <std::meta::info... Types> struct type_list {};
template <typename... Dependencies> struct dependency_types {};
template <typename... Registrations> struct registration_types {};
template <typename... Bindings> struct binding_types {};

template <std::meta::info Type, DependencyKind Kind> struct Dependency {
    using type = [:Type:];
    static constexpr auto type_info = Type;
    static constexpr auto kind = Kind;
};
} // namespace smol_di::detail
