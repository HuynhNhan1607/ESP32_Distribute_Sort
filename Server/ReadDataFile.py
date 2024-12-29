import time

FILE_NAME = "received_data.dat"  # Tên file cần đọc
TEXT_FILE_NAME = "data.txt"
# Danh sách để lưu các số nguyên đã đọc
reconstructed_array = []
read_array = []
# Mở file và đọc dữ liệu
with open(FILE_NAME, "rb") as file:
    start_time = time.perf_counter()
    while True:
        # Đọc 4 byte
        chunk = file.read(4)
        if not chunk:  # Nếu không còn dữ liệu, thoát khỏi vòng lặp
            end_time = time.perf_counter()
            print(f"Time Read Binary File: {(end_time - start_time)*1000:.12f} ms")
            break
        
        # Chuyển đổi 4 byte thành số nguyên (int32)
        if len(chunk) == 4:
            number = (chunk[0] |
                      (chunk[1] << 8) |
                      (chunk[2] << 16) |
                      (chunk[3] << 24))
            # Thêm số nguyên vào danh sách
            reconstructed_array.append(number)


print(f"Reconstructed array size: {len(reconstructed_array)}")
print(f"First 10 numbers: {reconstructed_array[:10]}")  # In 10 số đầu tiên để kiểm tra
# Define the file name for reading

# Open the text file and read data
with open(TEXT_FILE_NAME, "r") as file:
    start_time = time.perf_counter()
    for line in file:
        number = int(line.strip())  # Convert each line to an integer
        read_array.append(number)
    end_time = time.perf_counter()
    print(f"Time Read Text File: {(end_time - start_time)*1000:.12f} ms")

print(f"Reconstructed array size: {len(read_array)}")
print(f"First 10 numbers: {read_array[:10]}")  # Print the first 10 numbers for verification

# Assuming original_array is available from CreateDataFile.py
# Compare the two arrays
if len(reconstructed_array) != len(read_array):
    print("The arrays are not the same: different lengths.")
else:
    if reconstructed_array == read_array:
        print("The arrays are the same.")
    else:
        print("The arrays are not the same: different contents.")