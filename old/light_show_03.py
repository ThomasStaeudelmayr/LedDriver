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

def romantic_show():
    print("Starting Romantic RGBW lightshow... 💖")
    t = 0.0
    step = 0.02  # slower time step for smoother, calm feel

    try:
        while True:
            # Restrict hue range to warm romantic tones (reds, pinks, purples)
            base_hue = 0.95  # pink/purple anchor
            drift = 0.05 * math.sin(t * 0.05)  # subtle drift
            hue = (base_hue + drift) % 1.0

            # Saturation stays high, slight wave for natural feel
            sat = 0.8 + 0.15 * math.sin(t * 0.07)

            # Brightness pulses like gentle breathing
            val = 0.4 + 0.3 * (0.5 + 0.5 * math.sin(t * 0.1))

            r, g, b, w = hsv_to_rgbw(hue, sat, val)

            msg = f"{r}:{g}:{b}:{w}\n"
            ser.write(msg.encode())
            print(f"Sent → HSV({round(hue, 3)}, {round(sat, 2)}, {round(val, 2)}) → RGBW({r}, {g}, {b}, {w})")

            time.sleep(0.05)
            t += step

    except KeyboardInterrupt:
        print("\nRomantic lightshow stopped.")
        ser.close()

if __name__ == "__main__":
    romantic_show()
