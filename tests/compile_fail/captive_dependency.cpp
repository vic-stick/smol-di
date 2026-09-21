#include "smol_di/smol_di.hpp"

struct RequestContext {};
struct SingletonService {
    explicit SingletonService(RequestContext &) {}
};

int main() {
    auto app = smol_di::create_container<SingletonService>(
        smol_di::scoped<RequestContext>);
    (void)app;
}
