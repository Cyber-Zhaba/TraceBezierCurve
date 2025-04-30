#include <iostream>
#include <chrono>
#include <cstdlib>
#include <string>
#include <numeric>
#include <vector>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <executable_path> <iterations>\n";
        return 1;
    }

    std::string cmd = argv[1];
    int iterations = std::stoi(argv[2]);

    std::vector<double> durations;

    for (int i = 0; i < iterations; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        int result = std::system(cmd.c_str());
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
