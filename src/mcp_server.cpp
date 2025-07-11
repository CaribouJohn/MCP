#include "mcp_server.h"

#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "handlers/call_tool_handler.h"
#include "handlers/initialize_handler.h"
#include "handlers/list_tools_handler.h"
#include "handlers/ping_handler.h"
#include "json_rpc.h"
#include "mcp_logger.h"

McpServer::McpServer(const std::string &name, const std::string &version)
    : running_(false) {
  serverInfo_.name = name;
  serverInfo_.version = version;
  serverInfo_.capabilities.tools = true;
  serverInfo_.capabilities.resources = false;
  serverInfo_.capabilities.prompts = false;
  serverInfo_.capabilities.logging = true;

  jsonRpc_ = std::make_unique<JsonRpc>();
}

McpServer::~McpServer() { stop(); }

void McpServer::initialize() {
  spdlog::info("Initializing MCP server: " + serverInfo_.name + " v" +
               serverInfo_.version);
  setupDefaultTools();
  spdlog::info("Server initialization complete. Tools registered: " +
               std::to_string(tools_.size()));
}

void McpServer::start() {
  running_ = true;
  // Server is now ready to process requests via stdin/stdout
}

void McpServer::stop() { running_ = false; }

// Global request history buffer (file scope)
static std::vector<std::string> requestHistory;

void McpServer::processRequest(const std::string &request,
                               std::string &response) {
  spdlog::debug("Processing request: " + request.substr(0, 100) +
                (request.length() > 100 ? "..." : ""));

  // Store request in history (keep last 10)
  requestHistory.push_back(request);
  if (requestHistory.size() > 10) requestHistory.erase(requestHistory.begin());

  std::string method, id;
  json params;

  if (jsonRpc_->parseRequest(request, method, params, id)) {
    spdlog::info("Parsed request - Method: " + method + ", ID: " + id);

    if (method == "initialize") {
      handleInitialize(request, response);
    } else if (method == "tools/list") {
      handleListTools(request, response);
    } else if (method == "tools/call") {
      handleCallTool(request, response);
    } else if (method == "ping") {
      handlePing(request, response);
    } else {
      spdlog::warn("Unknown method: " + method);
      response = jsonRpc_->createErrorResponse(id, -32601,
                                               "Method not found: " + method);
    }
  } else {
    spdlog::error("Failed to parse JSON-RPC request: " + request);
    response = jsonRpc_->createErrorResponse("", -32700, "Parse error");
  }
}

void McpServer::handleInitialize(const std::string &request,
                                 std::string &response) {
  spdlog::info("Handling initialize request");

  std::string method, id;
  json params;

  if (jsonRpc_->parseRequest(request, method, params, id)) {
    // Create initialize response
    json result = {
        {"protocolVersion", "2024-11-05"},
        {"serverInfo",
         {{"name", serverInfo_.name}, {"version", serverInfo_.version}}},
        {"capabilities",
         {{"tools", serverInfo_.capabilities.tools},
          {"logging", serverInfo_.capabilities.logging}}}};

    response = jsonRpc_->createResponse(id, result);
    spdlog::info("Initialize response created successfully");
  } else {
    spdlog::error("Failed to parse initialize request");
    response = jsonRpc_->createErrorResponse(id, -32700, "Parse error");
  }
}

void McpServer::handleListTools(const std::string &request,
                                std::string &response) {
  std::string method, id;
  json params;

  if (jsonRpc_->parseRequest(request, method, params, id)) {
    json toolsArray = json::array();

    for (const auto &[name, tool] : tools_) {
      json toolObj = {{"name", tool.name},
                      {"description", tool.description},
                      {"inputSchema", tool.inputSchema}};
      toolsArray.push_back(toolObj);
    }

    json result = {{"tools", toolsArray}};

    response = jsonRpc_->createResponse(id, result);
  } else {
    response = jsonRpc_->createErrorResponse(id, -32700, "Parse error");
  }
}

void McpServer::handleCallTool(const std::string &request,
                               std::string &response) {
  spdlog::info("Handling tools/call request");

  std::string method, id;
  json params;

  if (jsonRpc_->parseRequest(request, method, params, id)) {
    std::string toolName = params["name"];
    json arguments =
        params.contains("arguments") ? params["arguments"] : json::object();

    spdlog::info("Tool call request - name: " + toolName +
                 ", arguments: " + arguments.dump());

    if (toolHandlers_.find(toolName) != toolHandlers_.end()) {
      try {
        spdlog::info("Calling tool: " + toolName);
        json result = toolHandlers_[toolName](arguments);

        // Format result according to MCP specification
        // content should be an array of content objects
        json contentArray = json::array();
        json textContent = {
            {"type", "text"},
            {"text",
             result.is_string() ? result.get<std::string>() : result.dump()}};
        contentArray.push_back(textContent);

        json resultObj = {{"content", contentArray}};
        response = jsonRpc_->createResponse(id, resultObj);
        spdlog::info("Tool call completed successfully: " + toolName);
      } catch (const std::exception &e) {
        spdlog::error("Tool call failed: " + toolName + " - " + e.what());
        response = jsonRpc_->createErrorResponse(id, -32603, e.what());
      }
    } else {
      spdlog::error("Tool not found: " + toolName);
      response = jsonRpc_->createErrorResponse(id, -32601,
                                               "Tool not found: " + toolName);
    }
  } else {
    spdlog::error("Failed to parse tools/call request");
    response = jsonRpc_->createErrorResponse(id, -32700, "Parse error");
  }
}

void McpServer::handlePing(const std::string &request, std::string &response) {
  std::string method, id;
  json params;

  if (jsonRpc_->parseRequest(request, method, params, id)) {
    json result = {{"status", "pong"}};
    response = jsonRpc_->createResponse(id, result);
  } else {
    response = jsonRpc_->createErrorResponse(id, -32700, "Parse error");
  }
}

void McpServer::addTool(const McpTool &tool,
                        std::function<json(const json &)> handler) {
  tools_[tool.name] = tool;
  toolHandlers_[tool.name] = handler;
}

void McpServer::setupDefaultTools() {
  // Add a simple "echo" tool
  McpTool echoTool;
  echoTool.name = "echo";
  echoTool.description = "Echoes back the input message";
  echoTool.inputSchema = {
      {"type", "object"},
      {"properties",
       {{"message",
         {{"type", "string"}, {"description", "The message to echo back"}}}}},
      {"required", {"message"}}};

  addTool(echoTool, [](const json &params) -> json {
    std::string message = params.value("message", "");
    if (!message.empty()) {
      return "Echo: " + message;
    }
    return "Echo: (no message provided)";
  });

  // Add a "time" tool
  McpTool timeTool;
  timeTool.name = "get_time";
  timeTool.description = "Returns the current system time";
  timeTool.inputSchema = {{"type", "object"}, {"properties", json::object()}};

  addTool(timeTool, [](const json &) -> json {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return "Current time: " + ss.str();
  });

  // Add a "system_info" tool
  McpTool systemTool;
  systemTool.name = "system_info";
  systemTool.description = "Returns basic system information";
  systemTool.inputSchema = {{"type", "object"}, {"properties", json::object()}};

  addTool(systemTool, [this](const json &) -> json {
    json info = {{"server_name", serverInfo_.name},
                 {"server_version", serverInfo_.version},
                 {"tools_available", tools_.size()},
                 {"capabilities",
                  {{"tools", serverInfo_.capabilities.tools},
                   {"logging", serverInfo_.capabilities.logging}}}};
    return info;
  });

  // Add a "context" tool to return recent requests (simple in-memory history)
  McpTool contextTool;
  contextTool.name = "context";
  contextTool.description =
      "Returns the most recent MCP requests processed by the server (for "
      "debugging).";
  contextTool.inputSchema = {{"type", "object"},
                             {"properties", json::object()}};

  // Add a simple history buffer (store last 10 requests)
  addTool(contextTool, [](const json &) -> json {
    json arr = json::array();
    for (const auto &req : requestHistory) arr.push_back(req);
    return json{{"recent_requests", arr}};
  });
}
