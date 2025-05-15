// Example of usage

#include <iostream>
#include <sstream>

#include "bezier.h"

void print_points4plot() {
    freopen("../tar.txt", "w", stdout);
    Bezier<10, 4> b;

    std::ostringstream string_cur_out;

    for (int frame = 0; frame < 2000; ++frame) {
        double t = (double) frame * 0.05;
        double target_x = sin(t + std::numbers::pi / 2);
        double target_y = sin(5 * t);

        b.UpdateTarget({target_x, target_y});

        for (int i = 0; i < 10; ++i) {
            std::cout << target_x << " " << target_y << "\n";
            auto p = b.GetNextPoint();
            string_cur_out << p.x << " " << p.y << "\n";
        }
    }

    freopen("../cur.txt", "w", stdout);
    std::cout << string_cur_out.str();
}

int main() {
    Bezier b;

    // --- 1 ---
    b.UpdateTarget({15.0, 167});
    for (int i = 0; i < 30; ++i) {
        std::cout << b.GetNextPoint() << '\n';
    }

    std::cout << "---\n";

    // --- 2 ---
    b.UpdateTarget({584, -209});
    for (int i = 0; i < 45; ++i) {
        std::cout << b.GetNextPoint() << '\n';
    }

    std::cout << "---\n";

    // --- 3 ---
    b.UpdateTarget({944, 181});
    for (int i = 0; i < 10; ++i) {
        std::cout << b.GetNextPoint() << '\n';
    }

    std::cout << "---\n";

    // --- 4 ---
    b.UpdateTarget({422, -442});
    for (int i = 0; i < 50; ++i) {
        std::cout << b.GetNextPoint() << '\n';
    }

    std::cout << "---\n";

    // --- 5 ---
    b.UpdateTarget({-34, -145});
    for (int i = 0; i < 50; ++i) {
        std::cout << b.GetNextPoint() << '\n';
    }
}
