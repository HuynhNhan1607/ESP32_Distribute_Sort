from server_gui import ESP32ServerGUI
from server_logic import ESP32ServerLogic
from tkinter import filedialog
import os

class ESP32ServerController:
    def __init__(self):
        self.logic = ESP32ServerLogic("0.0.0.0", 5001, 5002, self.log)
        self.firmware_file = None
        self.logic.data_file = "data_1M.dat"
        self.gui = ESP32ServerGUI(self)

    def log(self, message, level = 0):
        print(message)  # In log ra terminal
        if(level):
            self.gui.log(message)

    def start_firmware_server(self):
        """Bắt đầu server firmware."""
        self.logic.start_server("Firmware")
        self.log("[INFO] Firmware Server started.")

    def start_data_server(self):
        """Bắt đầu server data."""
        self.logic.start_server("Data")
        
        self.log("[INFO] Data Server started.")

    def select_firmware(self):
        file_path = filedialog.askopenfilename(filetypes=[("Binary Files", "*.bin")])
        if file_path:
            self.logic.firmware_path = file_path
            filename = os.path.basename(file_path)
            self.gui.lbl_fw_info.config(text=f"Selected: {filename}")
            self.log(f"[INFO] Selected firmware: {filename}")

    def send_firmware(self):
        self.logic.send_firmware()

        self.log("[INFO] Sending firmware to clients...")

    def send_data(self):
        
        self.logic.send_data()
        self.log("[INFO] Sending data to clients...")

    def send_command(self, command):
        """Gửi lệnh đến các client."""
        self.logic.send_command(command)

    def run(self):
        self.gui.run()

if __name__ == "__main__":
    controller = ESP32ServerController()
    controller.run()
