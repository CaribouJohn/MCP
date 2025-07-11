#!/usr/bin/env python3
"""
Test all tools to ensure they work with the updated response format.
"""

import json
import subprocess
import sys
import time


def test_tool(tool_name, arguments=None):
    """Test a specific tool with given arguments."""

    if arguments is None:
        arguments = {}

    # Create the JSON-RPC request for tools/call
    request = {
        "jsonrpc": "2.0",
        "id": 1,
        "method": "tools/call",
        "params": {"name": tool_name, "arguments": arguments},
    }

    print(f"Testing tool: {tool_name}")
    print(f"Arguments: {json.dumps(arguments, indent=2)}")
    print("-" * 50)

    try:
        # Start the MCP server process
        process = subprocess.Popen(
            ["c:\\Development\\MCP\\build\\bin\\Debug\\mcp_server.exe"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        # Send initialization first
        init_request = {
            "jsonrpc": "2.0",
            "id": 0,
            "method": "initialize",
            "params": {
                "protocolVersion": "2024-11-05",
                "capabilities": {"tools": {}},
                "clientInfo": {"name": "test-client", "version": "1.0.0"},
            },
        }

        process.stdin.write(json.dumps(init_request) + "\n")
        process.stdin.flush()

        # Read initialization response
        init_response = process.stdout.readline()

        # Wait a moment
        time.sleep(0.1)

        # Send the tool call
        process.stdin.write(json.dumps(request) + "\n")
        process.stdin.flush()

        # Read the response
        response = process.stdout.readline()
        print(f"Response: {response.strip()}")

        # Parse and validate the response
        try:
            response_data = json.loads(response)
            print(f"Parsed response: {json.dumps(response_data, indent=2)}")

            # Check if the response is correct
            if "result" in response_data:
                result = response_data["result"]
                if "content" in result and isinstance(result["content"], list):
                    print(f"✅ Tool {tool_name} test PASSED!")
                    return True
                else:
                    print(f"❌ Tool {tool_name} test FAILED! Invalid content format")
                    return False
            else:
                print(f"❌ Tool {tool_name} test FAILED! Error: {response_data}")
                return False

        except json.JSONDecodeError as e:
            print(f"❌ Failed to parse response as JSON: {e}")
            return False

        # Close the process
        process.stdin.close()
        process.terminate()
        process.wait(timeout=5)

    except Exception as e:
        print(f"❌ Error running test: {e}")
        return False


def main():
    print("MCP Tools Test Suite")
    print("=" * 50)

    tests = [
        ("echo", {"message": "Hello World!"}),
        ("get_time", {}),
        ("system_info", {}),
    ]

    results = []

    for tool_name, arguments in tests:
        print()
        success = test_tool(tool_name, arguments)
        results.append((tool_name, success))
        print()

    print("=" * 50)
    print("Test Results Summary:")
    for tool_name, success in results:
        status = "✅ PASSED" if success else "❌ FAILED"
        print(f"  {tool_name}: {status}")

    all_passed = all(success for _, success in results)
    print(
        f"\nOverall: {'✅ ALL TESTS PASSED' if all_passed else '❌ SOME TESTS FAILED'}"
    )


if __name__ == "__main__":
    main()
