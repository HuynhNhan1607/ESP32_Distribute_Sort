import struct
import random

# Define constants
ARRAY_SIZE = 1000000
RANDOM_RANGE = (0, 100000)  # Range of values from 0 to 100000
BINARY_FILE_NAME = "data_1M.dat"
TEXT_FILE_NAME = "data_1M.txt"

# 1. Create an array of random integers
array = [random.randint(RANDOM_RANGE[0], RANDOM_RANGE[1]) for _ in range(ARRAY_SIZE)]

print(array)
# 2. Write array to binary file
with open(BINARY_FILE_NAME, "wb") as binary_file:
    for number in array:
        binary_file.write(struct.pack("i", number))

# 3. Write array to text file
with open(TEXT_FILE_NAME, "w") as text_file:
    for number in array:
        text_file.write(f"{number}\n")

print(f"Successfully wrote {ARRAY_SIZE} random integers to {BINARY_FILE_NAME} and {TEXT_FILE_NAME}")