/*
 * ============================================================
 *       Kinetic Tournament for Linearly Moving Points
 * ============================================================
 *
 * Description:
 *   A Kinetic Data Structure (Basch, Guibas, Hershberger, 1997)
 *   maintains a geometric/combinatorial attribute of continuously
 *   moving objects by storing a set of local CERTIFICATES and an
 *   event queue of their earliest failure times.
 *
 *   Here we maintain the maximum of N linearly moving items:
 *     f_i(t) = a_i * t + b_i,   t >= t_0
 *   using a balanced binary tournament tree:
 *     - Leaves hold items 0..N-1.
 *     - Each internal node u holds the current winner of its two
 *       children left(u), right(u), plus a comparison certificate
 *       asserting f_{winner}(t) >= f_{loser}(t).
 *     - If the loser has strictly larger slope than the winner,
 *       the certificate fails at t_fail = (b_win - b_lose) / (a_lose - a_win),
 *       which is scheduled in a priority queue with a version stamp.
 *     - When an event fires, we recompute the node's winner and
 *       propagate any winner changes up to the root in O(log N).
 *
 * Complexity:
 *   Each kinetic event takes O(log^2 N) time; for linear trajectories
 *   there are at most O(N log N) events total.
 * ============================================================
 */

#include <iostream>
#include <vector>
#include <queue>
#include <cmath>
#include <random>
#include <iomanip>
#include <algorithm>

namespace kinetic {

    struct Line {
        double a, b;   // f(t) = a * t + b
        double eval(double t) const { return a * t + b; }
    };

    class KineticTournament {
        struct Event {
            double time;
            int node, version;
            bool operator>(const Event& o) const { return time > o.time; }
        };

        int n, size;
        double now;
        std::vector<Line> lines;
        std::vector<int> winner, ver;
        std::priority_queue<Event, std::vector<Event>, std::greater<Event>> pq;

        int better(int i, int j, double t) const {
            if (i == -1) return j;
            if (j == -1) return i;
            double vi = lines[i].eval(t), vj = lines[j].eval(t);
            if (std::fabs(vi - vj) > 1e-12) return (vi > vj) ? i : j;
            return (lines[i].a > lines[j].a) ? i : j;   // tie-break by larger slope
        }

        void update_node(int u) {
            int l = winner[2 * u], r = winner[2 * u + 1];
            int w = better(l, r, now);
            int loser = (w == l) ? r : l;
            winner[u] = w;
            int my_ver = ++ver[u];
            if (w != -1 && loser != -1 && lines[loser].a > lines[w].a) {
                double t_fail = (lines[w].b - lines[loser].b) / (lines[loser].a - lines[w].a);
                if (t_fail >= now - 1e-12)
                    pq.push({std::max(now, t_fail), u, my_ver});
            }
        }

    public:
        int events_processed = 0;

        KineticTournament(std::vector<Line> items, double start_time = 0.0)
            : n(static_cast<int>(items.size())), now(start_time), lines(std::move(items)) {
            size = 1;
            while (size < n) size <<= 1;
            winner.assign(2 * size, -1);
            ver.assign(2 * size, 0);
            for (int i = 0; i < n; ++i) winner[size + i] = i;
            for (int u = size - 1; u >= 1; --u) update_node(u);
        }

        void advance_to(double t_target) {
            while (!pq.empty() && pq.top().time <= t_target) {
                Event ev = pq.top(); pq.pop();
                if (ev.version != ver[ev.node]) continue;
                now = ev.time;
                ++events_processed;
                for (int u = ev.node; u >= 1; u >>= 1) update_node(u);
            }
            now = t_target;
        }

        int current_max_index() const { return winner[1]; }
        double current_max_value() const { return lines[winner[1]].eval(now); }

        void change_trajectory(int idx, Line new_line) {
            lines[idx] = new_line;
            for (int u = (size + idx) >> 1; u >= 1; u >>= 1) update_node(u);
        }
    };

} // namespace kinetic

int main() {
    using namespace kinetic;
    std::cout << "============================================================\n";
    std::cout << "       Kinetic Tournament for Linearly Moving Points\n";
    std::cout << "============================================================\n\n";

    std::vector<Line> demo = {
        {0.5, 10.0},   // starts high, slow
        {2.0,  4.0},   // medium
        {4.0, -2.0},   // starts low, steep
        {1.0,  8.0}
    };
    KineticTournament kt(demo, 0.0);
    for (double t : {0.0, 1.5, 2.5, 4.0, 5.0, 8.0}) {
        kt.advance_to(t);
        std::cout << "t = " << std::fixed << std::setprecision(1) << std::setw(4) << t
                  << ": leader = item " << kt.current_max_index()
                  << ", value = " << std::setprecision(3) << kt.current_max_value() << "\n";
    }

    // Randomized verification with flight-plan updates
    std::mt19937 rng(777);
    const int N = 120;
    std::vector<Line> lines(N);
    for (auto& l : lines) {
        l.a = (static_cast<int>(rng() % 2001) - 1000) / 100.0;
        l.b = (static_cast<int>(rng() % 20001) - 10000) / 100.0;
    }
    KineticTournament big(lines, 0.0);
    bool ok = true;
    for (int step = 0; step <= 400 && ok; ++step) {
        double t = step * 0.25;
        big.advance_to(t);
        double got = big.current_max_value();
        double brute = -1e300;
        for (const auto& l : lines) brute = std::max(brute, l.eval(t));
        if (std::fabs(got - brute) > 1e-7) ok = false;

        if (step % 25 == 0) {
            int idx = rng() % N;
            Line nl{(static_cast<int>(rng() % 2001) - 1000) / 100.0,
                    (static_cast<int>(rng() % 20001) - 10000) / 100.0};
            lines[idx] = nl;
            big.change_trajectory(idx, nl);
        }
    }
    std::cout << "\nKinetic events processed: " << big.events_processed << "\n";
    std::cout << "Random verification (120 trajectories, 400 time steps + flight updates): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
