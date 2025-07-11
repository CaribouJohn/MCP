#!/usr/bin/env python3
"""
Quick echo test - minimal version that won't hang.
"""

import json
import subprocess
import sys


def quick_echo_test():
    """Test echo tool with timeout to prevent hanging."""

    try:
        # Start server with timeout
        process = subprocess.Popen(
            ["c:\\Development\\MCP\\build\\bin\\Debug\\mcp_server.exe"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
        )

        # Initialize
        init_request = {
            "jsonrpc": "2.0",
            "id": 1,
            "method": "initialize",
            "params": {
                "protocolVersion": "2024-11-05",
                "capabilities": {"tools": {}},
                "clientInfo": {"name": "quick-test", "version": "1.0.0"},
            },
        }

        print("Sending initialize...")
        process.stdin.write(json.dumps(init_request) + "\n")
        process.stdin.flush()

        # Get init response with timeout
        try:
            init_response = process.stdout.readline()
            print(f"Init response: {init_response.strip()}")
        except:
            print("❌ No init response")
            process.kill()
            return False

        # Test echo
        echo_request = {
            "jsonrpc": "2.0",
            "id": 2,
            "method": "tools/call",
            "params": {"name": "echo", "arguments": {"message": "Hello from VS Code!"}},
        }

        print("Sending echo request...")
        process.stdin.write(json.dumps(echo_request) + "\n")
        process.stdin.flush()

        # Get echo response
        try:
            echo_response = process.stdout.readline()
            print(f"Echo response: {echo_response.strip()}")

            # Parse response
            response_data = json.loads(echo_response)
            if "result" in response_data and "content" in response_data["result"]:
                content = response_data["result"]["content"]
                if isinstance(content, list) and len(content) > 0:
                    text = content[0].get("text", "")
                    if "Hello from VS Code!" in text:
                        print("✅ Echo tool working perfectly!")
                        print(f"   Response: {text}")
                        return True

        except Exception as e:
            print(f"❌ Error parsing echo response: {e}")

        # Clean up
        process.stdin.close()
        process.kill()
        process.wait()

    except Exception as e:
        print(f"❌ Error in test: {e}")
        return False

    return False


if __name__ == "__main__":
    print("Quick Echo Test")
    print("=" * 30)
    success = quick_echo_test()
    print(f"\nResult: {'✅ SUCCESS' if success else '❌ FAILED'}")
