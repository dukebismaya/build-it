#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

class FileWrapper {
private:
  std::string file_name;
  std::string mode;
  std::FILE *file{};

public:
  FileWrapper(const std::string &file_name, const std::string &mode)
      : file_name{file_name}, mode{mode} {
    file = std::fopen(file_name.c_str(), mode.c_str());
    if (!file)
      throw std::runtime_error("Failed to open file:" + file_name);
  }

  FileWrapper(const FileWrapper &) = delete;
  FileWrapper &operator=(const FileWrapper &) = delete;

  FileWrapper(FileWrapper &&other) noexcept
      : file_name{std::move(other.file_name)}, mode{std::move(other.mode)},
        file(other.file) {
    other.file = nullptr;
    std::cout << "File handle moved via constructor.\n";
  }

  FileWrapper &operator=(FileWrapper &&other) noexcept {
    if (this != &other) {
      if (file) {
        std::fclose(file);
      }
      file_name = std::move(other.file_name);
      mode = std::move(other.mode);
      file = other.file;

      other.file = nullptr;
      std::cout << "File handled moved via assignment.\n";
    }
    return *this;
  }

  void write(const std::string &text) {
    if (file)
      std::fputs(text.c_str(), file);
  }
  ~FileWrapper() {
    if (file) {
      std::fclose(file);
      std::cout << "File closed successfully!" << std::endl;
    }
  }
};