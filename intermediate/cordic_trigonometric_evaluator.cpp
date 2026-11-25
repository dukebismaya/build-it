/*
 * ============================================================
 *         CORDIC Trigonometric and Vectoring Evaluator
 * ============================================================
 *
 * Description:
 *   CORDIC (COordinate Rotation DIgital Computer, Jack Volder
 *   1959) evaluates trigonometric functions using ONLY shift,
 *   add/subtract, and a table of N precomputed angles
 *     theta_i = arctan(2^{-i}),   i = 0, 1, ..., N-1.
 *
 *   A micro-rotation by sigma_i * theta_i (sigma_i in {-1, +1})
 *   is written as:
 *     x_{i+1} = x_i - sigma_i * y_i * 2^{-i}
 *     y_{i+1} = y_i + sigma_i * x_i * 2^{-i}
 *     z_{i+1} = z_i - sigma_i * theta_i
 *
 *   Each step scales the vector length by sqrt(1 + 2^{-2i}), so
 *   starting with x_0 = K = prod cos(theta_i) ~ 0.607252935008881
 *   and y_0 = 0 in ROTATION mode (driving z -> 0 with
 *   sigma_i = sign(z_i)) produces:
 *     (x_N, y_N) = (cos z_0, sin z_0).
 *
 *   In VECTORING mode (driving y -> 0 with sigma_i = -sign(y_i)
 *   from (x_0, y_0), z_0 = 0), we obtain:
 *     x_N = sqrt(x_0^2 + y_0^2) / K,   z_N = atan2(y_0, x_0).
 *
 * Complexity:
 *   O(N) iterations (~1 bit of precision per iteration).
 * ============================================================
 */

#include <iostream>
#include <cmath>
#include <array>
#include <utility>
#include <iomanip>

namespace cordic {

    constexpr int STEPS = 48;

    struct Tables {
        std::array<double, STEPS> atan_pow2;
        double K;
    };

    const Tables& tables() {
        static Tables t = [] {
            Tables tb{};
            double k = 1.0;
            for (int i = 0; i < STEPS; ++i) {
                double p = std::ldexp(1.0, -i);
                tb.atan_pow2[i] = std::atan(p);
                k /= std::sqrt(1.0 + p * p);
            }
            tb.K = k;
            return tb;
        }();
        return t;
    }

    // Rotation mode: returns {cos(angle), sin(angle)} for angle in [-pi, pi]
    std::pair<double, double> cos_sin(double angle) {
        const double PI = std::acos(-1.0);
        int flip = 1;
        if (angle > PI / 2)       { angle -= PI; flip = -1; }
        else if (angle < -PI / 2) { angle += PI; flip = -1; }

        const auto& tb = tables();
        double x = tb.K, y = 0.0, z = angle;
        for (int i = 0; i < STEPS; ++i) {
            double s = (z >= 0.0) ? 1.0 : -1.0;
            double dx = std::ldexp(y, -i), dy = std::ldexp(x, -i);
            x -= s * dx;
            y += s * dy;
            z -= s * tb.atan_pow2[i];
        }
        return {flip * x, flip * y};
    }

    // Vectoring mode: returns {hypot(x, y), atan2(y, x)}
    std::pair<double, double> hypot_atan2(double x0, double y0) {
        const double PI = std::acos(-1.0);
        double x = x0, y = y0, z = 0.0;
        if (x < 0.0) {
            x = -x; y = -y;
            z = (y0 >= 0.0) ? PI : -PI;
        }
        const auto& tb = tables();
        for (int i = 0; i < STEPS; ++i) {
            double s = (y < 0.0) ? 1.0 : -1.0;
            double dx = std::ldexp(y, -i), dy = std::ldexp(x, -i);
            x -= s * dx;
            y += s * dy;
            z -= s * tb.atan_pow2[i];
        }
        return {x * tb.K, z};
    }

} // namespace cordic

int main() {
    using namespace cordic;
    const double PI = std::acos(-1.0);
    std::cout << "============================================================\n";
    std::cout << "         CORDIC Trigonometric and Vectoring Evaluator\n";
    std::cout << "============================================================\n\n";

    std::cout << "CORDIC gain constant K = " << std::fixed << std::setprecision(15) << tables().K << "\n\n";
    std::cout << std::setw(10) << "Angle" << std::setw(20) << "CORDIC cos"
              << std::setw(20) << "CORDIC sin" << std::setw(14) << "Max Error" << "\n";
    std::cout << std::string(64, '-') << "\n";

    bool ok = true;
    for (int deg = -180; deg <= 180; deg += 30) {
        double rad = deg * PI / 180.0;
        auto [c, s] = cos_sin(rad);
        double err = std::max(std::fabs(c - std::cos(rad)), std::fabs(s - std::sin(rad)));
        if (err > 1e-12) ok = false;
        std::cout << std::setw(7) << deg << " deg"
                  << std::setw(20) << std::fixed << std::setprecision(13) << c
                  << std::setw(20) << s
                  << std::setw(14) << std::scientific << std::setprecision(2) << err << "\n";
    }

    // Vectoring test across all 4 quadrants
    double pts[][2] = {{3, 4}, {-5, 12}, {-8, -15}, {7, -24}, {1.234, 5.678}};
    for (auto& p : pts) {
        auto [r, ang] = hypot_atan2(p[0], p[1]);
        if (std::fabs(r - std::hypot(p[0], p[1])) > 1e-11 ||
            std::fabs(ang - std::atan2(p[1], p[0])) > 1e-11) ok = false;
    }
    std::cout << "\nRotation & vectoring accuracy (< 1e-11 across all quadrants): "
              << (ok ? "PASSED" : "FAILED") << "\n";
    return 0;
}
