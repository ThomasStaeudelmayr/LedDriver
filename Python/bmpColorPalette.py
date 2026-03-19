from PIL import Image
import numpy as np

# Beispielpalette: 255 Farben (hier nur Dummy, du musst deine eigene definieren)
palette = []
for i in range(255):
    palette.extend([i, 255-i, (i*37) % 256])  # Beispiel-Farbverlauf
palette.extend([0, 0, 0])  # 256. Farbe schwarz als Platzhalter

# Bild laden
img = Image.open("input.bmp").convert("RGB")
arr = np.array(img)

# Palette als numpy Array für schnelleren Vergleich
palette_arr = np.array(palette).reshape(-1, 3)

# Pixel zuordnen
reshaped = arr.reshape(-1, 3)
indices = []
for px in reshaped:
    dist = np.sum((palette_arr - px) ** 2, axis=1)
    indices.append(np.argmin(dist))
indices = np.array(indices, dtype=np.uint8).reshape(arr.shape[:2])

# Neues Bild mit Palette anlegen
pal_img = Image.fromarray(indices, mode="P")
pal_img.putpalette(palette)

pal_img.save("output.bmp")