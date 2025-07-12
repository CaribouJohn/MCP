#pragma once
#include <string>

// Abstract interface for message transport (stdin/stdout, TCP, etc.)
class ITransportAdapter {
 public:
  virtual ~ITransportAdapter() = default;
  // Reads a message from the transport. Returns false on EOF or error.
  virtual bool readMessage(std::string& message) = 0;
  // Writes a message to the transport. Returns false on error.
  virtual bool writeMessage(const std::string& message) = 0;
};
