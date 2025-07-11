#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

// Base class for all MCP request handlers
class McpRequestHandler {
 public:
  virtual ~McpRequestHandler() = default;
  // Handle a request and produce a response
  virtual void handle(const json& request, std::string& response) = 0;
};
