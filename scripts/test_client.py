#!/usr/bin/env python3
"""
Simple test client for the MCP C++ server.
This demonstrates how an MCP client would interact with the server.
"""

import json


def create_mcp_request(method, params=None, request_id="1"):
    """Create a JSON-RPC 2.0 request for MCP."""
    request = {"jsonrpc": "2.0", "id": request_id, "method": method}
    if params:
        request["params"] = params
    return json.dumps(request, indent=2)


def main():
    print("=== MCP C++ Server Test Client ===\n")

    # Test 1: Initialize request
    print("1. Initialize Request:")
    init_request = create_mcp_request(
        "initialize",
        {
            "protocolVersion": "2024-11-05",
            "capabilities": {"tools": {}},
            "clientInfo": {"name": "test-client", "version": "1.0.0"},
        },
    )
    print(init_request)
    print()

    # Test 2: List tools request
    print("2. List Tools Request:")
    list_tools_request = create_mcp_request("tools/list", {})
    print(list_tools_request)
    print()

    # Test 3: Call echo tool
    print("3. Call Echo Tool:")
    echo_request = create_mcp_request(
        "tools/call",
        {"name": "echo", "arguments": {"message": "Hello from MCP client!"}},
    )
    print(echo_request)
    print()

    # Test 4: Call get_time tool
    print("4. Call Get Time Tool:")
    time_request = create_mcp_request(
        "tools/call", {"name": "get_time", "arguments": {}}
    )
    print(time_request)
    print()

    # Test 5: Call system_info tool
    print("5. Call System Info Tool:")
    info_request = create_mcp_request(
        "tools/call", {"name": "system_info", "arguments": {}}
    )
    print(info_request)
    print()

    print("=== Instructions ===")
    print("The C++ MCP server currently demonstrates request/response formatting.")
    print("In a real implementation, you would:")
    print("1. Send these JSON requests to the server via stdin/stdout")
    print("2. Read the JSON responses from the server")
    print("3. Parse and handle the responses appropriately")
    print("\nThe server shows sample responses for initialize and tools/list requests.")


if __name__ == "__main__":
    main()
