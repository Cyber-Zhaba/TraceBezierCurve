#include <iostream>
#include "bezier.h"

int main() {
    freopen("../out.txt", "w", stdout);
    Point start(0, 0);
    Point end(5, 5.5);

    std::vector<Point> points;
    generate_random_dots(start, end, 5, points, 0.2, 0.1);
    std::vector<Point> curve;
    get_curve(points, curve);
    for (auto& e : curve) {
        std::cout << e.x << ' ' << e.y << '\n';
    }
}
