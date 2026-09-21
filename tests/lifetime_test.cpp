#include "smol_di/smol_di.hpp"

#include <gtest/gtest.h>

struct SingletonService {
    SingletonService() {}
};

TEST(LifetimeTest, ReturnsTheSameSingletonInstance) {
    auto app = smol_di::create_container<SingletonService>();

    auto &first = app.get<SingletonService>();
    auto &second = app.get<SingletonService>();

    EXPECT_EQ(&first, &second);
}
