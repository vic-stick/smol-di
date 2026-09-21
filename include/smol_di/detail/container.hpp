#pragma once

#include "smol_di/detail/lookup.hpp"
#include "smol_di/detail/reflection.hpp"

#include <optional>
#include <tuple>

namespace smol_di::detail {
template <typename... Registrations> struct Container;
template <typename... Registrations> class Scope;
template <typename Registration> struct Holder;
template <typename T, typename Resolver>
T construct_from_dependencies(Resolver &resolver);

template <std::meta::info Service, std::meta::info Implementation>
struct Holder<Registration<Service, Implementation, Lifetime::Singleton>> {
    using T = [:Implementation:];

    static constexpr auto lifetime = Lifetime::Singleton;

    std::optional<T> instance;
    template <typename Resolver> T &get(Resolver &resolver) {
        if (!instance)
            instance.emplace(construct_from_dependencies<T>(resolver));
        return *instance;
    }
};

template <std::meta::info Service, std::meta::info Implementation>
struct Holder<Registration<Service, Implementation, Lifetime::Scoped>> {
    using T = [:Implementation:];

    static constexpr auto lifetime = Lifetime::Scoped;

    std::optional<T> instance;
    template <typename Resolver> T &get(Resolver &resolver) {
        if (!instance)
            instance.emplace(construct_from_dependencies<T>(resolver));
        return *instance;
    }
};

template <typename... Registrations> struct Container {
    std::tuple<Holder<Registrations>...> holders;
    template <typename T> T &get();
    auto create_scope();
};

template <typename... Registrations> class Scope {
  public:
    using ContainerType = Container<Registrations...>;

    explicit Scope(ContainerType &container) : container_(container) {}

    template <typename T> T &get();

  private:
    ContainerType &container_;
    std::tuple<Holder<Registrations>...> holders;
};

template <typename... Registrations>
auto Container<Registrations...>::create_scope() {
    return Scope<Registrations...>{*this};
}

template <std::meta::info Param, typename Resolver>
decltype(auto) get_parameter(Resolver &resolver) {
    constexpr auto type =
        std::meta::remove_reference(std::meta::type_of(Param));
    using Dependency = [:type:];
    return resolver.template get<Dependency>();
}
template <typename T, typename Resolver>
auto get_constructor_parameters(Resolver &resolver) {
    constexpr auto constructor = get_constructor(^^T);
    return [:expand(std::meta::parameters_of(
                 constructor)):] >> [&]<auto parameter>() -> decltype(auto) {
        return get_parameter<parameter>(resolver);
    };
}
template <typename T, typename Resolver>
T construct_from_dependencies(Resolver &resolver) {
    auto dependencies = get_constructor_parameters<T>(resolver);
    return std::apply([](auto &...dependencies) { return T{dependencies...}; },
                      dependencies);
}
template <typename... Registrations>
template <typename T>
T &Container<Registrations...>::get() {
    using RegistrationType =
        typename find_registration<^^T, Registrations...>::type;
    if constexpr (RegistrationType::lifetime == Lifetime::Scoped) {
        static_assert(RegistrationType::lifetime == Lifetime::Singleton,
                      "Scoped service must be resolved through a Scope");
    } else {
        using HolderType = Holder<RegistrationType>;
        return std::get<HolderType>(holders).get(*this);
    }
    return *static_cast<T *>(nullptr);
}

template <typename... Registrations>
template <typename T>
T &Scope<Registrations...>::get() {
    using RegistrationType =
        typename find_registration<^^T, Registrations...>::type;
    using HolderType = Holder<RegistrationType>;

    if constexpr (RegistrationType::lifetime == Lifetime::Singleton) {
        return container_.template get<T>();
    } else {
        return std::get<HolderType>(holders).get(*this);
    }
}

template <typename... Bindings, std::meta::info... Types>
consteval auto make_registrations(type_list<Types...>) {
    return registration_types<
        Registration<Types, implementation_for<Types, Bindings...>(),
                     lifetime_for<Types, Bindings...>()>...>{};
}
template <typename... Registrations> struct container_builder {
    using type = Container<Registrations...>;
};

template <std::meta::info Type, std::meta::info... Registered>
consteval bool dependencies_registered(type_list<Registered...>) {
    constexpr auto dependencies = dependency_list<Type>();
    return []<typename... Dependencies>(dependency_types<Dependencies...>,
                                        type_list<Registered...>) {
        return (contains<Dependencies::type_info>(type_list<Registered...>{}) &&
                ...);
    }(dependencies, type_list<Registered...>{});
}
template <std::meta::info... Registered>
consteval bool validate_container(type_list<Registered...>) {
    return (dependencies_registered<Registered>(type_list<Registered...>{}) &&
            ...);
}
template <std::meta::info Type> consteval bool cycle_error() {
    static_assert(Type != Type, "Circular dependency detected");
    return false;
}

template <typename... Dependencies, std::meta::info... Path,
          typename BindingList>
consteval auto collect_each(dependency_types<Dependencies...>,
                            type_list<Path...> path, BindingList bindings);

template <std::meta::info Type, typename BindingList, std::meta::info... Path>
consteval auto collect() {
    if constexpr (contains<Type>(type_list<Path...>{})) {
        cycle_error<Type>();
        return type_list<>{};
    } else {
        constexpr auto implementation =
            graph_implementation<Type, BindingList>::value;

        constexpr auto direct = dependency_list<implementation>();

        return concat_all(
            type_list<Type>{},
            collect_each(direct, type_list<Type, Path...>{}, BindingList{}));
    }
}
template <typename... Dependencies, std::meta::info... Path,
          typename BindingList>
consteval auto collect_each(dependency_types<Dependencies...>,
                            type_list<Path...>, BindingList) {
    if constexpr (sizeof...(Dependencies) == 0)
        return type_list<>{};
    else
        return concat_all(
            collect<Dependencies::type_info, BindingList, Path...>()...);
}
template <typename... Registrations>
consteval auto make_container_type(registration_types<Registrations...>) {
    return ^^Container<Registrations...>;
}

template <std::meta::info Type> consteval auto collect_dependencies() {
    return dependency_list<Type>();
}

template <std::meta::info Type, typename... Registrations>
consteval bool lifetime_dependencies_safe(
    registration_types<Registrations...> registrations) {
    using RegistrationType =
        typename find_registration<Type, Registrations...>::type;

    if constexpr (RegistrationType::lifetime == Lifetime::Scoped) {
        return true;
    } else {
        constexpr auto dependencies =
            dependency_list<RegistrationType::implementation_info>();
        return []<typename... Dependencies>(
                   dependency_types<Dependencies...>,
                   registration_types<Registrations...>) {
            return ((find_registration<Dependencies::type_info,
                                       Registrations...>::type::lifetime ==
                     Lifetime::Singleton) &&
                    ...);
        }(dependencies, registrations);
    }
}

template <typename... Registrations>
consteval bool validate_lifetime_graph(
    registration_types<Registrations...> registrations) {
    return (lifetime_dependencies_safe<Registrations::service_info>(
                registrations) &&
            ...);
}

template <typename Root, typename... Bindings> auto make_container() {
    if constexpr (all_bindings_valid<Bindings...>()) {
        static_assert(validate_bindings<Bindings...>(),
                      "A service may only have one binding");
    } else {
        static_assert(all_bindings_valid<Bindings...>(),
                      "Binding implementation is not convertible to service");
    }

    using BindingList = binding_types<Bindings...>;

    constexpr auto collected = unique(collect<^^Root, BindingList>());

    static_assert(validate_container(collected),
                  "Container has an unregistered dependency");

    constexpr auto registrations = make_registrations<Bindings...>(collected);

    static_assert(validate_lifetime_graph(registrations),
                  "A singleton cannot depend on a scoped service");

    constexpr auto container_type = make_container_type(registrations);

    using App = [:container_type:];

    return App{};
}
} // namespace smol_di::detail
