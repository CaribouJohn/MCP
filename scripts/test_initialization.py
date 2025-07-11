import subprocess
import json
import time


def test_mcp_server_initialization():
    """Test the MCP server initialization specifically"""

    # Path to the MCP server executable
    server_path = r"C:\Development\MCP\build\bin\Debug\mcp_server.exe"

    try:
        # Start the server process
        print("Starting MCP server...")
        process = subprocess.Popen(
            [server_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=0,  # Unbuffered
        )

        # Send initialize request
        init_request = {
            "jsonrpc": "2.0",
            "id": "1",
            "method": "initialize",
            "params": {
                "protocolVersion": "2024-11-05",
                "capabilities": {"tools": {}},
                "clientInfo": {"name": "test-client", "version": "1.0.0"},
            },
        }

        print("Sending initialize request...")
        print(f"Request: {json.dumps(init_request, indent=2)}")

        # Send the request
        process.stdin.write(json.dumps(init_request) + "\n")
        process.stdin.flush()

        # Wait for response
        print("Waiting for response...")
        try:
            response = process.stdout.readline().strip()
            print(f"Response: {response}")

            if response:
                # Try to parse the response
                try:
                    response_json = json.loads(response)
                    print(f"Parsed response: {json.dumps(response_json, indent=2)}")
                    return True
                except json.JSONDecodeError as e:
                    print(f"Failed to parse response as JSON: {e}")
                    return False
            else:
                print("No response received")
                return False

        except Exception as e:
            print(f"Error reading response: {e}")
            return False

    except Exception as e:
        print(f"Error running server: {e}")
        return False
    finally:
        try:
            process.terminate()
            process.wait(timeout=5)
        except:
            process.kill()


if __name__ == "__main__":
    print("=== MCP Server Initialization Test ===")
    success = test_mcp_server_initialization()
    print(f"Test {'PASSED' if success else 'FAILED'}")
