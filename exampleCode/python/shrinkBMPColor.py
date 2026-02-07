from PIL import Image
import sys

def shrink_palette(input_file, output_file):
    # Open the BMP image
    img = Image.open(input_file)

    # Convert to 'P' mode with max 255 colors (quantization)
    img = img.convert("P", palette=Image.ADAPTIVE, colors=100)

    # Save back as BMP
    img.save(output_file, format="BMP")

    # Show the result
    img.show()

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage: python shrink_palette.py input.bmp output.bmp")
    else:
        shrink_palette(sys.argv[1], sys.argv[2])