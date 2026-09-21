#include "smol_di/smol_di.hpp"

struct CircularB;
struct CircularA { explicit CircularA(CircularB &) {} };
struct CircularB { explicit CircularB(CircularA &) {} };

int main() {
    auto app = smol_di::create_container<CircularA>();
    (void)app;
}
