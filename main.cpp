#include <iostream>
#include "bezier.h"
#include <string>

using std::string;

void write2file(string filename) {
    freopen(filename.data(), "w", stdout);
    Point start(0, 0);
    Point end(5, 5.5);

    std::vector<Point> points;
    generate_random_dots(start, end, 100, points, 0.2, 0.1);
    std::vector<Point> curve;
    get_curve(points, curve, 1000);
    for (auto& e : curve) {
        std::cout << e.x << ' ' << e.y << '\n';
    }
}

int main() {
    write2file("../out");
}
