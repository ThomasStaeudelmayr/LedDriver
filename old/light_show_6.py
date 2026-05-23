import serial
import time
import math
from colorsys import hsv_to_rgb

# Configure serial connection
ser = serial.Serial('COM6', 115200, timeout=1)
time.sleep(2)

def hsv_to_rgbw(h, s, v):
    """Convert HSV to RGBW, where white = min(R, G, B)."""
    r, g, b = hsv_to_rgb(h, s, v)
    r, g, b = [int(c * 255) for c in (r, g, b)]
    w = min(r, g, b)
    r -= w
    g -= w
    b -= w
    return (r, g, b, w)

def rainbow_show():
    print("Starting Rainbow RGBW lightshow... 🌈")
    t = 0.0
    step = 0.02  # smaller = slower sweep

    try:
        while True:
            # Full hue cycle (0..1) progressing smoothly
            hue = (t * 0.01) % 1.0  

            # Saturation steady but gently waving for variation
            sat = 0.9 + 0.1 * math.sin(t * 0.05)

            # Brightness pulsing like calm breathing
            val = 0.5 + 0.4 * (0.5 + 0.5 * math.sin(t * 0.07))

            r, g, b, w = hsv_to_rgbw(hue, sat, val)

            msg = f"{r}:{g}:{b}:{w}\n"
            ser.write(msg.encode())
            print(f"Sent → HSV({round(hue, 3)}, {round(sat, 2)}, {round(val, 2)}) → RGBW({r}, {g}, {b}, {w})")

            time.sleep(0.05)
            t += step

    except KeyboardInterrupt:
        print("\nRainbow lightshow stopped.")
        ser.close()

if __name__ == "__main__":
    rainbow_show()
