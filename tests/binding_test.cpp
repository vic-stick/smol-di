#include "smol_di/smol_di.hpp"

#include <gtest/gtest.h>
#include <type_traits>

struct IUserRepository {
    virtual ~IUserRepository() = default;
};
struct UserRepository : IUserRepository {
    UserRepository() {}
};

struct IFoo {
    virtual ~IFoo() = default;
};
struct Foo : IFoo {};
struct IBar {
    virtual ~IBar() = default;
};
struct Bar : IBar {};
struct Database {};

using RepositoryBinding = smol_di::Binding<IUserRepository, UserRepository>;

constexpr auto implementation =
    smol_di::detail::implementation_for<^^IUserRepository, RepositoryBinding>();
static_assert(implementation == ^^UserRepository);

using FooBinding = smol_di::Binding<IFoo, Foo>;
using BarBinding = smol_di::Binding<IBar, Bar>;

static_assert(smol_di::contains_binding<^^IFoo, FooBinding, BarBinding>());
static_assert(smol_di::contains_binding<^^IBar, FooBinding, BarBinding>());
static_assert(!smol_di::contains_binding<^^Database, FooBinding, BarBinding>());
static_assert(smol_di::validate_bindings<FooBinding, BarBinding>());

using DuplicateFooBinding = smol_di::Binding<IFoo, Foo>;
static_assert(!smol_di::validate_bindings<FooBinding, DuplicateFooBinding>());

using ValidBinding = smol_di::Binding<IUserRepository, UserRepository>;
using InvalidBinding = smol_di::Binding<IUserRepository, Database>;

static_assert(RepositoryBinding::service_info == ^^IUserRepository);
static_assert(RepositoryBinding::implementation_info == ^^UserRepository);

static_assert(std::is_same_v<
              decltype(smol_di::bind<IUserRepository, UserRepository>),
              const smol_di::Binding<IUserRepository, UserRepository>>);

using BoundRepository = std::remove_cv_t<decltype(
    smol_di::bind<IUserRepository, UserRepository>)>;
static_assert(BoundRepository::service_info == ^^IUserRepository);
static_assert(BoundRepository::implementation_info == ^^UserRepository);

using ScopedRepository =
    std::remove_cv_t<decltype(smol_di::scoped<IUserRepository>)>;
static_assert(ScopedRepository::service_info == ^^IUserRepository);
static_assert(ScopedRepository::lifetime == smol_di::Lifetime::Scoped);

using SingletonDatabase =
    std::remove_cv_t<decltype(smol_di::singleton<Database>)>;
static_assert(SingletonDatabase::service_info == ^^Database);
static_assert(SingletonDatabase::lifetime == smol_di::Lifetime::Singleton);

static_assert(smol_di::is_binding_v<RepositoryBinding>);
static_assert(smol_di::is_lifetime_config_v<ScopedRepository>);

using RepoLifetime =
    smol_di::LifetimeConfig<IUserRepository, smol_di::Lifetime::Scoped>;
static_assert(smol_di::lifetime_for<^^IUserRepository, RepoLifetime>() ==
              smol_di::Lifetime::Scoped);
static_assert(smol_di::lifetime_for<^^Database, RepoLifetime>() ==
              smol_di::Lifetime::Singleton);
static_assert(
    smol_di::lifetime_for<^^IUserRepository, RepositoryBinding, RepoLifetime>() ==
    smol_di::Lifetime::Scoped);

constexpr auto generated_registrations =
    smol_di::detail::make_registrations<RepositoryBinding, RepoLifetime>(
        smol_di::detail::type_list<^^IUserRepository, ^^Database>{});

template <std::meta::info Service, typename RegistrationPack>
struct generated_registration;

template <std::meta::info Service, typename... Registrations>
struct generated_registration<
    Service, smol_di::detail::registration_types<Registrations...>>
    : smol_di::detail::find_registration<Service, Registrations...> {};

using GeneratedRegistrationPack =
    std::remove_cv_t<decltype(generated_registrations)>;
using GeneratedRepositoryRegistration =
    typename generated_registration<^^IUserRepository,
                                   GeneratedRegistrationPack>::type;
using GeneratedDatabaseRegistration =
    typename generated_registration<^^Database, GeneratedRegistrationPack>::type;

static_assert(GeneratedRepositoryRegistration::implementation_info ==
              ^^UserRepository);
static_assert(GeneratedRepositoryRegistration::lifetime ==
              smol_di::Lifetime::Scoped);
static_assert(GeneratedDatabaseRegistration::implementation_info == ^^Database);
static_assert(GeneratedDatabaseRegistration::lifetime ==
              smol_di::Lifetime::Singleton);

static_assert(smol_di::valid_binding<ValidBinding>());
static_assert(!smol_di::valid_binding<InvalidBinding>());

TEST(BindingTest, ResolvesImplementationAtCompileTime) { SUCCEED(); }
