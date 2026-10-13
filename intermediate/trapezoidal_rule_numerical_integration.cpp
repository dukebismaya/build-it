/*
 * Problem Description:
 * Trapezoidal Rule for Numerical Integration.
 * Approximates the definite integral of f(x) over [a, b] by dividing the interval into n
 * subintervals and summing the areas of trapezoids formed at each subinterval.
 * Formula: Integral ≈ (h/2) * [f(a) + 2*f(x1) + 2*f(x2) + ... + 2*f(x_{n-1}) + f(b)]
 * where h = (b - a) / n.
 * Error order: O(h^2) or O(1/n^2).
 * Compared with Simpson's rule (O(h^4)), the trapezoidal rule is simpler but less accurate.
 */

#include <iostream>
#include <iomanip>
#include <cmath>
#include <functional>

double trapezoidalRule(const std::function<double(double)>& f, double a, double b, int n) {
    double h = (b - a) / n;
    double sum = f(a) + f(b);

    for (int i = 1; i < n; ++i) {
        double x = a + i * h;
        sum += 2.0 * f(x);
    }

    return (h / 2.0) * sum;
}

int main() {
    const double PI = 3.14159265358979323846;

    // Test 1: Integral of sin(x) from 0 to PI (Exact: 2.0)
    auto f1 = [](double x) { return std::sin(x); };

    // Test 2: Integral of e^x from 0 to 1 (Exact: e - 1 ≈ 1.71828)
    auto f2 = [](double x) { return std::exp(x); };

    // Test 3: Integral of x^3 from 0 to 2 (Exact: 4.0)
    auto f3 = [](double x) { return x * x * x; };

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Trapezoidal Rule Numerical Integration:\n\n";

    for (int n : {10, 100, 1000}) {
        std::cout << "n = " << n << " subintervals:\n";
        std::cout << "  sin(x) [0, PI]  : " << trapezoidalRule(f1, 0.0, PI, n) << " (exact: 2.000000)\n";
        std::cout << "  e^x [0, 1]      : " << trapezoidalRule(f2, 0.0, 1.0, n) << " (exact: " << std::exp(1.0) - 1.0 << ")\n";
        std::cout << "  x^3 [0, 2]      : " << trapezoidalRule(f3, 0.0, 2.0, n) << " (exact: 4.000000)\n\n";
    }

    return 0;
}
