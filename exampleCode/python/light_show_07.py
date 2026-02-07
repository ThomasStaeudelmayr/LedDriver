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

def morphing_dual_show():
    print("Starting ultra-smooth dual-color pendulum lightshow... 🌌")
    t = 0.0
    step = 0.01  # kleiner Schritt = feinere Bewegung

    try:
        while True:
            # Zwei Basisfarben, die sich seeeehr langsam verändern
            hue1 = (0.15 + 0.1 * math.sin(t * 0.0005)) % 1.0
            hue2 = (0.65 + 0.1 * math.cos(t * 0.0005)) % 1.0

            # Pendel: jetzt ~8 Sekunden für hin und zurück
            cycle_time = 8.0  
            mix = 0.5 + 0.5 * math.sin((2 * math.pi / cycle_time) * t)

            # Interpolierte Zwischenfarbe
            hue = (1 - mix) * hue1 + mix * hue2

            # Sanfte Helligkeit, fast ohne Flackern
            sat = 0.9
            val = 0.5 + 0.4 * (0.5 + 0.5 * math.sin(t * 0.01))

            r, g, b, w = hsv_to_rgbw(hue, sat, val)

            msg = f"{r}:{g}:{b}:{w}\n"
            ser.write(msg.encode())
            print(f"Sent → Mix({round(mix,2)}) HSV({round(hue,3)}, {round(sat,2)}, {round(val,2)}) → RGBW({r}, {g}, {b}, {w})")

            time.sleep(0.05)  # etwas längere Pause für smooth output
            t += step

    except KeyboardInterrupt:
        print("\nMorphing dual-color lightshow stopped.")
        ser.close()

if __name__ == "__main__":
    morphing_dual_show()
