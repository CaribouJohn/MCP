import subprocess
import json
import time


def test_tools_list():
    """Test the tools/list method specifically"""

    server_path = r"C:\Development\MCP\build\bin\Debug\mcp_server.exe"

    try:
        process = subprocess.Popen(
            [server_path],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            text=True,
            bufsize=0,
        )

        # Send initialize first
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

        print("Sending initialize...")
        process.stdin.write(json.dumps(init_request) + "\n")
        process.stdin.flush()

        # Read initialize response
        init_response = process.stdout.readline().strip()
        print(f"Initialize response: {init_response}")

        # Now send tools/list
        tools_request = {
            "jsonrpc": "2.0",
            "id": "2",
            "method": "tools/list",
            "params": {},
        }

        print("\nSending tools/list...")
        process.stdin.write(json.dumps(tools_request) + "\n")
        process.stdin.flush()

        # Read tools response
        tools_response = process.stdout.readline().strip()
        print(f"Tools response: {tools_response}")

        if tools_response:
            try:
                tools_json = json.loads(tools_response)
                print(f"\nParsed tools response:")
                print(json.dumps(tools_json, indent=2))

                if "result" in tools_json and "tools" in tools_json["result"]:
                    tools = tools_json["result"]["tools"]
                    print(f"\nAvailable tools ({len(tools)}):")
                    for tool in tools:
                        print(f"- {tool['name']}: {tool['description']}")

            except json.JSONDecodeError as e:
                print(f"Failed to parse tools response: {e}")

    except Exception as e:
        print(f"Error: {e}")
    finally:
        try:
            process.terminate()
            process.wait(timeout=5)
        except:
            process.kill()


if __name__ == "__main__":
    print("=== Testing tools/list method ===")
    test_tools_list()
