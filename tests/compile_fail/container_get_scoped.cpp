#include "smol_di/smol_di.hpp"

struct RequestContext {};

int main() {
    auto app = smol_di::create_container<RequestContext>(
        smol_di::scoped<RequestContext>);
    auto &context = app.get<RequestContext>();
    (void)context;
}
