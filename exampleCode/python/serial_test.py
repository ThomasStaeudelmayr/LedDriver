import serial
import time

# Configure serial connection
ser = serial.Serial('COM9', 115200, timeout=1)
time.sleep(2)  # Wait for connection to settle

# Define messages
msg1 = "200:0:0:0"
msg2 = "0:0:0:200"



try:
    while True:
        ser.write((msg1 + '\n').encode())
        print(f"Sent: {msg1}")
        time.sleep(1)

        ser.write((msg2 + '\n').encode())
        print(f"Sent: {msg2}")
        time.sleep(1)

except KeyboardInterrupt:
    print("Stopped by user.")

finally:
    ser.close()
    print("Serial connection closed.")