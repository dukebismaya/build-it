#include <format>
#include <iostream>
#include <vector>

void insertionSort(std::vector<int> &arr) {
  int i{1}, n = arr.size();
  while (i < n) {
    int j{i - 1}, key = arr[i];
    while (j >= 0 && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
    i++;
  }
}

int main() {
  std::vector<int> arr{3, 17, 0, 2, 2, 3};
  std::cout << "Unsorted Array: " << std::format("{}\n", arr);
  insertionSort(arr);
  std::cout << "Sorted Array (After Insertion Sort): "
            << std::format("{}\n", arr);
  return 0;
}