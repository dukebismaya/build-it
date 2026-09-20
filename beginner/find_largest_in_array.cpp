#include <iostream>

int main() {
    int i, n;
    float arr[100];

    std::cout << "Enter total number of elements(1 to 100): ";
    std::cin >> n;
    std::cout << "\n";

    for(i = 0; i < n; ++i) {
        std::cout << "Enter Number " << i + 1 << " : ";
        std::cin >> arr[i];
    }

    for(i = 1; i < n; ++i) {
        if(arr[0] < arr[i])
            arr[0] = arr[i];
    }

    std::cout << "\nLargest element = " << arr[0] << "\n";

    return 0;
}
