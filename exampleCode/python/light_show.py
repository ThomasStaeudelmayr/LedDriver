import serial
import time
import math
from colorsys import hsv_to_rgb

# Configure serial connection
ser = serial.Serial('COM9', 115200, timeout=1)
time.sleep(2)  # Wait for the serial connection to settle

def hsv_to_rgbw(h, s, v):
    """Convert HSV to RGBW, where white = min(R, G, B)."""
    r, g, b = hsv_to_rgb(h, s, v)
    r, g, b = [int(c * 255) for c in (r, g, b)]
    w = min(r, g, b)
    r -= w
    g -= w
    b -= w
    return (r, g, b, w)

def lightshow():
    print("Starting RGBW lightshow...")
    t = 0.0
    step = 0.05  # time step

    try:
        while True:
            # Smooth hue cycling
            hue = (t * 0.05) % 1.0

            # Pulsing saturation and brightness using sine waves
            sat = 0.5 + 0.5 * math.sin(t * 0.3)
            val = 0.5 + 0.5 * math.sin(t * 0.4 + math.pi / 4)

            r, g, b, w = hsv_to_rgbw(hue, sat, val)

            msg = f"{r}:{g}:{b}:{w}\n"
            ser.write(msg.encode())
            print(f"Sent → HSV({round(hue, 3)}, {round(sat, 2)}, {round(val, 2)}) → RGBW({r}, {g}, {b}, {w})")

            time.sleep(0.03)
            t += step

    except KeyboardInterrupt:
        print("\nLightshow stopped.")
        ser.close()

if __name__ == "__main__":
    lightshow()
