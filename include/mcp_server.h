#pragma once

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Forward declarations
class JsonRpc;

struct McpCapabilities
{
    bool tools = false;
    bool resources = false;
    bool prompts = false;
    bool logging = false;
};

struct McpServerInfo
{
    std::string name;
    std::string version;
    McpCapabilities capabilities;
};

struct McpTool
{
    std::string name;
    std::string description;
    json inputSchema; // Changed from map to json for better schema support
};

class McpServer
{
public:
    McpServer(const std::string &name, const std::string &version);
    ~McpServer();

    // Core MCP methods
    void initialize();
    void start();
    void stop();

    // MCP protocol handlers
    void handleInitialize(const std::string &request, std::string &response);
    void handleListTools(const std::string &request, std::string &response);
    void handleCallTool(const std::string &request, std::string &response);
    void handlePing(const std::string &request, std::string &response);
    
    // Request processing
    void processRequest(const std::string &request, std::string &response);

    // Tool management
    void addTool(const McpTool &tool, std::function<json(const json &)> handler);

    // Server info
    McpServerInfo getServerInfo() const { return serverInfo_; }

private:
    McpServerInfo serverInfo_;
    std::unique_ptr<JsonRpc> jsonRpc_;
    std::map<std::string, McpTool> tools_;
    std::map<std::string, std::function<json(const json &)>> toolHandlers_;

    bool running_;

    void setupDefaultTools();
    void processRequest(const std::string &request);
};
