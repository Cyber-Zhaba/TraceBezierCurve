#ifndef TRACEBEZIERCURVE_BEZIER_H
#define TRACEBEZIERCURVE_BEZIER_H

#include <array>
#include <vector>
#include <cstdint>
#include <cmath>
#include <valarray>

using std::size_t;

struct Point {
    long double x;
    long double y;
};

//TODO find better approach
int64_t factorial(int64_t n) {
    if (n < 0) {
        return 0;
    }
    uint64_t result = 1;
    for (uint64_t i = 1; i <= n; ++i) {
        result *= i;
    }
    return static_cast<int64_t>(result);
}

//TODO find better approach
int64_t binom(int64_t n, int64_t k) {
    return factorial(n) / factorial(k) / factorial(n - k);
}

long double z(long double t, long double Point::* member, const std::vector<Point>& points) {
    long double sum = 0;
    size_t number_of_points = points.size();
    for (size_t k = 0; k < number_of_points; ++k) {
        sum += points[k].*member * binom(number_of_points - 1, k) * std::pow(t, k) * std::pow(1 - t, number_of_points - 1 - k);
    }
    return sum;
}

Point B(long double t, const std::vector<Point>& points) {
    return {
        z(t, &Point::x, points),
        z(t, &Point::y, points)
    };
}

void get_curve(const std::vector<Point>& points, std::vector<Point>& curve, uint64_t num_points = 100) {
    for (uint64_t i = 0; i < num_points; ++i) {
        long double t = (long double) i / (num_points - 1);
        curve.push_back(B(t, points));
    }
}

#endif //TRACEBEZIERCURVE_BEZIER_H
