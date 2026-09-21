#include "smol_di/smol_di.hpp"

struct MissingDependency {};
struct NeedsMissingDependency {
    explicit NeedsMissingDependency(MissingDependency &) {}
};

int main() {
    auto app = smol_di::create_container<NeedsMissingDependency>();
    (void)app;
}
