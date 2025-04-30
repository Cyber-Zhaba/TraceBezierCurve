#ifndef TRACEBEZIERCURVE_BEZIER_H
#define TRACEBEZIERCURVE_BEZIER_H

// === Includes ===

#include <array>
#include <vector>
#include <cstdint>
#include <cmath>
#include <random>
#include <algorithm>
#include <optional>
#include <unordered_map>
#include <memory_resource>

// === Max buffer size for arrays located on stack ===

constexpr std::size_t BinomCacheBufferSize = 1024;
constexpr std::size_t MaxPoints = 128;
constexpr std::size_t MaxCurvePoints = 1024;

// === Point2D struct ===

struct Point {
    long double x;
    long double y;

    Point& operator+=(const Point& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Point& operator-=(const Point& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Point& operator*=(long double lambda) {
        x *= lambda;
        y *= lambda;
        return *this;
    }

    Point& operator/=(long double lambda) {
        x /= lambda;
        y /= lambda;
        return *this;
    }

    Point operator+(const Point& other) const {
        Point temp = *this;
        temp += other;
        return temp;
    }

    Point operator-(const Point& other) const {
        Point temp = *this;
        temp -= other;
        return temp;
    }

    Point operator*(long double lambda) const {
        Point temp = *this;
        temp *= lambda;
        return temp;
    }

    Point operator/(long double lambda) const {
        Point temp = *this;
        temp /= lambda;
        return temp;
    }
};

// === Sub functions ===

// Hash function for std::pair
namespace std {
    template<>
    struct hash<std::pair<int64_t, int64_t>> {
        std::size_t operator()(const std::pair<int64_t, int64_t>& p) const noexcept {
            std::size_t h1 = std::hash<int64_t>{}(p.first);
            std::size_t h2 = std::hash<int64_t>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };
}

//TODO find better approach
long double dist(Point a, Point b) {
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

// Binomial coefficient with caching
class BinomCache {
    static constexpr std::size_t MaxLUTSize = MaxPoints + 1;

    std::array<std::array<std::optional<int64_t>, MaxLUTSize>, MaxLUTSize> lut_{};

    using Key = std::pair<int64_t, int64_t>;
    alignas(std::max_align_t) std::byte buffer_[BinomCacheBufferSize];
    std::pmr::monotonic_buffer_resource resource_{buffer_, BinomCacheBufferSize};
    std::pmr::unordered_map<Key, int64_t> map_{&resource_};

    BinomCache() = default;

public:
    static BinomCache& instance() {
        static BinomCache cache;
        return cache;
    }

    std::optional<int64_t> get(int64_t n, int64_t k) {
        if (n < MaxLUTSize && k < MaxLUTSize) {
            return lut_[n][k];
        } else {
            auto it = map_.find({n, k});
            if (it != map_.end()) return it->second;
            return std::nullopt;
        }
    }

    void put(int64_t n, int64_t k, int64_t value) {
        if (n < MaxLUTSize && k < MaxLUTSize) {
            lut_[n][k] = value;
        } else {
            map_[{n, k}] = value;
        }
    }
};

int64_t binom(int64_t n, int64_t k) {
    if (k > n / 2) k = n - k;

    auto& cache = BinomCache::instance();
    if (auto val = cache.get(n, k))
        return *val;

    uint64_t result = 1;
    for (int64_t i = 1; i <= k; ++i) {
        result *= (n - (k - i));
        result /= i;
    }

    auto final = static_cast<int64_t>(result);
    cache.put(n, k, final);
    return final;
}

// Fast pow
long double pow(long double base, int64_t exp) {
    long double result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result *= base;
        }
        base *= base;
        exp /= 2;
    }
    return result;
}

//TODO find better approach with std::minstd_rand
long double random_0_1() {
    static thread_local std::mt19937 gen(std::random_device{}());
    static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

// Calculation of function B of one coordinate
long double z(long double t, long double Point::* member, const std::array<Point, MaxPoints>& points, std::size_t number_of_points) {
    long double t_pow = 1.0;
    long double one_minus_t = 1.0 - t;
    long double one_minus_t_pow = pow(one_minus_t, number_of_points - 1);

    long double sum = 0;
    for (std::size_t k = 0; k < number_of_points; ++k) {
        long double coeff = binom(number_of_points - 1, k);
        sum += points[k].*member * coeff * t_pow * one_minus_t_pow;
        t_pow *= t;
        if (k + 1 < number_of_points)
            one_minus_t_pow /= one_minus_t;
    }
    return sum;
}

// Function B for calculating the point on the curve according to the parameter t
Point B(long double t, const std::array<Point, MaxPoints>& points, std::size_t number_of_points) {
    return {
        z(t, &Point::x, points, number_of_points + 2),
        z(t, &Point::y, points, number_of_points + 2)
    };
}

// Get a discrete curve
void get_curve(
        const std::array<Point,
        MaxPoints>& points,
        std::array<Point, MaxCurvePoints>& curve,
        std::size_t number_of_mid_points,
        uint64_t curve_points = 100
                ) {
    for (uint64_t i = 0; i < curve_points; ++i) {
        long double t = (long double) i / (curve_points - 1);
        curve[i] = B(t, points, number_of_mid_points);
    }
}

// Generate random points for Bezier curve
void generate_random_dots(
        Point start,
        Point end,
        std::size_t num_mid_points,
        std::array<Point, MaxPoints>& points,
        long double aspect = 1.0,
        long double alpha_jitter = 0.1
                ) {
    long double length = dist(start, end);

    Point d = (end - start) / length;
    Point n(- d.y, d.x);

    long double spread = length * aspect;
    long double alpha_spray = (long double) 1 / (num_mid_points + 1);

    points[0] = start;
    for (std::size_t i = 0; i < num_mid_points; ++i) {
        long double alpha_noise = random_0_1() * alpha_jitter * (2 * alpha_spray) - alpha_spray * alpha_jitter;
        long double alpha = (long double) (i + 1) / (num_mid_points + 1) + alpha_noise;
        long double beta = random_0_1() * (2 * spread) - spread;

        points[i + 1] = d * alpha * length + start + n * beta;
    }
    points[num_mid_points + 1] = end;
}

#endif //TRACEBEZIERCURVE_BEZIER_H
