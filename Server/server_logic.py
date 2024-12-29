import socket
import threading
import time

class ESP32ServerLogic:
    BUFFER_SIZE = 1024

    def __init__(self, host, port_upgrade, port_data, logger):
        self.host = host
        self.port_upgrade = port_upgrade
        self.port_data = port_data
        self.logger = logger
        self.current_server = None
        self.data_ready_to_send = False
        self.clients_data = []
        self.clients_upgrade = []
        self.data_offset = 0  # Vị trí hiện tại của file data
        self.processed_clients = set()
        self.lock = threading.Lock()
        self.data_file = None

    def start_server(self, server_type):
        """Bật server cho firmware hoặc data, chỉ một server chạy cùng lúc."""
        self.stop_current_server()
        if server_type == "Firmware":
            threading.Thread(target=self._start_upgrade_server, daemon=True).start()
            self.data_ready_to_send = False
            self.data_offset = 0
        elif server_type == "Data":
            threading.Thread(target=self._start_data_server, daemon=True).start()

        self.current_server = server_type
        self.logger(f"[INFO] {server_type} Server started.")

    def stop_current_server(self):
        """Dừng server hiện tại nếu có."""
        if self.current_server:
            self.logger(f"[INFO] Stopping {self.current_server} Server...")
        self.current_server = None
        self.data_ready_to_send = False
        self.data_offset = 0

    def send_firmware(self):
        if self.current_server != "Firmware" or not self.firmware_path:
            self.logger("[ERROR] Firmware Server is not running or no firmware selected.")
            return
        self.logger("[INFO] Sending firmware to clients...")
        with self.lock:
            for client in self.clients_upgrade:
                threading.Thread(target=self._send_file, args=(client, self.firmware_path)).start()

    def send_data(self):
        if self.current_server != "Data":
            self.logger("[ERROR] Data Server is not running.")
            return
        self.logger("[INFO] Data Server ready. Waiting for client requests...")
        self.data_ready_to_send = True

    def send_command(self, command):
        if self.current_server is None:
            self.logger("[ERROR] No server is running.")
            return

        self.logger(f"[INFO] Sending command to clients: {command}")
        with self.lock:
            for client in self.clients_data:  # Gửi đến cả hai loại client
                try:
                    client.sendall(command.encode())
                    client.shutdown(socket.SHUT_WR)
                    self.logger(f"[INFO] Send Done")
                except Exception as e:
                    self.logger(f"[ERROR] Failed to send command to client: {e}")

    def _handle_request(self, client_socket):
        """Xử lý request dữ liệu từ client."""
        # received_sorted_event = threading.Event()
        client_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
        try:

            request = client_socket.recv(1024).decode().strip()
            self.logger(f"[INFO] Received request: {request}")

            if request.startswith("REQUEST"):
                size = int(request.split()[1])  # Kích thước client yêu cầu
                self.logger(f"[INFO] Client requested {size} bytes of data.")

                # Kiểm tra file data đã sẵn sàng chưa
                if not self.data_ready_to_send or not self.data_file:
                    self.logger("[INFO] Waiting for data file to be specified...")
                    while not self.data_ready_to_send:
                        time.sleep(0.1) 

                # Đọc dữ liệu từ file và gửi cho client
                with open(self.data_file, "rb") as f, self.lock:
                    f.seek(self.data_offset)
                    data_chunk = f.read(size)  # Đọc dữ liệu đúng với kích thước yêu cầu
                    if not data_chunk:
                        self.logger("[INFO] No more data to send.")
                        client_socket.sendall(b"END")  # Gửi tín hiệu kết thúc
                    else:
                        client_socket.sendall(data_chunk)  # Gửi dữ liệu
                        self.logger(f"[INFO] Sent {len(data_chunk)} bytes to client.")
                        self.data_offset += len(data_chunk)  # Cập nhật offset
                        # received_sorted_event.set()
                        client_socket.shutdown(socket.SHUT_WR)
                        
            else:
                self.logger(f"[ERROR] Invalid request: {request}")
                client_socket.sendall(b"ERROR: Invalid request")
                
            # received_sorted_event.wait()    
            # Nhận dữ liệu từ client
            data = b""
            while True:
                chunk = client_socket.recv(1024)
                if not chunk:
                    break
                data += chunk

            self.logger(f"[INFO] Received data from client: {len(data)}")

            self.logger(f"[INFO] Received data: {data}")
            # Ghi dữ liệu vào file
            with open("received_data.dat", "ab") as f:  # Mở file để ghi
                f.write(data)  # Ghi dữ liệu vào file

            self.logger(b"Data received and saved.")  # Gửi phản hồi cho client
        except Exception as e:
            self.logger(f"[ERROR] Error handling request: {e}")
        finally:
            client_socket.close()

    def _send_file(self, client_socket, firmware_path):
        """Send the firmware file to the client."""
        try:
            with open(firmware_path, "rb") as firmware_file:
                while True:
                    data = firmware_file.read(1024)  # Read in chunks of 1024 bytes
                    if not data:
                        break  # End of file
                    client_socket.sendall(data)  # Send data to the client
            self.logger("[INFO] Firmware sent successfully.")
        except Exception as e:
            self.logger(f"[ERROR] Failed to send firmware: {e}")
        finally:
            client_socket.close()  # Close the client socket after sending
    def _start_upgrade_server(self):
        self._start_server(self.port_upgrade, self.clients_upgrade, "Firmware")

    def _start_data_server(self):
        self._start_server(self.port_data, self.clients_data, "Data")
        

    def _start_server(self, port, client_list, name):
        server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1) 
        server_socket.bind((self.host, port))
        server_socket.listen(5)
        self.logger(f"[INFO] {name} Server listening on port {port}")
        while True:
            client, addr = server_socket.accept()
            client.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)
            with self.lock:
                client_list.append(client)
            self.logger(f"[INFO] New client connected: {addr}", 1)
            if name == "Data":
                threading.Thread(target=self._handle_request, args=(client,), daemon=True).start()

