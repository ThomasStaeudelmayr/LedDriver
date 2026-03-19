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

def wave_show():
    print("Starting Waves RGBW lightshow... 🌊✨")
    t = 0.0
    step = 0.03

    try:
        while True:
            # Hue drifting gently in the ocean spectrum (teal/blue/cyan)
            hue = 0.5 + 0.1 * math.sin(t * 0.01)

            # Rich color (saturation steady, slightly wavy)
            sat = 0.8 + 0.1 * math.sin(t * 0.05)

            # Two overlapping sine waves for wave motion
            wave1 = 0.5 + 0.5 * math.sin(t * 0.2)
            wave2 = 0.5 + 0.5 * math.sin(t * 0.07 + math.pi / 3)

            # Combine them for complex rolling brightness
            val = 0.3 + 0.5 * ((wave1 + wave2) / 2)

            r, g, b, w = hsv_to_rgbw(hue, sat, val)

            msg = f"{r}:{g}:{b}:{w}\n"
            ser.write(msg.encode())
            print(
                f"Waves → HSV({round(hue, 3)}, {round(sat, 2)}, {round(val, 2)}) "
                f"→ RGBW({r}, {g}, {b}, {w})"
            )

            time.sleep(step)
            t += step

    except KeyboardInterrupt:
        print("\nWaves lightshow stopped.")
        ser.close()

if __name__ == "__main__":
    wave_show()
