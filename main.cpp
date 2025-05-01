// Example of usage

#include <iostream>
#include "bezier.h"

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
