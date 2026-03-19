import tkinter as tk
from PIL import Image, ImageTk

def get_rgb(event):
    x, y = event.x, event.y
    if 0 <= x < image.width and 0 <= y < image.height:
        r, g, b = image.getpixel((x, y))
        print(f"Clicked RGB: ({r}, {g}, {b})")

# Generate a color gradient image (HSV to RGB)
def generate_color_gradient(width=360, height=100):
    from colorsys import hsv_to_rgb
    img = Image.new("RGB", (width, height))
    for x in range(width):
        for y in range(height):
            hue = x / width
            sat = 1
            val = 1 - y / height
            r, g, b = hsv_to_rgb(hue, sat, val)
            img.putpixel((x, y), (int(r * 255), int(g * 255), int(b * 255)))
    return img

# Setup GUI
root = tk.Tk()
root.title("Always-Visible Color Picker")

image = generate_color_gradient()
photo = ImageTk.PhotoImage(image)

canvas = tk.Canvas(root, width=image.width, height=image.height)
canvas.pack()
canvas.create_image(0, 0, anchor=tk.NW, image=photo)
canvas.bind("<Button-1>", get_rgb)

root.mainloop()