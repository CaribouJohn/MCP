#include "stdio_adapter.h"

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

StdioAdapter::StdioAdapter() {
  logFile_.open("bridge_stdio.log", std::ios::app);
  if (logFile_.is_open()) {
    logFile_ << "[CONNECT] VS Code stdio bridge established" << std::endl;
    logFile_.flush();
  }
}

StdioAdapter::~StdioAdapter() {
  if (logFile_.is_open()) logFile_.close();
}

void StdioAdapter::log(const std::string& direction, const std::string& msg) {
  if (!logFile_.is_open()) return;
  auto now = std::chrono::system_clock::now();
  std::time_t now_c = std::chrono::system_clock::to_time_t(now);
  std::tm tm;
#ifdef _WIN32
  localtime_s(&tm, &now_c);
#else
  localtime_r(&now_c, &tm);
#endif
  logFile_ << std::put_time(&tm, "%Y-%m-%d %H:%M:%S") << " [" << direction
           << "] " << msg << std::endl;
  logFile_.flush();
}

bool StdioAdapter::readMessage(std::string& message) {
  if (!std::getline(std::cin, message)) {
    return false;
  }
  log("IN", message);
  return true;
}

bool StdioAdapter::writeMessage(const std::string& message) {
  std::cout << message << std::endl;
  log("OUT", message);
  return std::cout.good();
}
