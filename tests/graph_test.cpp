#include "smol_di/smol_di.hpp"

#include <gtest/gtest.h>

struct RepositoryConfig {};
struct IRepository {
    virtual ~IRepository() = default;
};
struct Repository : IRepository {
    explicit Repository(RepositoryConfig &) {}
};

using RepositoryBinding = smol_di::Binding<IRepository, Repository>;
using Bindings = smol_di::detail::binding_types<RepositoryBinding>;
constexpr auto graph = smol_di::detail::unique(
    smol_di::detail::collect<^^IRepository, Bindings>());
static_assert(smol_di::detail::contains<^^RepositoryConfig>(graph));

TEST(GraphTest, FollowsBoundImplementationDependenciesAtCompileTime) {
    SUCCEED();
}
