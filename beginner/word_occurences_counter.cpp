#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

// Part1
// Display the word and count from the
// std::map<std::string, int>

void display_words(const std::map<std::string, int> &words) {
  std::cout << std::setw(12) << std::left << "\nWord" << std::setw(7)
            << std::right << "Count" << std::endl;
  std::cout << "===================" << std::endl;
  for (const auto &[word, frequency] : words)
    std::cout << std::setw(12) << std::left << word << std::setw(7)
              << std::right << frequency << std::endl;
}

// Part2
// Display the word and occurences from the
// std::map<std::string, std::set<int>>

void display_words(const std::map<std::string, std::set<int>> &words) {
  std::cout << std::setw(12) << std::left << "\nWord"
            << "Occurrences" << std::endl;
  std::cout
      << "====================================================================="
      << std::endl;
  for (const auto &[word, occurences] : words) {
    std::cout << std::setw(12) << std::left << word << std::left << "[ ";
    for (const auto i : occurences)
      std::cout << i << " ";
    std::cout << "]" << std::endl;
  }
}

// Remove periods, commas, semicolons and colon in
// a string and returns the clean version
std::string clean_string(const std::string &s) {
  std::string result;
  for (char c : s) {
    if (c == '.' || c == ',' || c == ';' || c == ':')
      continue;
    else
      result += c;
  }
  return result;
}

// Part1 process the file and builds a map of words and the
// number of times they occur in the file

void part1() {
  std::map<std::string, int> words;
  std::ifstream in_file{"./words.txt"};
  if (in_file) {
    std::string line{};
    while (std::getline(in_file, line)) {
      std::stringstream ss{line};
      std::string word;
      while (ss >> word) {
        auto cleaned_str = clean_string(word);
        if (!cleaned_str.empty()) {
          words[cleaned_str]++;
        }
      }
    }
    in_file.close();
    display_words(words);
  } else {
    std::cerr << "Error opening input file" << std::endl;
  }
}

// Part2 process the file and builds a map of words and a
// set of line numbers in which the word appears
void part2() {
  std::map<std::string, std::set<int>> words;
  std::ifstream in_file{"./words.txt"};
  if (in_file) {

    int curr_line{};
    std::string line{};
    while (std::getline(in_file, line)) {
      curr_line++;
      std::stringstream ss{line};
      std::string word{};
      while (ss >> word) {
        auto cleaned_str = clean_string(word);
        if (!cleaned_str.empty()) {
          words[cleaned_str].insert(curr_line);
        }
      }
    }

    in_file.close();
    display_words(words);
  } else {
    std::cerr << "Error opening input file" << std::endl;
  }
}

int main() {
  part1();
  part2();
  return 0;
}
