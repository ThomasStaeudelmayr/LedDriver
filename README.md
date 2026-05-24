<img src="assets\ledDriver_cc.JPG" width="700">

This repository contains two distinct 4-channel LED driver hardware designs controlled by an **ESP32-S3 Zero**. Both versions support pulse-width modulation (PWM) dimming.

- **Constant Current (CC) Driver:** Designed for high-power LEDs (like the Cree XLamp Color Series) that require current regulation.
    
- **Constant Voltage (CV) Driver:** Features 4 power MOSFETs designed for dimming standard 12V/24V RGBW LED strips.

## Hardware
### LED Driver CC

<img src="assets\ledDriver_cc_kicad.JPG" width="500">

The CC version is built around the **LM3404** LED driver IC. This is a buck (step-down) switching regulator that drives the connected LEDs in constant current mode, eliminating the need for current-limiting resistors and increasing system efficiency.
#### Current Sense Resistor Calculation
The maximum forward current ($I_F$) delivered to the LEDs is determined by the value of the sense resistor ($R_{SNS}$) connected to the `CS` pin. Use the following formula to select your resistor value:

$$R_{SNS} = \frac{V_{REF}}{I_F} = \frac{0.2\text{ V}}{I_F}$$

Using a 0.2 $\Omega$ resistor sets the maximum forward current to exactly 1.0 A.

#### Parts
| Part name                 | Quantity | Order number       | Distibuter |
| ------------------------- | -------- | ------------------ | ---------- |
| ESP32-S3-Zero             | 1        | B0GV459V87         | Amazon     |
| LM3404                    | 4        | LM 3404 MA         | Reichelt   |
| DK_SLF10145 47uH          | 4        | 810-SLF10145T      | Mouser     |
| 0R2 2512                  | 4        | 708-RNCL2512FTR200 | Mouser     |
| SMAJ40A                   | 4        | 652-SMAJ40A        | Mouser     |
| DCDC Converter            | 1        | LME78_05-1.0       | Reichelt   |
| MCV_1,5_8-G-3.81          | 1        | 220-4888           | RS         |
| Fuse 5A                   | 1        | 576-0454005.NR     | Mouser     |
| MCV_1,5_8-G-3.81          | 1        | 220-4888           | RS         |
| MSTBVA 2-G 5,08           | 1        | STBVA 2-G 5,08     | Reichelt   |
| Capacitor 470uF 10x10 25V | 2        | -                  | -          |
| Capacitor 470nF 1210      | 4        | -                  | -          |
| Capacitor 2.2uF  1206     | 4        | -                  | -          |
| Capacitor 100nF 1206      | 8        | -                  | -          |
| Capacitor 10uF 1210       | 6        | -                  | -          |
| Resistor 536k             | 4        | -                  | -          |

### LED Driver CV

<img src="assets\ledDriver_cv_kicad.JPG" width="500">

Designed for standard 12V/24V RGBW LED strips with integrated current-limiting resistors, utilizing 4 power MOSFETs as low-side switches. Dedicated gate drivers rapidly charge the MOSFET gate capacitance. The resulting fast rise and fall times minimize switching power losses during high-frequency PWM operation.
#### Parts
| Part name            | Quantity | Order number         | Distibuter |
| -------------------- | -------- | -------------------- | ---------- |
| ESP32-S3-Zero        | 1        | B0GV459V87           | Amazon     |
| MCV_1,5_5-G-3.81     | 1        | 651-1803455          | Mouser     |
| MSTBVA 2-G 5,08      | 1        | STBVA 2-G 5,08       | Reichelt   |
| Capacitor 470uF      | 2        | FR-A 470U 63B        | Reichelt   |
| SMAJ40A              | 5        | 652-SMAJ40A          | Mouser     |
| Tantal 7343 100uF    | 2        | 80-T495D107M16ATE060 | Mouser     |
| 5V LDO               | 1        | 511-LD1117S50        | Mouser     |
| 9V DCDC Converter    | 1        | TSRN 1-2490          | Reichelt   |
| MCP1416 SOT-23-5     | 4        | 579-MCP1416RTEOTVAO  | Mouser     |
| Mosfet STD80N6F6     | 4        | 511-STD80N6F6        | Mouser     |
| Capacitor 10uF 1210  | 4        | -                    | -          |
| Capacitor 100nF 0805 | 4        | -                    | -          |
| Resistor 10R 0805    | 4        | -                    | -          |

## Software 

### ESP-NOW Control Example

The PC sends color values over a USB serial connection to the **Led Driver Bridge**, which then wirelessly sends those values to the **Led Driver Receiver**. 
##### 1. Get the Receiver's MAC Address
To pair the devices, you need the unique MAC address of the receiver board.
1. Upload the **Receiver Code** to your LED driver board.
2. Open the Serial Monitor. On startup, the ESP32 will print its MAC address.
3. Copy this address; you will need it for the transmitter code.
    
##### 2. Transmitter Configuration & Operation
The transmitter connects to your PC via USB, listens for incoming serial data, and forwards it wirelessly.
1. Open the **Transmitter Code**.
2. Replace the placeholder bytes with your receiver's MAC address
3. Upload the code to the transmitter ESP32. 

```cpp
// Example: Replace with your actual receiver MAC address
uint8_t broadcastAddress[] = {0x34, 0xB7, 0xDA, 0x5B, 0x58, 0xE0};
```

#### Sending Control Commands 
The bridge ESP expects a single string formatted as `R:G:B:W` followed by a newline character (`\n`).  

**Example:** `1024:255:150:360` 

You can type this manually into the Arduino Serial Monitor or automate it using a program. This repository contains a 2 python and one html example that generate the rgbw values. Once the transmitter gets a valid message, it splits the string into individual color values and sends them out over ESP-NOW. 

**LedDriver_web_app_rgbw.html** - control leds via a browser 
**LedDriver_control_hsv.py** - control leds via a python script 
**LedDriver_rgbw_waves.py** - python script that generates led animation 

<img src="assets\web_app.jpg" width="500">

#### How to execute Python Scripts 
1. In folder open cmd 
2. generate environment (python -m venv env)
3. open environment via activate.bat
4. install modules using pip
5. execute 

### Web Server Example
The Web Server example eliminates the need for a dedicated transmitter controller. The ESP32 on the driver board will act as a webserver. After altering the wifi name and password and uploading the code the leds can be controlled on: http://ledcontrol.local



