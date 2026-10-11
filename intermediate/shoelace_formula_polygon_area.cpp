/*
 * Problem Description:
 * Shoelace Formula (Gauss's Area Formula).
 * Computes the area of a simple planar polygon given the ordered Cartesian coordinates
 * of its vertices (in either clockwise or counter-clockwise order).
 * The formula accumulates cross products between consecutive vertex pairs:
 * Area = 0.5 * |sum_{i=0}^{n-1} (x_i * y_{i+1} - x_{i+1} * y_i)|, with (x_n, y_n) = (x_0, y_0).
 * Time Complexity: O(N) where N is the number of polygon vertices.
 * Space Complexity: O(1) auxiliary space.
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>

struct Point {
    double x;
    double y;
};

// Function to calculate polygon area using Shoelace formula
double polygonArea(const std::vector<Point>& vertices) {
    int n = vertices.size();
    if (n < 3) {
        return 0.0; // A polygon must have at least 3 vertices
    }

    double area = 0.0;
    for (int i = 0; i < n; ++i) {
        int next = (i + 1) % n;
        area += (vertices[i].x * vertices[next].y) - (vertices[next].x * vertices[i].y);
    }

    return std::abs(area) / 2.0;
}

int main() {
    // Example 1: Pentagon with known area
    std::vector<Point> pentagon = {
        {1.0, 6.0},
        {3.0, 1.0},
        {7.0, 2.0},
        {4.0, 4.0},
        {8.0, 5.0}
    };

    // Example 2: Simple square of side length 4 (Area = 16)
    std::vector<Point> square = {
        {0.0, 0.0},
        {4.0, 0.0},
        {4.0, 4.0},
        {0.0, 4.0}
    };

    // Example 3: Triangle (Base = 6, Height = 4, Area = 12)
    std::vector<Point> triangle = {
        {0.0, 0.0},
        {6.0, 0.0},
        {3.0, 4.0}
    };

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Shoelace Formula Polygon Area Calculation:\n";
    std::cout << "Pentagon Area: " << polygonArea(pentagon) << "\n";
    std::cout << "Square Area (expected 16.00): " << polygonArea(square) << "\n";
    std::cout << "Triangle Area (expected 12.00): " << polygonArea(triangle) << "\n";

    return 0;
}
