import serial
import time
import random

# --- Serial Setup ---
ser = serial.Serial('COM9', 115200, timeout=1)
time.sleep(2)

def send_rgbw(r, g, b, w):
    msg = f"{r}:{g}:{b}:{w}\n"
    ser.write(msg.encode())
    print(f"Sent → RGBW({r}, {g}, {b}, {w})")

def fire_effect():
    """Fire effect with warm tones and reduced color shift."""
    # Base intensity with subtle flicker
    base = random.uniform(0.6, 1.0)
    flicker = random.uniform(0.85, 1.05)

    r = int(base * flicker * 255)
    g = int(base * flicker * 120)  # reduced green component
    b = 0

    # Rare white flicker for ember pop
    w = random.randint(0, 8)
    if random.random() < 0.05:
        w += random.randint(10, 20)

    # Clamp values
    r = max(100, min(r, 255))
    g = max(40, min(g, 100))
    w = min(w, 30)

    return (r, g, b, w)

# --- Main Loop ---
def fire_loop():
    print("🔥 Smooth warm fire effect started.")
    step = 0.1  # 100ms

    try:
        while True:
            r, g, b, w = fire_effect()
            send_rgbw(r, g, b, w)
            time.sleep(step)

    except KeyboardInterrupt:
        print("\n🔥 Fire effect stopped.")
        ser.close()

if __name__ == "__main__":
    fire_loop()
