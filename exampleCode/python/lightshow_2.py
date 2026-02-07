import serial
import time
import math
import random
from colorsys import hsv_to_rgb

# --- Serial Setup ---
ser = serial.Serial('COM9', 115200, timeout=1)
time.sleep(2)

def hsv_to_rgbw(h, s, v):
    r, g, b = hsv_to_rgb(h, s, v)
    r, g, b = [int(c * 255) for c in (r, g, b)]
    w = min(r, g, b)
    r -= w
    g -= w
    b -= w
    return (r, g, b, w)

def send_rgbw(r, g, b, w):
    msg = f"{r}:{g}:{b}:{w}\n"
    ser.write(msg.encode())
    print(f"Sent → RGBW({r}, {g}, {b}, {w})")

# --- Effects ---

def effect_rainbow(t):
    hue = (t * 0.05) % 1.0
    s = 1.0
    v = 1.0
    return hsv_to_rgbw(hue, s, v)

def effect_strobe(t):
    on = int(t * 10) % 2 == 0  # 5 Hz strobe
    if on:
        return (255, 255, 255, 0)
    else:
        return (0, 0, 0, 0)

def effect_pulse(t):
    v = 0.5 + 0.5 * math.sin(t * 2)
    return hsv_to_rgbw(0.0, 1.0, v)  # red pulse

def effect_color_flash(t):
    if int(t * 4) % 2 == 0:
        h = random.random()
        return hsv_to_rgbw(h, 1, 1)
    else:
        return (0, 0, 0, 0)

def effect_white_wave(t):
    v = 0.5 + 0.5 * math.sin(t * 5)
    w = int(v * 255)
    return (0, 0, 0, w)

def effect_calm_cycle(t):
    hue = (t * 0.01) % 1.0
    v = 0.3 + 0.2 * math.sin(t)
    return hsv_to_rgbw(hue, 0.6, v)

# List of effects
effects = [
    effect_rainbow,
    effect_strobe,
    effect_pulse,
    effect_color_flash,
    effect_white_wave,
    effect_calm_cycle
]

# --- Main Loop ---
def lightshow():
    print("Starting multi-effect lightshow!")
    t = 0.0
    step = 0.03
    effect_duration = 10  # seconds
    effect_index = 0
    next_switch = time.time() + effect_duration

    try:
        while True:
            # Switch effects every few seconds
            if time.time() >= next_switch:
                effect_index = (effect_index + 1) % len(effects)
                next_switch = time.time() + effect_duration
                print(f"\nSwitching to effect: {effects[effect_index].__name__}")

            # Run current effect
            r, g, b, w = effects[effect_index](t)
            send_rgbw(r, g, b, w)

            time.sleep(step)
            t += step

    except KeyboardInterrupt:
        print("\nLightshow stopped.")
        ser.close()

if __name__ == "__main__":
    lightshow()
