#include <iostream>
#include <chrono>
#include <cstdlib>
#include <string>
#include <numeric>
#include <vector>
#include <sstream>

int main(int argc, char* argv[]) {
    if (argc < 6) {
        std::cerr << "Usage: " << argv[0] << " <executable_path> <iterations> <x> <y> <curve_points>\n";
        return 1;
    }

    std::string executable = argv[1];
    int iterations = std::stoi(argv[2]);
    double x = std::strtod(argv[3], nullptr);
    double y = std::strtod(argv[4], nullptr);
    uint64_t curve_points = std::strtoull(argv[5], nullptr, 10);

    std::vector<double> durations;

    for (int i = 0; i < iterations; ++i) {
        std::ostringstream cmd;
        cmd << executable << " " << x << " " << y << " " << curve_points << " > /dev/null 2>&1";
        std::string command = cmd.str();

        auto start = std::chrono::high_resolution_clock::now();
        int result = std::system(command.c_str());
        auto end = std::chrono::high_resolution_clock::now();

        if (result != 0) {
            std::cerr << "Execution failed at iteration " << i << "\n";
            return 1;
        }

        std::chrono::duration<double> elapsed = end - start;
        durations.push_back(elapsed.count());
    }

    double total_time = std::accumulate(durations.begin(), durations.end(), 0.0);
    double average_time = total_time / iterations;

    std::cout << "Total time for " << iterations << " runs: " << total_time << " seconds\n";
    std::cout << "Average time per run: " << average_time << " seconds\n";

    return 0;
}