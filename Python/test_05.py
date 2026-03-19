import tkinter as tk
from PIL import Image, ImageTk
from colorsys import hsv_to_rgb
import serial
import time
import math
import serial.tools.list_ports



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

def generate_color_wheel(diameter=300):
    """Generates a circular hue color wheel image."""
    radius = diameter // 2
    img = Image.new("RGB", (diameter, diameter), color=(0, 0, 0))
    for y in range(diameter):
        for x in range(diameter):
            dx, dy = x - radius, y - radius
            distance = math.hypot(dx, dy)
            if distance <= radius:
                angle = math.atan2(dy, dx)
                hue = (angle + math.pi) / (2 * math.pi)
                r, g, b = hsv_to_rgb(hue, 1, 1)
                img.putpixel((x, y), (int(r * 255), int(g * 255), int(b * 255)))
    return img

def hsv_to_rgbw(h, s, v):
    """Converts HSV to RGBW (white = min of RGB)."""
    r, g, b = hsv_to_rgb(h, s, v)
    r, g, b = [int(c * 255) for c in (r, g, b)]
    w = min(r, g, b)
    r -= w
    g -= w
    b -= w
    return (r, g, b, w)

def update_output():
    h = current_hue.get()
    s = sat_slider.get()
    v = val_slider.get()
    r, g, b, w = hsv_to_rgbw(h, s, v)
    msg = f"{r}:{g}:{b}:{w}\n"
    ser.write(msg.encode())
    print(f"HSV: ({round(h, 3)}, {s}, {v}) → RGBW: ({r}, {g}, {b}, {w})")

def get_rgb_from_wheel(event):
    x, y = event.x, event.y
    dx = x - wheel_radius
    dy = y - wheel_radius
    distance = math.hypot(dx, dy)
    if distance <= wheel_radius:
        angle = math.atan2(dy, dx)
        hue = (angle + math.pi) / (2 * math.pi)
        current_hue.set(hue)
        update_output()

# --- GUI Setup ---

ser = find_led_driver_port()

if ser is None:
    print("Exiting: No LED driver found.")
    exit(1)

time.sleep(2)


root = tk.Tk()
root.title("Circular Hue Picker with RGBW Output")

current_hue = tk.DoubleVar(value=0.0)

# Hue Wheel
wheel_diameter = 300
wheel_radius = wheel_diameter // 2
hue_img = generate_color_wheel(wheel_diameter)
hue_photo = ImageTk.PhotoImage(hue_img)

canvas = tk.Canvas(root, width=wheel_diameter, height=wheel_diameter)
canvas.pack(pady=10)
canvas.create_image(0, 0, anchor=tk.NW, image=hue_photo)
canvas.bind("<Button-1>", get_rgb_from_wheel)

# Saturation Slider
tk.Label(root, text="Saturation").pack()
sat_slider = tk.Scale(root, from_=0, to=1, resolution=0.01, orient=tk.HORIZONTAL, length=wheel_diameter, command=lambda x: update_output())
sat_slider.set(1)
sat_slider.pack()

# Value (Brightness) Slider
tk.Label(root, text="Value").pack()
val_slider = tk.Scale(root, from_=0, to=1, resolution=0.01, orient=tk.HORIZONTAL, length=wheel_diameter, command=lambda x: update_output())
val_slider.set(1)
val_slider.pack()

root.mainloop()
