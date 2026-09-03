#include <algorithm>
#include <arpa/inet.h>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <format>
#include <iostream>
#include <netdb.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

constexpr int PORT_TO_USE = 4221;

int main(int argc, char **argv) {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  std::cout << "Logs from your program will appear here!\n";
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }

  int reuse = 1;
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) <
      0) {
    std::cerr << "setsockopt failed\n";
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT_TO_USE);

  if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) !=
      0) {
    std::cerr << "Failed to bind to port 4221\n";
    return 1;
  }

  int connection_backlog = 5;
  if (listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    return 1;
  }

  struct sockaddr_in client_addr;
  int client_addr_len = sizeof(client_addr);

  std::cout << "Waiting for a client to connect...\n";
  int client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                         (socklen_t *)&client_addr_len);

  if (client_fd < 0) {
    std::cerr << "Client Refused to Connect." << std::endl;
    return 1;
  }

  std::cout << "Client connected\n";
  char buffer[1024];
  int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
  if (bytes_received > 0) {
    buffer[bytes_received] = '\0';
  }
  std::istringstream iss(buffer);
  std::string first_line;
  std::getline(iss, first_line);
  std::istringstream first_line_iss(first_line);
  std::string method, path, version;
  first_line_iss >> method >> path >> version;

  std::string response{};
  std::string user_agent;
  std::string line;
  while (std::getline(iss, line) && line != "\r") {
    auto colon_pos = line.find(":");
    if (colon_pos != std::string::npos) {
      std::string header_name = line.substr(0, colon_pos);
      std::string header_value = line.substr(colon_pos + 1);
      
      size_t start = header_value.find_first_not_of(" \t");
      if (start != std::string::npos) {
        header_value.erase(0, start);
      } else {
        header_value.clear();
      }

      if (!header_value.empty() && header_value.back() == '\r') {
        header_value.pop_back();
      }

      std::string header_name_lower = header_name;
      std::transform(header_name_lower.begin(), header_name_lower.end(),
                     header_name_lower.begin(),
                     [](unsigned char c) { return std::tolower(c); });

      if (header_name_lower == "user-agent") {
        user_agent = header_value;
      }
    }
  }
  if (path == "/") {
    response = "HTTP/1.1 200 OK\r\n\r\n";
  } else if (path.starts_with("/user-agent")) {
    response = std::format("HTTP/1.1 200 OK\r\nContent-Type: "
                           "text/plain\r\nContent-Length: {}\r\n\r\n{}",
                           user_agent.length(), user_agent);
  } else if (path.starts_with("/echo/")) {
    std::string contents = path.substr(6);
    response = std::format("HTTP/1.1 200 OK\r\nContent-Type: "
                           "text/plain\r\nContent-Length: {}\r\n\r\n{}",
                           contents.length(), contents);
  } else {
    response = "HTTP/1.1 404 Not Found\r\n\r\n";
  }
  send(client_fd, response.c_str(), response.length(), 0);
  close(client_fd);
  close(server_fd);

  return 0;
}