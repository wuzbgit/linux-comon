import socket
import json

SOCKET_PATH = "/tmp/cam.sock"

request = {
    "module": "sensor",
    "submodule": "framerate",
    "action": "set",
    "fps": "30"
}

def main():
    try:
        client = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        client.connect(SOCKET_PATH)

        message = json.dumps(request)
        client.sendall(message.encode('utf-8'))

        # 接收响应
        response = client.recv(4096)
        if response:
            try:
                data = json.loads(response.decode('utf-8'))
                print("响应内容:")
                print(json.dumps(data, indent=4, ensure_ascii=False))
            except json.JSONDecodeError:
                print("返回数据非JSON格式：", response.decode('utf-8'))

    except socket.error as e:
        print("Socket错误:", e)

    finally:
        client.close()

if __name__ == "__main__":
    main()
