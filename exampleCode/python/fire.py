import serial
import time
import random
import math

# --- Serial Setup ---
ser = serial.Serial('COM9', 115200, timeout=1)
time.sleep(2)

def send_rgbw(r, g, b, w):
    msg = f"{r}:{g}:{b}:{w}\n"
    ser.write(msg.encode())
    print(f"Sent → RGBW({r}, {g}, {b}, {w})")

def fire_effect(t):
    """Simulates a warm fire flicker using red/orange tones."""
    # Base flicker brightness
    intensity = 0.4 + 0.4 * random.random()
    flicker = 0.2 * math.sin(t * 15 + random.uniform(-1, 1))  # add dynamic movement

    # RGB components with warm palette
    r = int((intensity + flicker) * 255)
    g = int((intensity + flicker * 0.5) * 100)
    b = 0
    w = int((intensity * 0.1 + random.random() * 0.05) * 255)

    # Clamp values
    r = max(0, min(r, 255))
    g = max(0, min(g, 80))
    b = 0
    w = max(0, min(w, 30))

    return (r, g, b, w)

# --- Main Loop ---
def fire_loop():
    print("Starting 🔥 fire effect!")
    t = 0.0
    step = 0.10

    try:
        while True:
            r, g, b, w = fire_effect(t)
            send_rgbw(r, g, b, w)
            time.sleep(step)
            t += step

    except KeyboardInterrupt:
        print("\nFire effect stopped.")
        ser.close()

if __name__ == "__main__":
    fire_loop()
