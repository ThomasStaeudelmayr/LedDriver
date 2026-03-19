import tkinter as tk
from PIL import Image, ImageTk
from colorsys import hsv_to_rgb

def generate_hue_bar(width=360, height=50):
    """Creates an image that only varies hue across the x-axis."""
    img = Image.new("RGB", (width, height))
    for x in range(width):
        hue = x / width
        r, g, b = hsv_to_rgb(hue, 1, 1)
        for y in range(height):
            img.putpixel((x, y), (int(r * 255), int(g * 255), int(b * 255)))
    return img

def get_rgb_from_hue(event):
    x = event.x
    if 0 <= x < hue_img.width:
        hue = x / hue_img.width
        sat = sat_slider.get()
        val = val_slider.get()
        r, g, b = hsv_to_rgb(hue, sat, val)
        rgb = (int(r * 255), int(g * 255), int(b * 255))
        print(f"Hue clicked: {round(hue, 3)} | Sat: {sat} | Val: {val} → RGB: {rgb}")

# --- GUI Setup ---
root = tk.Tk()
root.title("Hue Picker with Sliders")

# Hue Bar
hue_img = generate_hue_bar()
hue_photo = ImageTk.PhotoImage(hue_img)

canvas = tk.Canvas(root, width=hue_img.width, height=hue_img.height)
canvas.pack(pady=10)
canvas.create_image(0, 0, anchor=tk.NW, image=hue_photo)
canvas.bind("<Button-1>", get_rgb_from_hue)

# Saturation Slider
tk.Label(root, text="Saturation").pack()
sat_slider = tk.Scale(root, from_=0, to=1, resolution=0.01, orient=tk.HORIZONTAL, length=hue_img.width)
sat_slider.set(1)
sat_slider.pack()

# Value (Brightness) Slider
tk.Label(root, text="Value").pack()
val_slider = tk.Scale(root, from_=0, to=1, resolution=0.01, orient=tk.HORIZONTAL, length=hue_img.width)
val_slider.set(1)
val_slider.pack()

root.mainloop()
