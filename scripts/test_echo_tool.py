#!/usr/bin/env python3
"""
Test script to specifically test the echo tool via tools/call request.
This will help verify that the echo tool is working correctly.
"""

import json
import subprocess
import sys
import time


def test_echo_tool():
    """Test the echo tool with a tools/call request."""

    # Test message for echo
    test_message = "Hello from echo tool test!"

    # Create the JSON-RPC request for tools/call
    request = {
        "jsonrpc": "2.0",
        "id": 1,
        "method": "tools/call",
        "params": {"name": "echo", "arguments": {"message": test_message}},
    }

    print(f"Testing echo tool with message: '{test_message}'")
    print(f"Sending request: {json.dumps(request, indent=2)}")
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

        print("Sending initialize request...")
        process.stdin.write(json.dumps(init_request) + "\n")
        process.stdin.flush()

        # Read initialization response
        init_response = process.stdout.readline()
        print(f"Initialize response: {init_response.strip()}")

        # Wait a moment
        time.sleep(0.1)

        # Send the echo tool call
        print("Sending echo tool call...")
        process.stdin.write(json.dumps(request) + "\n")
        process.stdin.flush()

        # Read the response
        response = process.stdout.readline()
        print(f"Echo tool response: {response.strip()}")

        # Parse and validate the response
        try:
            response_data = json.loads(response)
            print(f"Parsed response: {json.dumps(response_data, indent=2)}")

            # Check if the response is correct
            if "result" in response_data:
                result = response_data["result"]
                if "content" in result:
                    content = result["content"]
                    if isinstance(content, list) and len(content) > 0:
                        text_content = content[0].get("text", "")
                        if test_message in text_content:
                            print("✅ Echo tool test PASSED!")
                            print(
                                f"   Expected message found in response: '{test_message}'"
                            )
                        else:
                            print("❌ Echo tool test FAILED!")
                            print(
                                f"   Expected message '{test_message}' not found in response"
                            )
                    else:
                        print("❌ Echo tool test FAILED!")
                        print("   Response content is not in expected format")
                else:
                    print("❌ Echo tool test FAILED!")
                    print("   No 'content' field in result")
            else:
                print("❌ Echo tool test FAILED!")
                print(f"   Error in response: {response_data}")

        except json.JSONDecodeError as e:
            print(f"❌ Failed to parse response as JSON: {e}")
            print(f"   Raw response: {response}")

        # Close the process
        process.stdin.close()
        process.terminate()
        process.wait(timeout=5)

    except Exception as e:
        print(f"❌ Error running test: {e}")
        return False

    return True


if __name__ == "__main__":
    print("Echo Tool Test")
    print("=" * 50)
    test_echo_tool()
