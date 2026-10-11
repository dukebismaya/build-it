/*
 * Problem Description:
 * Simpson's 1/3 Rule for Numerical Integration.
 * Approximates the definite integral of a real-valued function f(x) over an interval [a, b]
 * by interpolating f(x) using quadratic parabolas on each pair of consecutive subintervals.
 * The interval [a, b] is partitioned into an even number of subintervals n with width h = (b - a) / n.
 * Formula:
 * Integral ~= (h / 3) * [f(a) + 4 * sum(f(x_odd)) + 2 * sum(f(x_even)) + f(b)]
 * Error order: O(h^4) or O(1/n^4), yielding high precision for smooth integrands.
 */

#include <iostream>
#include <iomanip>
#include <cmath>
#include <functional>

// Simpson's 1/3 rule numerical integration
double simpsonsRule(const std::function<double(double)>& f, double a, double b, int n) {
    // n must be an even number of subintervals
    if (n % 2 != 0) {
        n++;
    }

    double h = (b - a) / n;
    double sum = f(a) + f(b);

    for (int i = 1; i < n; ++i) {
        double x = a + i * h;
        if (i % 2 == 0) {
            sum += 2.0 * f(x);
        } else {
            sum += 4.0 * f(x);
        }
    }

    return (h / 3.0) * sum;
}

int main() {
    // Test 1: Integral of sin(x) from 0 to PI (Exact: 2.0)
    const double PI = 3.14159265358979323846;
    auto f1 = [](double x) { return std::sin(x); };
    double res1 = simpsonsRule(f1, 0.0, PI, 100);

    // Test 2: Integral of x^2 from 0 to 3 (Exact: 9.0)
    auto f2 = [](double x) { return x * x; };
    double res2 = simpsonsRule(f2, 0.0, 3.0, 100);

    // Test 3: Integral of 1 / (1 + x^2) from 0 to 1 (Exact: PI / 4 ~= 0.785398)
    auto f3 = [](double x) { return 1.0 / (1.0 + x * x); };
    double res3 = simpsonsRule(f3, 0.0, 1.0, 100);

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Simpson's 1/3 Rule Numerical Integration Results:\n";
    std::cout << "1. Integral of sin(x) [0, PI]         : " << res1 << " (Expected: 2.000000)\n";
    std::cout << "2. Integral of x^2 [0, 3]             : " << res2 << " (Expected: 9.000000)\n";
    std::cout << "3. Integral of 1/(1 + x^2) [0, 1]     : " << res3 << " (Expected: " << PI / 4.0 << ")\n";

    return 0;
}
