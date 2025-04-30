#include <iostream>
#include "bezier.h"
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]) {
    // Arg check
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <x> <y> <curve_points>" << std::endl;
        return 1;
    }

    // Arg parse
    double x = std::strtod(argv[1], nullptr);
    double y = std::strtod(argv[2], nullptr);
    uint64_t curve_points = std::strtoull(argv[3], nullptr, 10);

    // Generate Bezier curve
    std::array<Point, Bezier::MaxCurvePoints> curve{};
    Bezier::Trace({x, y}, curve_points, curve);

    // Print the curve
    bool first_print = false;
    for (auto& [x, y] : curve) {
        if (x == 0 && y == 0 && first_print) {
            break;
        }
        std::cout << x << ' ' << y << '\n';
        first_print = true;
    }
}
