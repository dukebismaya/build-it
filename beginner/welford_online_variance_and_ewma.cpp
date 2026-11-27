/*
 * ============================================================
 *     Welford's Online Variance, Parallel Merge & EWMA Stats
 * ============================================================
 *
 * Description:
 *   Computing variance via the naive textbook formula
 *   E[X^2] - (E[X])^2 suffers from catastrophic cancellation when
 *   the standard deviation is small compared to the mean (e.g.,
 *   timestamps or high-offset sensor readings) and requires two
 *   passes or storing all samples.
 *
 *   1. Welford's Algorithm (1962) updates count n, mean mu, and
 *      sum of squared deviations M2 in a single streaming pass:
 *        delta  = x - mean
 *        mean  += delta / n
 *        delta2 = x - mean
 *        M2    += delta * delta2
 *      Sample variance = M2 / (n - 1), population variance = M2 / n.
 *
 *   2. Chan, Golub & LeVeque (1979) Parallel Batch Merge combines
 *      two Welford accumulators (n_A, mu_A, M2_A) and (n_B, mu_B, M2_B)
 *      in O(1) time for MapReduce / multi-threaded aggregation:
 *        delta = mu_B - mu_A
 *        M2    = M2_A + M2_B + delta^2 * n_A * n_B / (n_A + n_B)
 *
 *   3. Exponentially Weighted Moving Average (EWMA) & EWMA Variance
 *      track non-stationary latency / RTT streams (like TCP SRTT).
 *
 * Complexity:
 *   Time:  O(1) per sample or shard merge; Space: O(1).
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <cstdint>
#include <iomanip>

namespace streaming_stats {

    class WelfordAccumulator {
        uint64_t n_ = 0;
        double mean_ = 0.0;
        double m2_ = 0.0;

    public:
        void update(double x) {
            ++n_;
            double delta = x - mean_;
            mean_ += delta / static_cast<double>(n_);
            double delta2 = x - mean_;
            m2_ += delta * delta2;
        }

        uint64_t count() const { return n_; }
        double mean() const { return mean_; }
        double population_variance() const { return n_ ? m2_ / static_cast<double>(n_) : 0.0; }
        double sample_variance() const { return n_ > 1 ? m2_ / static_cast<double>(n_ - 1) : 0.0; }
        double sample_stddev() const { return std::sqrt(sample_variance()); }

        // Chan's parallel merge of two independent partitions
        static WelfordAccumulator merge(const WelfordAccumulator& a, const WelfordAccumulator& b) {
            if (a.n_ == 0) return b;
            if (b.n_ == 0) return a;
            WelfordAccumulator out;
            out.n_ = a.n_ + b.n_;
            double delta = b.mean_ - a.mean_;
            out.mean_ = a.mean_ + delta * (static_cast<double>(b.n_) / static_cast<double>(out.n_));
            out.m2_ = a.m2_ + b.m2_ + delta * delta * (static_cast<double>(a.n_) * static_cast<double>(b.n_)
                                                       / static_cast<double>(out.n_));
            return out;
        }
    };

    class EWMAEstimator {
        double alpha_;
        bool initialized_ = false;
        double mean_ = 0.0;
        double var_ = 0.0;

    public:
        explicit EWMAEstimator(double alpha) : alpha_(alpha) {}

        void update(double x) {
            if (!initialized_) {
                mean_ = x;
                var_ = 0.0;
                initialized_ = true;
                return;
            }
            double diff = x - mean_;
            double incr = alpha_ * diff;
            mean_ += incr;
            var_ = (1.0 - alpha_) * (var_ + diff * incr);
        }

        double mean() const { return mean_; }
        double stddev() const { return std::sqrt(var_); }
    };

} // namespace streaming_stats

int main() {
    using namespace streaming_stats;
    std::cout << "============================================================\n";
    std::cout << "     Welford's Online Variance, Parallel Merge & EWMA Stats\n";
    std::cout << "============================================================\n\n";

    // High-offset dataset: 10^9 + {4, 7, 13, 16} where exact sample variance is 30.0
    const double base = 1e9;
    std::vector<double> offsets = {4.0, 7.0, 13.0, 16.0};
    WelfordAccumulator w;
    double sum = 0.0, sum_sq = 0.0;
    for (double d : offsets) {
        double x = base + d;
        w.update(x);
        sum += x;
        sum_sq += x * x;
    }
    double n = static_cast<double>(offsets.size());
    double naive_var = (sum_sq - (sum * sum) / n) / (n - 1.0);

    std::cout << "Catastrophic cancellation test on 10^9 + {4, 7, 13, 16} (exact variance = 30):\n";
    std::cout << "  Naive E[X^2]-E[X]^2 sample variance : " << std::fixed << std::setprecision(6) << naive_var << "\n";
    std::cout << "  Welford online sample variance      : " << w.sample_variance() << "\n";

    // Test parallel Chan merge across 4 shards against single-stream Welford
    WelfordAccumulator full, shards[4];
    for (int i = 0; i < 4000; ++i) {
        double v = 1e8 + std::sin(i * 0.1) * 50.0 + (i % 17);
        full.update(v);
        shards[i % 4].update(v);
    }
    auto merged = WelfordAccumulator::merge(
        WelfordAccumulator::merge(shards[0], shards[1]),
        WelfordAccumulator::merge(shards[2], shards[3])
    );

    std::cout << "\nParallel 4-shard Chan merge vs sequential Welford (N=4000):\n";
    std::cout << "  Sequential mean = " << std::setprecision(6) << full.mean()
              << ", var = " << full.sample_variance() << "\n";
    std::cout << "  Merged     mean = " << merged.mean()
              << ", var = " << merged.sample_variance() << "\n";

    EWMAEstimator ewma(0.1);
    for (int i = 0; i < 200; ++i) ewma.update(50.0 + ((i & 1) ? 5.0 : -5.0));
    std::cout << "\nEWMA (alpha=0.1) on 50 +- 5ms stream: mean = " << ewma.mean()
              << " ms, stddev = " << ewma.stddev() << " ms\n";

    bool ok = std::fabs(w.sample_variance() - 30.0) < 1e-7 &&
              std::fabs(full.mean() - merged.mean()) < 1e-7 &&
              std::fabs(full.sample_variance() - merged.sample_variance()) < 1e-7;

    std::cout << "\nNumerical stability & parallel Chan merge check: "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
