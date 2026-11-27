/*
 * ============================================================
 *       Token Bucket and Leaky Bucket API Rate Limiters
 * ============================================================
 *
 * Description:
 *   Rate limiters control request throughput in APIs, gateways,
 *   and network packet schedulers:
 *
 *   1. Token Bucket (Burst-Tolerant):
 *      - Bucket holds up to B tokens, refilled continuously at
 *        rate R tokens/second:
 *          tokens = min(B, tokens + (now - last_time) * R)
 *      - A request of cost C is allowed iff tokens >= C (consuming
 *        C tokens); otherwise we can compute the exact wait time
 *        (C - tokens) / R until enough tokens accumulate.
 *      - Allows short bursts of up to B requests while enforcing
 *        long-term average rate R.
 *
 *   2. Leaky Bucket as a Meter / Queue (Smooth Shaping):
 *      - Water (work) leaks out at a constant rate R units/sec
 *        and overflows at capacity B. Equivalently, tracks the
 *        Theoretical Arrival Time (TAT) via the Generic Cell Rate
 *        Algorithm (GCRA) used in ATM/telecom and Redis-cell.
 *
 * Complexity:
 *   Time:  O(1) per allow() check (lazy timestamp arithmetic, no
 *          background timer threads needed).
 *   Space: O(1) per limiter instance.
 * ============================================================
 */

#include <iostream>
#include <algorithm>
#include <cmath>
#include <iomanip>

namespace rate_limit {

    class TokenBucket {
        double capacity_;
        double fill_rate_;     // tokens per second
        double tokens_;
        double last_time_;

        void refill(double now) {
            if (now > last_time_) {
                tokens_ = std::min(capacity_, tokens_ + (now - last_time_) * fill_rate_);
                last_time_ = now;
            }
        }

    public:
        TokenBucket(double capacity, double fill_rate, double start_time = 0.0)
            : capacity_(capacity), fill_rate_(fill_rate), tokens_(capacity), last_time_(start_time) {}

        bool try_consume(double now, double cost = 1.0) {
            refill(now);
            if (tokens_ + 1e-12 >= cost) {
                tokens_ = std::max(0.0, tokens_ - cost);
                return true;
            }
            return false;
        }

        double time_until_available(double now, double cost = 1.0) {
            refill(now);
            if (tokens_ >= cost) return 0.0;
            return (cost - tokens_) / fill_rate_;
        }

        double available_tokens(double now) {
            refill(now);
            return tokens_;
        }
    };

    // GCRA (Generic Cell Rate Algorithm) Leaky Bucket
    class LeakyBucketGCRA {
        double emission_interval_; // T = 1 / rate
        double delay_tolerance_;   // tau = (burst - 1) * T
        double tat_ = 0.0;         // Theoretical Arrival Time

    public:
        LeakyBucketGCRA(double rate_per_sec, double burst_capacity)
            : emission_interval_(1.0 / rate_per_sec),
              delay_tolerance_((burst_capacity - 1.0) / rate_per_sec) {}

        bool try_acquire(double now, double& retry_after) {
            double new_tat = std::max(now, tat_) + emission_interval_;
            double allow_at = new_tat - emission_interval_ - delay_tolerance_;
            if (now + 1e-12 >= allow_at) {
                tat_ = new_tat;
                retry_after = 0.0;
                return true;
            }
            retry_after = allow_at - now;
            return false;
        }
    };

} // namespace rate_limit

int main() {
    using namespace rate_limit;
    std::cout << "============================================================\n";
    std::cout << "       Token Bucket and Leaky Bucket API Rate Limiters\n";
    std::cout << "============================================================\n\n";

    // Capacity = 5 requests burst, sustained refill = 2 requests/sec
    TokenBucket tb(5.0, 2.0, 0.0);
    LeakyBucketGCRA gcra(2.0, 5.0);

    std::cout << "Simulating burst of 7 requests at t=0.0s (burst=5, rate=2/s):\n";
    int tb_allowed = 0, gcra_allowed = 0;
    for (int i = 1; i <= 7; ++i) {
        double wait = tb.time_until_available(0.0, 1.0);
        bool ok1 = tb.try_consume(0.0, 1.0);
        double retry2 = 0.0;
        bool ok2 = gcra.try_acquire(0.0, retry2);
        tb_allowed += ok1;
        gcra_allowed += ok2;
        std::cout << "  Req #" << i << ": TokenBucket=" << (ok1 ? "ALLOW" : "DENY ")
                  << " (wait " << std::fixed << std::setprecision(2) << wait << "s)"
                  << " | GCRA=" << (ok2 ? "ALLOW" : "DENY ")
                  << " (retry " << retry2 << "s)\n";
    }

    bool ok = (tb_allowed == 5 && gcra_allowed == 5);

    // Over 100 seconds at 10 requests/sec against a 2 req/sec + 5 burst limiter,
    // exactly 5 (initial burst) + 100 * 2 = 205 requests must be admitted.
    TokenBucket sim_tb(5.0, 2.0, 0.0);
    LeakyBucketGCRA sim_gcra(2.0, 5.0);
    int c1 = 0, c2 = 0;
    for (int step = 0; step <= 1000; ++step) {
        double t = step * 0.1;
        if (sim_tb.try_consume(t, 1.0)) ++c1;
        double r = 0.0;
        if (sim_gcra.try_acquire(t, r)) ++c2;
    }
    std::cout << "\nOver 100s under heavy load (expected 205 admitted): TokenBucket=" << c1
              << ", GCRA=" << c2 << "\n";
    if (c1 != 205 || c2 != 205) ok = false;

    std::cout << "Burst & sustained rate verification: " << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
