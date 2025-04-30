#include <iostream>
#include "bezier.h"
#include <string>

using std::string;

void write2file(string filename) {
    freopen(filename.data(), "w", stdout);

    std::array<Point, Bezier::MaxCurvePoints> curve{};
    Bezier::Trace({5.0, 5.5}, 100, curve);

    for (auto& e : curve) {
        std::cout << e.x << ' ' << e.y << '\n';
    }
}

int main() {
    write2file("../out");
}
