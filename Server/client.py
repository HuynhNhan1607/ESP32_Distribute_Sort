import socket
import threading

SERVER_IP = "192.168.2.126"  # IP server
SERVER_PORT = 5002

def process_received_data(data):
    reconstructed_array = []
    for i in range(0, len(data), 4):
        chunk = data[i:i+4]
        if len(chunk) == 4:
            number = (chunk[0] |
                      (chunk[1] << 8) |
                      (chunk[2] << 16) |
                      (chunk[3] << 24))
            reconstructed_array.append(number)
    return reconstructed_array

def client_thread():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((SERVER_IP, SERVER_PORT))
    
    try:
        client_socket.sendall(b"REQUEST 102400")
        print("Sent: REQUEST FILE")

        received_data = b""
        while True:
            chunk = client_socket.recv(1024)
            if not chunk:
                break
            received_data += chunk

        reconstructed_array = process_received_data(received_data)
        print("Reconstructed Array from thread: {}, Size: {}".format(reconstructed_array[-10:], len(reconstructed_array)))
        # Sắp xếp mảng theo chiều giảm dần
        sorted_array = sorted(reconstructed_array, reverse=True)
        print("Sorted Array (descending):", sorted_array[:10])

        # Chuyển đổi mảng đã sắp xếp thành dữ liệu nhị phân để gửi lại cho server
        data_to_send = bytearray()
        for number in sorted_array:
            data_to_send.extend(number.to_bytes(4, byteorder='little'))
        print("Size Data to Send: ", len(data_to_send))
        # Gửi dữ liệu đã sắp xếp ngược lại cho server
        client_socket.sendall(data_to_send)
        print("Sent sorted data back to server.")

    finally:
        client_socket.close()
        print("Disconnected.")

# Tạo và khởi động 5 thread client
threads = []
for i in range(1):
    thread = threading.Thread(target=client_thread)
    threads.append(thread)
    thread.start()

# Chờ tất cả các thread hoàn thành
for thread in threads:
    thread.join()