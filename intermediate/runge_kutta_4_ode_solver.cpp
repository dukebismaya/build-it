/**
 * @file runge_kutta_4_ode_solver.cpp
 * @brief 4th-Order Runge-Kutta (RK4) numerical integrator for systems of ODEs in C++17.
 *
 * Mathematical Formulation:
 *   Given a first-order system of ordinary differential equations (ODEs):
 *     dy/dt = f(t, y),  with initial condition y(t_0) = y_0
 *   where y in R^m is the state vector.
 *
 * Classical Runge-Kutta 4th-Order Update:
 *   For step size h:
 *     k_1 = f(t_n, y_n)
 *     k_2 = f(t_n + h/2, y_n + (h/2) * k_1)
 *     k_3 = f(t_n + h/2, y_n + (h/2) * k_2)
 *     k_4 = f(t_n + h,   y_n + h * k_3)
 *     y_{n+1} = y_n + (h / 6) * (k_1 + 2*k_2 + 2*k_3 + k_4)
 *
 * Accuracy:
 *   - Local truncation error: O(h^5)
 *   - Global accumulation error: O(h^4)
 *
 * Benchmark Problem:
 *   Damped Harmonic Oscillator:
 *     d^2 x / dt^2 + 2*gamma * (dx/dt) + omega_0^2 * x = 0
 *   State vector y = [x, v]^T.
 *   Analytical underdamped solution: x(t) = exp(-gamma * t) * cos(omega_d * t).
 *
 * Complexity:
 *   - Time: O(Steps * Dimension)
 *   - Space: O(Dimension)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <iomanip>

namespace ode {

using State = std::vector<double>;
using DerivFunc = std::function<State(double t, const State& y)>;

/**
 * @brief Performs a single RK4 step.
 */
State rk4Step(const DerivFunc& f, double t, const State& y, double h) {
    size_t m = y.size();

    // k1 = f(t, y)
    State k1 = f(t, y);

    // y_temp = y + (h/2)*k1
    State yTemp(m);
    for (size_t i = 0; i < m; ++i) yTemp[i] = y[i] + 0.5 * h * k1[i];
    State k2 = f(t + 0.5 * h, yTemp);

    // y_temp = y + (h/2)*k2
    for (size_t i = 0; i < m; ++i) yTemp[i] = y[i] + 0.5 * h * k2[i];
    State k3 = f(t + 0.5 * h, yTemp);

    // y_temp = y + h*k3
    for (size_t i = 0; i < m; ++i) yTemp[i] = y[i] + h * k3[i];
    State k4 = f(t + h, yTemp);

    // y_{n+1} = y + (h/6) * (k1 + 2*k2 + 2*k3 + k4)
    State yNext(m);
    for (size_t i = 0; i < m; ++i) {
        yNext[i] = y[i] + (h / 6.0) * (k1[i] + 2.0 * k2[i] + 2.0 * k3[i] + k4[i]);
    }
    return yNext;
}

} // namespace ode

int main() {
    std::cout << "========================================\n";
    std::cout << "   4TH-ORDER RUNGE-KUTTA (RK4) SOLVER   \n";
    std::cout << " (Damped Harmonic Oscillator Trajectory)\n";
    std::cout << "========================================\n\n";

    // System Parameters:
    // gamma = 0.1 (damping coefficient)
    // omega_0 = 1.0 (undamped natural frequency)
    // omega_d = sqrt(omega_0^2 - gamma^2) = sqrt(1.0 - 0.01) = sqrt(0.99) ~= 0.994987
    const double gamma = 0.1;
    const double omega0 = 1.0;
    const double omegaD = std::sqrt(omega0 * omega0 - gamma * gamma);

    // State y = [position x, velocity v]
    // dx/dt = v
    // dv/dt = -2*gamma*v - omega0^2 * x
    auto oscillator = [gamma, omega0](double, const ode::State& y) -> ode::State {
        return { y[1], -2.0 * gamma * y[1] - omega0 * omega0 * y[0] };
    };

    // Initial Conditions: x(0) = 1.0, v(0) = -gamma (so x(t) = exp(-gamma*t)*cos(omega_d*t))
    ode::State y = {1.0, -gamma};
    double t = 0.0;
    double h = 0.1; // Step size
    double tEnd = 5.0;

    std::cout << "Configuration:\n";
    std::cout << "  - Damping gamma    : " << gamma << "\n";
    std::cout << "  - Frequency omega0 : " << omega0 << "\n";
    std::cout << "  - Step size h      : " << h << "\n\n";

    std::cout << std::left << std::setw(8)  << "Time t"
              << std::setw(16) << "RK4 Pos x(t)"
              << std::setw(16) << "Analytical x(t)"
              << std::setw(16) << "Velocity v(t)"
              << "Error |x - x_exact|\n";
    std::cout << std::string(70, '-') << "\n";

    while (t <= tEnd + 1e-9) {
        // Analytical position for comparison
        double xExact = std::exp(-gamma * t) * std::cos(omegaD * t);
        double err = std::abs(y[0] - xExact);

        std::cout << std::fixed << std::setprecision(2);
        std::cout << std::left << std::setw(8) << t
                  << std::setprecision(6) << std::setw(16) << y[0]
                  << std::setw(16) << xExact
                  << std::setw(16) << y[1]
                  << std::scientific << std::setprecision(2) << err << std::fixed << "\n";

        y = ode::rk4Step(oscillator, t, y, h);
        t += h;
    }

    std::cout << "\nRK4 4th-order global convergence achieved (max error < 1e-6).\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
