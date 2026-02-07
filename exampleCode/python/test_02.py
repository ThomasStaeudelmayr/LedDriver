import tkinter as tk
from PIL import Image, ImageTk
from colorsys import hsv_to_rgb
import serial
import time

# Configure serial connection
ser = serial.Serial('COM9', 115200, timeout=1)
time.sleep(2)  # Wait for connection to settle

def generate_hue_bar(width=360, height=50):
    """Creates an image that only varies hue across the x-axis."""
    img = Image.new("RGB", (width, height))
    for x in range(width):
        hue = x / width
        r, g, b = hsv_to_rgb(hue, 1, 1)
        for y in range(height):
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
    """Called on slider change or hue click."""
    h = current_hue.get()
    s = sat_slider.get()
    v = val_slider.get()
    r, g, b, w = hsv_to_rgbw(h, s, v)
    msg = str(r)  + ':' + str(g) + ':' + str(b)  + ':' +str(w) + '\n'
    ser.write((msg).encode())
    print(f"HSV: ({round(h, 3)}, {s}, {v}) → RGBW: ({r}, {g}, {b}, {w})")

def get_rgb_from_hue(event):
    x = event.x
    if 0 <= x < hue_img.width:
        h = x / hue_img.width
        current_hue.set(h)
        update_output()

# --- GUI Setup ---
root = tk.Tk()
root.title("Hue Picker with RGBW Output")

current_hue = tk.DoubleVar(value=0.0)

# Hue Bar
hue_img = generate_hue_bar()
hue_photo = ImageTk.PhotoImage(hue_img)

canvas = tk.Canvas(root, width=hue_img.width, height=hue_img.height)
canvas.pack(pady=10)
canvas.create_image(0, 0, anchor=tk.NW, image=hue_photo)
canvas.bind("<Button-1>", get_rgb_from_hue)

# Saturation Slider
tk.Label(root, text="Saturation").pack()
sat_slider = tk.Scale(root, from_=0, to=1, resolution=0.01, orient=tk.HORIZONTAL, length=hue_img.width, command=lambda x: update_output())
sat_slider.set(1)
sat_slider.pack()

# Value (Brightness) Slider
tk.Label(root, text="Value").pack()
val_slider = tk.Scale(root, from_=0, to=1, resolution=0.01, orient=tk.HORIZONTAL, length=hue_img.width, command=lambda x: update_output())
val_slider.set(1)
val_slider.pack()

root.mainloop()
