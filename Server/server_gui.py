import tkinter as tk
from tkinter import LabelFrame


class ESP32ServerGUI:
    def __init__(self, controller):
        self.controller = controller
        self.root = tk.Tk()
        self.root.title("ESP32 Server Control")
        self.root.configure(bg="#E6F7FF")  # Nền chủ đạo xanh nhạt
        self.root.geometry("1200x600")
        self.root.resizable(False, False)

        # Trạng thái server
        self.server_states = {"Firmware": False, "Data": False}
        self.create_gui()

    def create_gui(self):
        font_title = ("Arial", 20, "bold")
        font_mode = ("Arial", 16, "bold")
        font_button = ("Arial", 12, "bold")
        font_label = ("Arial", 12)
        font_log = ("Bahnschrift", 12)
        # Header
        tk.Label(self.root, text="ESP32 Server Control", font=font_title, bg="#00539C", fg="white", pady=10).pack(fill="x")

        # Main Frame
        main_frame = tk.Frame(self.root, bg="#E6F7FF")
        main_frame.pack(pady=20, padx=20, fill="both", expand=True)

        main_frame.columnconfigure(0, weight=1)  # Chia đều không gian cho cột 0
        main_frame.columnconfigure(1, weight=1)  # Chia đều không gian cho cột 1

        # Firmware Upgrade Frame
        fw_frame = LabelFrame(main_frame, bg="#D9F1FF", bd=2, relief="groove")
        fw_frame.grid(row=0, column=0, padx=20, pady=10, sticky="nsew")

        tk.Label(fw_frame, text="Firmware Upgrade", font=font_mode, bg="#87CEEB", fg="black", pady=5).pack(fill="x")

        self.btn_fw_server = tk.Button(fw_frame, text="Start Firmware Server", font=font_button, width=22,
                                    bg="#FF0000", fg="white", command=self.toggle_fw_server)
        self.btn_fw_server.pack(pady=10)

        self.btn_select_fw = tk.Button(fw_frame, text="Select Firmware", font=font_button, width=22,
                                    bg="#4682B4", fg="white", command=self.controller.select_firmware)
        self.btn_select_fw.pack(pady=10)

        self.lbl_fw_info = tk.Label(fw_frame, text="No file selected", font=font_label, bg="#D9F1FF", fg="black")
        self.lbl_fw_info.pack(pady=5)

        self.btn_send_fw = tk.Button(fw_frame, text="Send Firmware", font=font_button, width=22,
                                    bg="#32CD32", fg="white", command=self.controller.send_firmware)
        self.btn_send_fw.pack(pady=10)

        # Data Transfer Frame
        data_frame = LabelFrame(main_frame, bg="#D9F1FF", bd=2, relief="groove")
        data_frame.grid(row=0, column=1, padx=20, pady=10, sticky="nsew")

        tk.Label(data_frame, text="Data Transfer", font=font_mode, bg="#87CEEB", fg="black", pady=5).pack(fill="x")

        self.btn_data_server = tk.Button(data_frame, text="Start Data Server", font=font_button, width=22,
                                        bg="#FF0000", fg="white", command=self.toggle_data_server)
        self.btn_data_server.pack(pady=10)

        self.lbl_data_info = tk.Label(data_frame, text=f"Data File: {self.controller.logic.data_file}", font=font_label, bg="#D9F1FF", fg="black")
        self.lbl_data_info.pack(pady=5)

        self.btn_send_data = tk.Button(data_frame, text="Send Data", font=font_button, width=22,
                                    bg="#32CD32", fg="white", command=self.controller.send_data)
        self.btn_send_data.pack(pady=10)

        # Tạo khung con để chứa ô nhập command và nút send command
        command_frame = tk.Frame(data_frame, bg="#D9F1FF")
        command_frame.pack(pady=10)  # Đặt khung con với khoảng cách

        # Thêm ô nhập command vào khung con
        self.command_entry = tk.Entry(command_frame, font=font_label, width=30)
        self.command_entry.pack(side="left", padx=(0, 10))  # Đặt ô nhập command bên trái

        # Thêm nút send command vào khung con
        self.btn_send_command = tk.Button(command_frame, text="Send Command", font=font_button, width=12,
                                        bg="#32CD32", fg="white", command=self.send_command)
        self.btn_send_command.pack(side="left")  # Đặt nút send command bên phải

        # Nút Reset nằm ở giữa và dưới hai frame
        self.btn_reset = tk.Button(main_frame, text="Reset Servers", font=font_button, width=22,
                                    bg="#FFA500", fg="white", command=self.reset_servers)
        self.btn_reset.grid(row=1, column=0, columnspan=2, pady=5)  # Đặt nút Reset ở giữa và dưới hai frame

        # Logs Section
        log_frame = LabelFrame(self.root, text="Logs", font=font_log, bg="#D9F1FF", fg="black", padx=10, pady=5, bd=2, relief="groove")
        log_frame.pack(fill="both", expand=True, padx=20, pady=5)

        self.log_text = tk.Text(log_frame, font=font_log, bg="white", fg="black", height=20)  # Tăng chiều cao
        self.log_text.pack(fill="both", expand=True)

        # Thêm thanh cuộn cho phần log
        scrollbar = tk.Scrollbar(log_frame, command=self.log_text.yview)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.log_text.config(yscrollcommand=scrollbar.set)

    def toggle_fw_server(self):
        """Chuyển trạng thái Firmware Server."""
        if not self.server_states["Firmware"]:
            self.server_states["Firmware"] = True
            self.controller.start_firmware_server()
            self.btn_fw_server.config(bg="#32CD32", text="Firmware Server Running")

        # Tắt Data Server
        self.server_states["Data"] = False
        self.btn_data_server.config(bg="#FF0000", text="Start Data Server")

    def reset_servers(self):
        """Reset cả hai server về trạng thái khởi tạo ban đầu."""
        self.server_states["Firmware"] = False
        self.server_states["Data"] = False
        self.btn_fw_server.config(bg="#FF0000", text="Start Firmware Server")
        self.btn_data_server.config(bg="#FF0000", text="Start Data Server")
        self.controller.logic.stop_current_server()  # Dừng server hiện tại
        self.log("Servers have been reset to initial state.")

    def send_command(self):
        command = self.command_entry.get()
        if command:
            self.controller.send_command(command)
            self.log(f"Sent command: {command}")
            self.command_entry.delete(0, tk.END) 

    def toggle_data_server(self):
        """Chuyển trạng thái Data Server."""
        if not self.server_states["Data"]:
            self.server_states["Data"] = True
            self.controller.start_data_server()
            self.btn_data_server.config(bg="#32CD32", text="Data Server Running")

        # Tắt Firmware Server
        self.server_states["Firmware"] = False
        self.btn_fw_server.config(bg="#FF0000", text="Start Firmware Server")

    def log(self, message):
        """Hiển thị log trong cửa sổ Logs."""
        self.log_text.insert(tk.END, message + "\n")
        self.log_text.see(tk.END)

    def run(self):
        self.root.mainloop()
