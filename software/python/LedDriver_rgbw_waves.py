import serial
import time
import math
import serial.tools.list_ports

def find_led_driver_port(baudrate=115200):
    print("Searching for LED driver...")
    ports = serial.tools.list_ports.comports()
    for port in ports:
        try:
            test_ser = serial.Serial(port.device, baudrate, timeout=1, write_timeout=1)
            test_ser.setDTR(True)
            test_ser.setRTS(True)
            time.sleep(1.5) 
            test_ser.reset_input_buffer()
            test_ser.write(b"deviceinfo\n")
            time.sleep(0.2)
            
            response = test_ser.readline().decode('utf-8', errors='ignore').strip().lower()
            if response == "leddriver_bridge":
                print(f"Connected to LED driver on {port.device}")
                return test_ser  
            test_ser.close()
        except (serial.SerialException, UnicodeDecodeError, PermissionError):
            pass
    return None

# Initialize Connection
ser = find_led_driver_port()
if ser is None:
    print("Error: Could not find your ESP32 Transmitter. Check USB cable connection.")
    exit(1)

print("\n--- Starting Color Waves ---")
print("Press Ctrl+C in your terminal to stop the program.\n")

# Wave Animation Variables
step = 0.0
speed = 0.03       # Higher numbers = faster color transitions
max_pwm = 1023     # Target 10-bit ceiling

try:
    while True:
        # Generate independent offset sine waves (-1.0 to 1.0) scaled up to 10-bit amplitudes
        # Math offsets (like + 2.0 or + 4.0) prevent the channels from peaking at the exact same moment
        r = int((math.sin(step) + 1) / 2 * max_pwm)
        g = int((math.sin(step + 2.0) + 1) / 2 * max_pwm)
        b = int((math.sin(step + 4.0) + 1) / 2 * max_pwm)
        
        # Make the White LED pulse slowly on an alternate cadence for ambient background filling
        w = int((math.cos(step * 0.5) + 1) / 2 * max_pwm)

        # Pack and transmit payload
        payload = f"{r}:{g}:{b}:{w}\n"
        ser.write(payload.encode())
        
        # Print monitoring log directly to terminal
        print(f"Sending Data Flow -> R: {r:<4} | G: {g:<4} | B: {b:<4} | W: {w:<4}", end="\r")
        
        # Control animation pacing loop rate (approx 50 updates per second)
        step += speed
        time.sleep(0.02)

except KeyboardInterrupt:
    # Graceful shutdown code on exit
    print("\n\nStopping script... Turning off LEDs.")
    ser.write(b"0:0:0:0\n")
    ser.close()