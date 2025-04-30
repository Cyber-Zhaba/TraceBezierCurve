#include <iostream>
#include "bezier.h"

int main() {
    freopen("../out.txt", "w", stdout);
    std::vector<Point> points = {
            {0, 0},
            {1, 4},
            {2, 1},
            {3, 3},
            {4, 2},
            {3, 1}
    };
    std::vector<Point> curve;
    get_curve(points, curve);
    for (auto& e : curve) {
        std::cout << e.x << ' ' << e.y << '\n';
    }
}
