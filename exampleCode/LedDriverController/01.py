

import time
import serial
import serial.tools.list_ports as port_list
import tkinter as tk
import random
import colorsys


def change_color():
    ser.flush
    data = ser.readline().decode('ascii').strip()  
    #data = ser.readline().decode('utf-8').strip()
    #print(data)
    try:
        encoderValue = int(data)
    except:
        encoderValue = 0

    steps = 4096
    hue = encoderValue/steps


    r,g,b = hsl_to_rgb(hue, 1, 0.5)

    if(r>255 or r<0): r = 0
    if(g>255 or g<0): g = 0
    if(b>255 or b<0): b = 0

    new_color = "#{:02x}{:02x}{:02x}".format(r, g, b)
    window.configure(bg=new_color)
    window.after(10, change_color)


def downscale_12bit_to_8bit(value_12bit):
    max_value_12bit = 4095  # Maximum value for 12 bits
    max_value_8bit = 255    # Maximum value for 8 bits
    factor = max_value_12bit / max_value_8bit
    scaled_value_8bit = round(value_12bit / factor)


def downscale(Lmax,Smax,value):
    max_value_12bit = 4095  # Maximum value for 12 bits
    max_value_8bit = 255    # Maximum value for 8 bits
    factor = Lmax / Smax
    out = round(value / factor)
    return out


def getport():
    ports = list(port_list.comports())
    devicefound = 0

    for p in ports: 
        s = str(p)
        i = s.find('Every')
        if(i != -1): 
            devicefound = 1
            f = s.rindex(" - Arduino NANO Every")
            gg = s[0:f]
            print("connecting to: ",gg)
    return gg


def hsl_to_rgb(h, s, l):
    # h, s, l should be in the range [0.0, 1.0]
    r, g, b = colorsys.hls_to_rgb(h, l, s)
    
    # Convert to the range [0, 255]
    r = int(r * 255)
    g = int(g * 255)
    b = int(b * 255)
    
    return r, g, b

ser = serial.Serial(getport(), baudrate = 115200, timeout=2)
window = tk.Tk()
window.title("Color Changing Window")
window.geometry("300x200")

change_color()

# Run the Tkinter event loop
window.mainloop()
#collapse all STRG+K than STRG+0




