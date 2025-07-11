<!-- Use this file to provide workspace-specific custom instructions to Copilot. For more details, visit https://code.visualstudio.com/docs/copilot/copilot-customization#_use-a-githubcopilotinstructionsmd-file -->

# MCP Server C++ Project Instructions

This is a Model Context Protocol (MCP) server implementation in C++.

## Key Information

- You can find more info and examples at https://modelcontextprotocol.io/llms-full.txt
- This project implements a basic MCP server that supports the tools capability
- The server uses JSON-RPC 2.0 for communication
- Built with C++17 standard using CMake
- Uses nlohmann/json library for robust JSON handling

## Architecture

- `McpServer` class handles the main server logic and MCP protocol
- `JsonRpc` class provides JSON-RPC parsing and response generation
- Tools are registered with handlers that can be called remotely
- Current tools include: echo, get_time, and system_info

## Build System

- Uses CMake for cross-platform building
- Target C++17 standard
- Outputs executable to `build/bin/mcp_server`

## Development Guidelines

- Follow RAII principles for resource management
- Use smart pointers for memory management
- Use nlohmann/json for all JSON operations instead of manual string building
- Keep the JSON-RPC implementation simple but extensible
- Add new tools by implementing them in `setupDefaultTools()` method
