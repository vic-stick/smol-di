#include "smol_di/smol_di.hpp"

#include <gtest/gtest.h>

struct SingletonService {
    SingletonService() {}
};

struct ScopedService {
    explicit ScopedService(SingletonService &singleton) : singleton(singleton) {}

    SingletonService &singleton;
};

int scoped_alive = 0;

struct TrackedScopedService {
    TrackedScopedService() { ++scoped_alive; }
    TrackedScopedService(TrackedScopedService &&) { ++scoped_alive; }
    ~TrackedScopedService() { --scoped_alive; }
};

TEST(LifetimeTest, ReturnsTheSameSingletonInstance) {
    auto app = smol_di::create_container<SingletonService>();

    auto &first = app.get<SingletonService>();
    auto &second = app.get<SingletonService>();

    EXPECT_EQ(&first, &second);
}

TEST(LifetimeTest, CreatesAScopeShell) {
    auto container = smol_di::create_container<SingletonService>();
    auto scope = container.create_scope();

    (void)scope;
}

TEST(LifetimeTest, ReusesScopedInstancesWithinOneScope) {
    auto container = smol_di::create_container<ScopedService>(
        smol_di::scoped<ScopedService>);
    auto scope = container.create_scope();

    auto &first = scope.get<ScopedService>();
    auto &second = scope.get<ScopedService>();

    EXPECT_EQ(&first, &second);
}

TEST(LifetimeTest, SeparatesScopedInstancesAcrossScopes) {
    auto container = smol_di::create_container<ScopedService>(
        smol_di::scoped<ScopedService>);
    auto first_scope = container.create_scope();
    auto second_scope = container.create_scope();

    auto &first = first_scope.get<ScopedService>();
    auto &second = second_scope.get<ScopedService>();

    EXPECT_NE(&first, &second);
    EXPECT_EQ(&first.singleton, &second.singleton);
}

TEST(LifetimeTest, DestroysScopedInstancesWithTheirScope) {
    scoped_alive = 0;
    auto container = smol_di::create_container<TrackedScopedService>(
        smol_di::scoped<TrackedScopedService>);

    {
        auto scope = container.create_scope();
        (void)scope.get<TrackedScopedService>();
        EXPECT_EQ(scoped_alive, 1);
    }

    EXPECT_EQ(scoped_alive, 0);
}
