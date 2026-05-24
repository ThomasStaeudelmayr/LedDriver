import customtkinter as ctk
from PIL import Image, ImageTk
import serial
import time
import math
import serial.tools.list_ports

# Set UI Theme styling
ctk.set_appearance_mode("Dark")
ctk.set_default_color_theme("green")

def find_led_driver_port(baudrate=115200):
    print("Searching for LED driver...")
    ports = serial.tools.list_ports.comports()
    for port in ports:
        try:
            test_ser = serial.Serial(port.device, baudrate, timeout=1, write_timeout=1)
            test_ser.setDTR(True)
            test_ser.setRTS(True)
            time.sleep(1.5) 
            test_ser.reset_input_buffer()
            test_ser.write(b"deviceinfo\n")
            time.sleep(0.2)
            
            response = test_ser.readline().decode('utf-8', errors='ignore').strip().lower()
            if response == "leddriver_bridge":
                print(f"LED driver found on {port.device}")
                return test_ser  
            test_ser.close()
        except (serial.SerialException, UnicodeDecodeError, PermissionError):
            pass
    print("LED driver not found.")
    return None

def calculate_rgbw_10bit(h_deg, saturation, luminance):
    """
    Converts HSV parameters using independent master-dimmer logic.
    Outputs true 10-bit values (0-1023).
    """
    max_pwm = 1023

    # 1. Calculate vibrant chroma from Hue angle
    h_prime = h_deg / 60
    x = 1 - abs((h_prime % 2) - 1)
    
    r_chroma, g_chroma, b_chroma = 0.0, 0.0, 0.0
    if   0 <= h_prime < 1: r_chroma, g_chroma, b_chroma = 1.0, x, 0.0
    elif 1 <= h_prime < 2: r_chroma, g_chroma, b_chroma = x, 1.0, 0.0
    elif 2 <= h_prime < 3: r_chroma, g_chroma, b_chroma = 0.0, 1.0, x
    elif 3 <= h_prime < 4: r_chroma, g_chroma, b_chroma = 0.0, x, 1.0
    elif 4 <= h_prime < 5: r_chroma, g_chroma, b_chroma = x, 0.0, 1.0
    elif 5 <= h_prime < 6: r_chroma, g_chroma, b_chroma = 1.0, 0.0, x

    # 2. Crossfade color vs white based strictly on saturation position
    r_mix = r_chroma * saturation
    g_mix = g_chroma * saturation
    b_mix = b_chroma * saturation
    w_mix = (1.0 - saturation)

    # 3. Apply the luminance slider as a pure master dimmer scale across all channels
    return (
        int(round(r_mix * luminance * max_pwm)),
        int(round(g_mix * luminance * max_pwm)),
        int(round(b_mix * luminance * max_pwm)),
        int(round(w_mix * luminance * max_pwm))
    )

def update_output(*args):
    # Fetch slider attributes
    h = hue_slider.get()
    s = sat_slider.get()
    v = val_slider.get()
    
    # Calculate 10-bit RGBW
    r, g, b, w = calculate_rgbw_10bit(h, s, v)
    
    # Format and send over serial connection
    msg = f"{r}:{g}:{b}:{w}\n"
    if ser:
        ser.write(msg.encode())
    
    # Update UI Labels
    hue_val_label.configure(text=f"{int(h)}°")
    sat_val_label.configure(text=f"{int(s*100)}%")
    val_val_label.configure(text=f"{int(v*100)}%")
    debug_display.configure(text=f"R: {r}  |  G: {g}  |  B: {b}  |  W: {w}")

# --- GUI Core Layout ---

ser = find_led_driver_port()
if ser is None:
    print("Exiting: No active hardware device discovered.")
    exit(1)

app = ctk.CTk()
app.title("10-Bit HSV Led Driver Panel")
app.geometry("420x520")
app.resizable(False, False)

# Main container card frame
main_card = ctk.CTkFrame(master=app, corner_radius=20, border_width=1, border_color="#222222")
main_card.pack(fill="both", expand=True, padx=20, pady=20)

# Title Header
title_label = ctk.CTkLabel(master=main_card, text="RGBW CONTROLLER", font=("Segoe UI", 16, "bold"), text_color="#00e676")
title_label.pack(pady=(20, 10))

# --- SLIDER 1: HUE ---
hue_frame = ctk.CTkFrame(master=main_card, fg_color="transparent")
hue_frame.pack(fill="x", padx=30, pady=10)

hue_title = ctk.CTkLabel(master=hue_frame, text="HUE SELECTION", font=("Segoe UI", 10, "bold"), text_color="#666666")
hue_title.pack(side="left")
hue_val_label = ctk.CTkLabel(master=hue_frame, text="0°", font=("Consolas", 12, "bold"), text_color="#00e676")
hue_val_label.pack(side="right")

hue_slider = ctk.CTkSlider(master=main_card, from_=0, to=359, number_of_steps=360, command=update_output)
hue_slider.set(0)
hue_slider.pack(fill="x", padx=30, pady=(0, 15))

# --- SLIDER 2: SATURATION ---
sat_frame = ctk.CTkFrame(master=main_card, fg_color="transparent")
sat_frame.pack(fill="x", padx=30, pady=10)

sat_title = ctk.CTkLabel(master=sat_frame, text="COLOR SATURATION (RGB vs W)", font=("Segoe UI", 10, "bold"), text_color="#666666")
sat_title.pack(side="left")
sat_val_label = ctk.CTkLabel(master=sat_frame, text="100%", font=("Consolas", 12, "bold"), text_color="#00e676")
sat_val_label.pack(side="right")

sat_slider = ctk.CTkSlider(master=main_card, from_=0.0, to=1.0, number_of_steps=100, command=update_output)
sat_slider.set(1.0)
sat_slider.pack(fill="x", padx=30, pady=(0, 15))

# --- SLIDER 3: VALUE / BRIGHTNESS ---
val_frame = ctk.CTkFrame(master=main_card, fg_color="transparent")
val_frame.pack(fill="x", padx=30, pady=10)

val_title = ctk.CTkLabel(master=val_frame, text="MASTER LUMINANCE (INTENSITY)", font=("Segoe UI", 10, "bold"), text_color="#666666")
val_title.pack(side="left")
val_val_label = ctk.CTkLabel(master=val_frame, text="100%", font=("Consolas", 12, "bold"), text_color="#00e676")
val_val_label.pack(side="right")

val_slider = ctk.CTkSlider(master=main_card, from_=0.0, to=1.0, number_of_steps=100, command=update_output)
val_slider.set(1.0)
val_slider.pack(fill="x", padx=30, pady=(0, 25))

# --- DATA READOUT TERMINAL ---
debug_display = ctk.CTkLabel(master=main_card, text="R: 0  |  G: 0  |  B: 0  |  W: 0",
                             font=("Consolas", 14, "bold"), text_color="#00e676",
                             fg_color="#000000", height=45, corner_radius=8)
debug_display.pack(fill="x", padx=30, pady=15)

# Initialize application layout state variables
update_output()

app.mainloop()