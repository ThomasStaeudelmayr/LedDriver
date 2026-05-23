import serial.tools.list_ports
import time


def find_led_driver_port(baudrate=115200, timeout=1):
    print("Searching for LED driver...")
    ports = serial.tools.list_ports.comports()
    for port in ports:
        try:
            test_ser = serial.Serial(port.device, baudrate, timeout=timeout)
            time.sleep(0.5)
            test_ser.reset_input_buffer()
            test_ser.write(b"deviceinfo\n")
            time.sleep(0.2)
            response = test_ser.readline().decode().strip().lower()
            if response == "leddriver":
                print(f"LED driver found on {port.device}")
                return test_ser  # ✅ Return already-opened instance
            test_ser.close()
        except (serial.SerialException, UnicodeDecodeError, PermissionError) as e:
            print(f"Skipped {port.device}: {e}")
    print("LED driver not found.")
    return None



ser = find_led_driver_port()

if ser is None:
    print("Exiting: No LED driver found.")
    exit(1)