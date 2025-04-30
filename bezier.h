#ifndef TRACEBEZIERCURVE_BEZIER_H
#define TRACEBEZIERCURVE_BEZIER_H

// === LIBS ===

#include <array>
#include <vector>
#include <cstdint>
#include <cmath>
#include <random>
#include <algorithm>
#include <optional>
#include <unordered_map>
#include <memory_resource>

// === Point struct ===

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

class BinomCache {
    static constexpr std::size_t BufferSize = 1024 * 90;

    using Key = std::pair<int64_t, int64_t>;

    alignas(std::max_align_t) std::byte buffer_[BufferSize];
    std::pmr::monotonic_buffer_resource resource_{buffer_, BufferSize};
    std::pmr::unordered_map<Key, int64_t> map_{&resource_};

    BinomCache() = default;

public:
    static BinomCache& instance() {
        static BinomCache cache;
        return cache;
    }

    std::optional<int64_t> get(int64_t n, int64_t k) {
        auto it = map_.find({n, k});
        if (it != map_.end()) return it->second;
        return std::nullopt;
    }

    void put(int64_t n, int64_t k, int64_t value) {
        map_[{n, k}] = value;
    }
};

int64_t binom(int64_t n, int64_t k) {
    auto& cache = BinomCache::instance();
    if (auto val = cache.get(n, k))
        return *val;

    uint64_t result = 1;
    for (uint64_t i = k + 1; i <= n; ++i) {
        result *= i;
    }
    for (uint64_t i = 1; i <= n - k; ++i) {
        result /= i;
    }

    auto final = static_cast<int64_t>(result);
    cache.put(n, k, final);
    return final;
}

//TODO find better approach
long double random_0_1() {
    static thread_local std::mt19937 gen(std::random_device{}()); // Быстрый генератор (thread-local!)
    static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(gen);
}

long double z(long double t, long double Point::* member, const std::vector<Point>& points) {
    long double sum = 0;
    std::size_t number_of_points = points.size();
    for (std::size_t k = 0; k < number_of_points; ++k) {
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

void generate_random_dots(Point start, Point end, std::size_t num_mid_points, std::vector<Point>& result, long double aspect = 1.0, long double alpha_jitter = 0.1) {
    long double length = dist(start, end);

    Point d = (end - start) / length;
    Point n(- d.y, d.x);

    long double spread = length * aspect;
    long double alpha_spray = (long double) 1 / (num_mid_points + 1);

    result.push_back(start);
    for (std::size_t i = 0; i < num_mid_points; ++i) {
        long double alpha_noise = random_0_1() * alpha_jitter * (2 * alpha_spray) - alpha_spray * alpha_jitter;
        long double alpha = (long double) (i + 1) / (num_mid_points + 1) + alpha_noise;
        long double beta = random_0_1() * (2 * spread) - spread;

        result.push_back(d * alpha * length + start + n * beta);
    }
    result.push_back(end);
}

#endif //TRACEBEZIERCURVE_BEZIER_H
