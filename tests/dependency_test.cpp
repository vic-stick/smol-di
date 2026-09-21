#include "smol_di/smol_di.hpp"

#include <gtest/gtest.h>

struct Database {};
struct NeedsDatabase {
    explicit NeedsDatabase(Database &) {}
};

constexpr auto dependencies =
    smol_di::detail::dependency_list<^^NeedsDatabase>();
static_assert(smol_di::detail::dependencies_registered<^^NeedsDatabase>(
    smol_di::detail::type_list<^^Database>{}));

TEST(DependencyTest, DiscoversConstructorDependenciesAtCompileTime) {
    SUCCEED();
}
