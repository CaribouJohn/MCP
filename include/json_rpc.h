#pragma once

#include <string>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class JsonRpc
{
public:
    JsonRpc();
    ~JsonRpc();

    // Parse incoming JSON-RPC request
    bool parseRequest(const std::string &jsonStr, std::string &method,
                      json &params, std::string &id);

    // Create JSON-RPC response
    std::string createResponse(const std::string &id, const json &result);
    std::string createErrorResponse(const std::string &id, int errorCode,
                                    const std::string &errorMessage);

    // Create JSON-RPC notification
    std::string createNotification(const std::string &method,
                                   const json &params);

    // Utility methods (keeping for backwards compatibility)
    static std::string escapeJson(const std::string &str);
    static std::string createJsonObject(const std::map<std::string, std::string> &data);
    static std::string createJsonArray(const std::vector<std::string> &items);

private:
    // Legacy parsing helpers (will be replaced with nlohmann/json)
    std::string extractValue(const std::string &json, const std::string &key);
    std::map<std::string, std::string> parseParams(const std::string &paramsJson);
};
