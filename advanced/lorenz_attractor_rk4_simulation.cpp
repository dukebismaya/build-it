/**
 * @file lorenz_attractor_rk4_simulation.cpp
 * @brief Lorenz Attractor 3D chaotic simulation & Lyapunov divergence via RK4 in C++17.
 *
 * Overview (Edward Lorenz, 1963):
 *   A system of three ordinary differential equations originally derived as a simplified
 *   model for atmospheric convection:
 *     dx/dt = sigma * (y - x)
 *     dy/dt = x * (rho - z) - y
 *     dz/dt = x * y - beta * z
 *
 * Classical Chaotic Parameters:
 *   sigma = 10.0 (Prandtl number)
 *   rho   = 28.0 (Rayleigh number)
 *   beta  = 8.0 / 3.0 (Geometric aspect ratio)
 *
 * The Butterfly Effect & Lyapunov Exponent:
 *   Trajectories that start with an imperceptible initial difference delta_0 = 1e-8
 *   diverge exponentially fast at rate:
 *     delta(t) ~ delta_0 * exp(lambda * t)
 *   where lambda is the maximal Lyapunov exponent (~0.90 for standard Lorenz).
 *
 * Complexity:
 *   - Time: O(Steps)
 *   - Space: O(Steps) or O(1) streaming state.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <array>

namespace chaos {

using State3D = std::array<double, 3>;

struct LorenzParams {
    double sigma = 10.0;
    double rho = 28.0;
    double beta = 8.0 / 3.0;
};

/**
 * @brief Computes derivatives [dx/dt, dy/dt, dz/dt].
 */
State3D lorenzDerivatives(const State3D& s, const LorenzParams& p) {
    double x = s[0], y = s[1], z = s[2];
    return {
        p.sigma * (y - x),
        x * (p.rho - z) - y,
        x * y - p.beta * z
    };
}

/**
 * @brief 4th-Order Runge-Kutta step for 3D state.
 */
State3D rk4Step(const State3D& s, const LorenzParams& p, double dt) {
    State3D k1 = lorenzDerivatives(s, p);

    State3D s2 = {s[0] + 0.5 * dt * k1[0], s[1] + 0.5 * dt * k1[1], s[2] + 0.5 * dt * k1[2]};
    State3D k2 = lorenzDerivatives(s2, p);

    State3D s3 = {s[0] + 0.5 * dt * k2[0], s[1] + 0.5 * dt * k2[1], s[2] + 0.5 * dt * k2[2]};
    State3D k3 = lorenzDerivatives(s3, p);

    State3D s4 = {s[0] + dt * k3[0], s[1] + dt * k3[1], s[2] + dt * k3[2]};
    State3D k4 = lorenzDerivatives(s4, p);

    return {
        s[0] + (dt / 6.0) * (k1[0] + 2.0 * k2[0] + 2.0 * k3[0] + k4[0]),
        s[1] + (dt / 6.0) * (k1[1] + 2.0 * k2[1] + 2.0 * k3[1] + k4[1]),
        s[2] + (dt / 6.0) * (k1[2] + 2.0 * k2[2] + 2.0 * k3[2] + k4[2])
    };
}

double distance3D(const State3D& a, const State3D& b) {
    double dx = a[0] - b[0];
    double dy = a[1] - b[1];
    double dz = a[2] - b[2];
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

} // namespace chaos

int main() {
    std::cout << "========================================\n";
    std::cout << "    LORENZ CHAOTIC ATTRACTOR (RK4)      \n";
    std::cout << "   (Butterfly Effect & Chaos Horizon)   \n";
    std::cout << "========================================\n\n";

    chaos::LorenzParams params;
    double dt = 0.01;
    double tMax = 30.0;

    // Two trajectories starting with an imperceptible difference of 1e-8 in x
    chaos::State3D traj1 = {1.0, 1.0, 1.0};
    chaos::State3D traj2 = {1.0 + 1e-8, 1.0, 1.0};
    double initialDist = chaos::distance3D(traj1, traj2);

    std::cout << "Parameters: sigma = " << params.sigma << ", rho = " << params.rho
              << ", beta = 8/3\n";
    std::cout << "Initial Separation: delta(0) = " << std::scientific << initialDist << "\n\n";

    std::cout << std::left << std::setw(8)  << "Time t"
              << std::setw(28) << "Trajectory 1 (x, y, z)"
              << std::setw(28) << "Trajectory 2 (x, y, z)"
              << "Separation delta(t)\n";
    std::cout << std::string(82, '-') << "\n";

    double t = 0.0;
    int printInterval = 250; // Every 2.5 seconds
    int step = 0;

    while (t <= tMax + 1e-9) {
        if (step % printInterval == 0) {
            double dist = chaos::distance3D(traj1, traj2);

            std::cout << std::fixed << std::setprecision(1);
            std::cout << std::left << std::setw(8) << t;

            std::cout << std::setprecision(2);
            std::string s1 = "(" + std::to_string(traj1[0]).substr(0, 5) + "," +
                                   std::to_string(traj1[1]).substr(0, 5) + "," +
                                   std::to_string(traj1[2]).substr(0, 5) + ")";
            std::string s2 = "(" + std::to_string(traj2[0]).substr(0, 5) + "," +
                                   std::to_string(traj2[1]).substr(0, 5) + "," +
                                   std::to_string(traj2[2]).substr(0, 5) + ")";

            std::cout << std::left << std::setw(28) << s1
                      << std::setw(28) << s2
                      << std::scientific << std::setprecision(4) << dist << "\n";
        }

        traj1 = chaos::rk4Step(traj1, params, dt);
        traj2 = chaos::rk4Step(traj2, params, dt);
        t += dt;
        step++;
    }

    std::cout << "\nObservation:\n";
    std::cout << "  - Trajectories remain virtually identical for t < 15s.\n";
    std::cout << "  - Beyond t = 20s, trajectories completely decorrelate into opposite attractor lobes.\n";
    std::cout << "  - Demonstrates deterministic chaos and the limits of long-term predictability.\n\n";

    std::cout << "Program finished successfully.\n";
    return 0;
}
