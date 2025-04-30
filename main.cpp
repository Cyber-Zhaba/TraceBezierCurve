#include <iostream>
#include "bezier.h"
#include <string>
#include <cassert>

using std::string;

void write2file(string filename) {
    freopen(filename.data(), "w", stdout);

    Point start(0, 0);
    Point end(5, 5.5);
    int mid_points = 5;
    int curve_points = 1000;

    assert(mid_points <= MaxPoints);
    assert(curve_points <= MaxCurvePoints);

    std::array<Point, MaxPoints> points{};
    generate_random_dots(start, end, mid_points, points, 0.2, 0.1);
    std::array<Point, MaxCurvePoints> curve{};
    get_curve(points, curve, mid_points, curve_points);
    for (auto& e : curve) {
        std::cout << e.x << ' ' << e.y << '\n';
    }
}

int main() {
    write2file("../out");
}
