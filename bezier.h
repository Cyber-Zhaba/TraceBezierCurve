#ifndef TRACE_BEZIER_CURVE_BEZIER_H
#define TRACE_BEZIER_CURVE_BEZIER_H

// === Includes ===

#include <array>
#include <vector>
#include <cstdint>
#include <cmath>
#include <random>
#include <optional>
#include <unordered_map>
#include <memory_resource>

// === Point2D struct ===

struct Point {
    long double x;
    long double y;

    Point(): x(0.0), y(0.0) {}
    Point(long double x, long double y): x(x), y(y) {}

    Point& operator+=(const Point& other) { x += other.x; y += other.y; return *this; }
    Point& operator-=(const Point& other) { x -= other.x; y -= other.y; return *this; }
    Point& operator*=(long double lambda) { x *= lambda; y *= lambda; return *this; }
    Point& operator/=(long double lambda) { x /= lambda; y /= lambda; return *this; }

    Point operator+(const Point& other) const { return Point(*this) += other; }
    Point operator-(const Point& other) const { return Point(*this) -= other; }
    Point operator*(long double lambda) const { return Point(*this) *= lambda; }
    Point operator/(long double lambda) const { return Point(*this) /= lambda; }

    bool operator==(const Point& other) const { return x == other.x and y == other.y; }
    bool operator!=(const Point& other) const { return !(*this == other); }
};

// Print point
inline std::ostream& operator<<(std::ostream& of, const Point& p) noexcept {
    of << p.x << ' ' << p.y;
    return of;
}

// === Bezier class ===
template<size_t CurvePoints=50, size_t MidPoints=5>
class Bezier {
    size_t offset_idx{0};
    long double aspect;                         // Ration between distance from start to end and beta spread
    long double alpha_jitter;                   // Funny word
    Point current{0.0, 0.0};
    Point target{0.0, 0.0};
    std::array<Point, MidPoints + 2> points{};  // array of mid points

public:
    // === Constructor ===
    explicit Bezier(long double aspect=0.2, long double alpha_jitter=0.1)
        : aspect(aspect),
          alpha_jitter(alpha_jitter) {
        // Assume that user is smart enough to not use more points than allowed
        // Otherwise, you should uncomment assertions
        // assert(CurvePoints >= 1);
        // assert(MidPoints >= 0);
    }

    // Generate next point on
    std::pair<double, double> GetNextPoint() noexcept {
        if (offset_idx + 1 == CurvePoints) {
            current = target;
            return {target.x, current.y};
        }
        long double t = static_cast<long double>(++offset_idx) / CurvePoints;
        current = B(t);
        return {current.x, current.y};
    }

    // Change target Point
    void UpdateTarget(const long double x, const long double y) noexcept {
        Point new_target(x, y);
        if (target == new_target) {
            return;
        }
        points[1] = target;
        target = new_target;
        offset_idx = 0;
        GRMP();
    }

    [[nodiscard]] size_t GetOffsetIdx() const noexcept {
        return offset_idx;
    }

private:
    long double dist(Point a, Point b) {
        return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    }

    // Calculate binomial coefficient with memoization
    int64_t binom(int64_t n, int64_t k) {
        if (k > n / 2) k = n - k;

        auto& cache = BinomCache::instance();
        if (auto val = cache.get(n, k))
            return *val;

        uint64_t result = 1;
        for (int64_t i = 1; i <= k; ++i) {
            result *= n - (k - i);
            result /= i;
        }

        auto final = static_cast<int64_t>(result);
        cache.put(n, k, final);
        return final;
    }

    // Fast pow
    long double pow(long double base, int64_t exp) {
        if (base == 0) return 0;
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
        thread_local std::mt19937 gen(std::random_device{}());
        thread_local std::uniform_real_distribution dist(0.0, 1.0);
        return dist(gen);
    }

    // Magic happens here
    long double z(long double t, long double Point::* member) {
        long double t_pow = 1.0;
        long double one_minus_t = 1.0 - t;
        long double one_minus_t_pow = pow(one_minus_t, MidPoints + 1);

        long double sum = 0;
        for (std::size_t k = 0; k < MidPoints + 2; ++k) {
            long double coeff = binom(MidPoints + 1, k);
            sum += points[k].*member * coeff * t_pow * one_minus_t_pow;
            t_pow *= t;
            if (k + 1 < MidPoints + 2)
                one_minus_t_pow /= one_minus_t;
        }
        return sum;
    }

    void GRMP() {
        long double length = dist(current, target);

        Point d = (target - current) / length;
        Point n(- d.y, d.x);

        long double spread = length * aspect;
        long double alpha_spray = (long double) 1 / (MidPoints + 1);

        points[0] = current;
        for (std::size_t i = 1; i < MidPoints; ++i) {
            long double alpha_noise = random_0_1() * alpha_jitter * (2 * alpha_spray) - alpha_spray * alpha_jitter;
            long double alpha = (long double) i / MidPoints + alpha_noise;
            long double beta = random_0_1() * (2 * spread) - spread;

            points[i + 1] = d * alpha * length + current + n * beta;
        }
        points[MidPoints + 1] = target;
    }

    // Bezier function by parameter t
    Point B(long double t) {
        return {
            z(t, &Point::x),
            z(t, &Point::y)
        };
    }

    // Singleton class for binomial coefficient cache
    class BinomCache {
        static constexpr std::size_t MaxLUTSize = MidPoints + 1;
        std::array<std::array<std::optional<int64_t>, MaxLUTSize>, MaxLUTSize> lut_{};

        BinomCache() = default;

    public:
        static BinomCache& instance() {
            static BinomCache cache;
            return cache;
        }

        std::optional<int64_t> get(int64_t n, int64_t k) {
            if (n < MaxLUTSize && k < MaxLUTSize) {
                return lut_[n][k];
            }
            return std::nullopt;
        }

        void put(int64_t n, int64_t k, int64_t value) {
            if (n < MaxLUTSize && k < MaxLUTSize) {
                lut_[n][k] = value;
            }
        }
    };
};

#endif //TRACE_BEZIER_CURVE_BEZIER_H
