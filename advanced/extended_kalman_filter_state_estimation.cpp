/**
 * @file extended_kalman_filter_state_estimation.cpp
 * @brief Extended Kalman Filter (EKF) for non-linear radar tracking in C++17.
 *
 * The Extended Kalman Filter (EKF) is the non-linear version of the Kalman filter
 * which linearizes about an estimate of the current mean and covariance.
 *
 * System Model (2D Target Tracking):
 *   State vector x = [p_x, p_y, v_x, v_y]^T (position and velocity in Cartesian 2D).
 *   Dynamic model (Linear Constant Velocity):
 *     x_{k} = F * x_{k-1} + w_{k},  w_k ~ N(0, Q)
 *     F = [ [1, 0, dt, 0],
 *           [0, 1, 0, dt],
 *           [0, 0, 1,  0],
 *           [0, 0, 0,  1] ]
 *
 * Measurement Model (Non-linear Radar Sensing in Polar Coordinates):
 *   z = h(x) + v,  v ~ N(0, R)
 *   h(x) = [ range (rho), bearing (phi) ]^T:
 *     rho = sqrt(p_x^2 + p_y^2)
 *     phi = atan2(p_y, p_x)
 *
 * Jacobian Matrix H_j = dh/dx evaluated at predicted state x^-:
 *   H_j = [ [ p_x / rho,       p_y / rho,       0, 0 ],
 *           [ -p_y / rho^2,    p_x / rho^2,     0, 0 ] ]
 *
 * Complexity:
 *   - Prediction Step: O(n^3) matrix multiplications (n = 4 -> negligible).
 *   - Update Step: O(m^3 + n * m^2) (m = 2 measurement dimension).
 *   - Per-step time: O(1), memory: O(1).
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include <array>

namespace estimation {

// 4-state vector: [px, py, vx, vy]
using Vec4 = std::array<double, 4>;
using Mat4 = std::array<std::array<double, 4>, 4>;

// 2-measurement vector: [range, bearing]
using Vec2 = std::array<double, 2>;
using Mat2 = std::array<std::array<double, 2>, 2>;
using Mat2x4 = std::array<std::array<double, 4>, 2>;
using Mat4x2 = std::array<std::array<double, 2>, 4>;

/**
 * @brief Normalizes angle into [-pi, pi].
 */
double normalizeAngle(double angle) {
    const double PI = 3.14159265358979323846;
    while (angle > PI) angle -= 2.0 * PI;
    while (angle < -PI) angle += 2.0 * PI;
    return angle;
}

class ExtendedKalmanFilter {
public:
    Vec4 x; // Estimated state
    Mat4 P; // State covariance
    Mat4 Q; // Process noise covariance
    Mat2 R; // Measurement noise covariance

    ExtendedKalmanFilter() {
        x = {0.0, 0.0, 0.0, 0.0};
        // Initialize covariance with high uncertainty
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                P[i][j] = (i == j) ? 10.0 : 0.0;
                Q[i][j] = (i == j) ? 0.05 : 0.0;
            }
        }
        R[0][0] = 0.5;   // Range variance (m^2)
        R[0][1] = 0.0;
        R[1][0] = 0.0;
        R[1][1] = 0.005; // Bearing variance (rad^2)
    }

    void predict(double dt) {
        // State Transition Matrix F
        Mat4 F = {{
            {1.0, 0.0, dt,  0.0},
            {0.0, 1.0, 0.0, dt },
            {0.0, 0.0, 1.0, 0.0},
            {0.0, 0.0, 0.0, 1.0}
        }};

        // x = F * x
        Vec4 xNew = {0.0, 0.0, 0.0, 0.0};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                xNew[i] += F[i][j] * x[j];
            }
        }
        x = xNew;

        // P = F * P * F^T + Q
        Mat4 FP = {};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    FP[i][j] += F[i][k] * P[k][j];
                }
            }
        }

        Mat4 PNew = {};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    PNew[i][j] += FP[i][k] * F[j][k]; // F^T[k][j] = F[j][k]
                }
                PNew[i][j] += Q[i][j];
            }
        }
        P = PNew;
    }

    void update(const Vec2& z) {
        double px = x[0];
        double py = x[1];
        double rho = std::sqrt(px * px + py * py);
        if (rho < 1e-4) {
            return; // Avoid division by zero
        }
        double rho2 = rho * rho;

        // Jacobian H_j
        Mat2x4 Hj = {{
            { px / rho,     py / rho,    0.0, 0.0 },
            { -py / rho2,   px / rho2,   0.0, 0.0 }
        }};

        // Non-linear observation function h(x)
        Vec2 hx = { rho, std::atan2(py, px) };

        // Innovation y = z - h(x)
        Vec2 y = { z[0] - hx[0], normalizeAngle(z[1] - hx[1]) };

        // S = Hj * P * Hj^T + R
        Mat2 S = R;
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 4; ++k) {
                    for (int l = 0; l < 4; ++l) {
                        S[i][j] += Hj[i][k] * P[k][l] * Hj[j][l];
                    }
                }
            }
        }

        // S Inverse (2x2)
        double det = S[0][0] * S[1][1] - S[0][1] * S[1][0];
        Mat2 S_inv = {{
            { S[1][1] / det, -S[0][1] / det },
            { -S[1][0] / det, S[0][0] / det }
        }};

        // Kalman Gain K = P * Hj^T * S_inv
        Mat4x2 K = {};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 2; ++j) {
                for (int k = 0; k < 4; ++k) {
                    for (int l = 0; l < 2; ++l) {
                        K[i][j] += P[i][k] * Hj[l][k] * S_inv[l][j];
                    }
                }
            }
        }

        // State update: x = x + K * y
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 2; ++j) {
                x[i] += K[i][j] * y[j];
            }
        }

        // Covariance update: P = (I - K * Hj) * P
        Mat4 I_KHj = {};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                I_KHj[i][j] = (i == j) ? 1.0 : 0.0;
                for (int k = 0; k < 2; ++k) {
                    I_KHj[i][j] -= K[i][k] * Hj[k][j];
                }
            }
        }

        Mat4 PUpdated = {};
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                for (int k = 0; k < 4; ++k) {
                    PUpdated[i][j] += I_KHj[i][k] * P[k][j];
                }
            }
        }
        P = PUpdated;
    }
};

} // namespace estimation

int main() {
    std::cout << "========================================\n";
    std::cout << "  EXTENDED KALMAN FILTER (EKF) TRACKING \n";
    std::cout << " (2D Cartesian Kinematics + Polar Radar)\n";
    std::cout << "========================================\n\n";

    estimation::ExtendedKalmanFilter ekf;
    // Initial truth: pos = (10.0, 10.0), vel = (1.5, 0.5)
    double truePx = 10.0;
    double truePy = 10.0;
    double vx = 1.5;
    double vy = 0.5;
    double dt = 1.0;

    // Initialize EKF with approximate starting guess
    ekf.x = {9.0, 11.0, 0.0, 0.0};

    std::cout << std::fixed << std::setprecision(2);
    std::cout << std::left << std::setw(6)  << "Step"
              << std::setw(20) << "True Pos (x, y)"
              << std::setw(22) << "Measured (rho, phi)"
              << std::setw(22) << "EKF Estimated (x, y)"
              << "Pos Error (m)\n";
    std::cout << std::string(85, '-') << "\n";

    for (int step = 1; step <= 8; ++step) {
        // True physics simulation
        truePx += vx * dt;
        truePy += vy * dt;

        // Non-linear Radar observation: rho = sqrt(x^2+y^2), phi = atan2(y, x)
        double trueRho = std::sqrt(truePx * truePx + truePy * truePy);
        double truePhi = std::atan2(truePy, truePx);

        // Add slight synthetic sensor noise
        double noisyRho = trueRho + ((step % 2 == 0) ? 0.3 : -0.2);
        double noisyPhi = truePhi + ((step % 3 == 0) ? 0.01 : -0.01);
        estimation::Vec2 z = {noisyRho, noisyPhi};

        // EKF Predict and Update
        ekf.predict(dt);
        ekf.update(z);

        double err = std::sqrt(std::pow(ekf.x[0] - truePx, 2) + std::pow(ekf.x[1] - truePy, 2));

        std::cout << std::left << std::setw(6) << step
                  << "(" << std::setw(6) << truePx << ", " << std::setw(6) << truePy << ")  "
                  << "(" << std::setw(6) << noisyRho << ", " << std::setw(6) << noisyPhi << ")  "
                  << "(" << std::setw(6) << ekf.x[0] << ", " << std::setw(6) << ekf.x[1] << ")  "
                  << std::setw(10) << err << "\n";
    }

    std::cout << "\nEKF Final Estimated Velocity: (" << ekf.x[2] << ", " << ekf.x[3]
              << ") m/s | True: (" << vx << ", " << vy << ") m/s\n";
    std::cout << "Program finished successfully.\n";
    return 0;
}
