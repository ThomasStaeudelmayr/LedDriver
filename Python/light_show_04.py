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

def techno_dynamic():
    print("Starting Dynamic Techno Bass RGBW lightshow... 🔊🌈")
    t = 0.0
    step = 0.03

    try:
        while True:
            # --- Smooth color drift (wider than before) ---
            hue = (0.6 + 0.4 * math.sin(t * 0.02)) % 1.0
            sat = 0.85 + 0.1 * math.sin(t * 0.07)

            # --- Bass tempo variation (switch every ~20s) ---
            slow_bass = 1.0  # Hz (~60 BPM)
            fast_bass = 2.5  # Hz (~150 BPM)
            tempo_mix = 0.5 * (1 + math.sin(t * 0.01))  # slowly blends over ~1 min
            bass_freq = slow_bass * (1 - tempo_mix) + fast_bass * tempo_mix

            bass_wave = 0.5 * (1 + math.sin(2 * math.pi * bass_freq * t))

            # --- Brightness dynamics ---
            ambient_val = 0.25 + 0.2 * math.sin(t * 0.15)
            energy_wave = 0.5 + 0.5 * math.sin(t * 0.005)  # big, slow "drop" wave

            if bass_wave > 0.95:  # Bass hit
                val = min(1.0, ambient_val + 0.8 * energy_wave)
            else:
                val = ambient_val * energy_wave

            r, g, b, w = hsv_to_rgbw(hue, sat, val)

            msg = f"{r}:{g}:{b}:{w}\n"
            ser.write(msg.encode())
            print(
                f"Tempo {bass_freq:.2f}Hz | Bass {bass_wave:.2f} "
                f"→ HSV({round(hue, 3)}, {round(sat, 2)}, {round(val, 2)}) "
                f"→ RGBW({r}, {g}, {b}, {w})"
            )

            time.sleep(step)
            t += step

    except KeyboardInterrupt:
        print("\nDynamic techno lightshow stopped.")
        ser.close()

if __name__ == "__main__":
    techno_dynamic()
