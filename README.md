#ESP32 #Arduino

<img src="assets\ledDriver_cc.JPG" width="700">

This projects consists of two different hardware designs. Both are 4 channel LED drivers. The CC version can drive RGBW Power LEDs like the Cree XLamp Color Series and the CV Driver has 4 Mosfets that allow dimming of RGBW LED Strips. Both use a ESP32-S3 Zero as Microcontroller that can dim the LEDs at various frequencies.

## Hardware
### LED Driver CC

<img src="assets\ledDriver_cc_kicad.jpg" width="500">

The CC Version is built around the LM3404 Led driver IC. Its a switching regulator that drives the connected LEDs in constant current mode eliminating the need for current limiting resistors and therefore increasing efficiency. 
#### Current Sens Resistor 
The maximum current is dependent on the sens-resistors used. 

$R_{SNS} = \frac{V_{REF}}{I_F} = \frac{0.2\text{ V}}{I_F}$


#### Parts

| Part name           | Quantity | Order number       | Distibuter |
| ------------------- | -------- | ------------------ | ---------- |
| ESP32-S3-Zero       | 1        |                    |            |
| LM3404              | 4        | LM 3404 MA         | Reichelt   |
| DK_SLF10145 47uH    | 4        | 810-SLF10145T      | Mouser     |
| 0R2 2512            | 4        | 708-RNCL2512FTR200 | Mouser     |
| CP_Elec_10x10 470uF | 2        |                    |            |
| SMAJ40A             | 4        |                    |            |
| TSR_1-2490          | 1        | LME78_05-1.0       | Reichelt   |
| MCV_1,5_8-G-3.81    | 1        | 220-4888           | RS         |
| Resistor            |          |                    |            |
| Capacitor           |          |                    |            |




### LED Driver CV

<img src="assets\ledDriver_cv_kicad.jpg" width="500">

The CV Version of the Led driver uses 4 Mosfets controlled via Mosfet. It is designed to cointrol RGBW LED Strips that already have current limitation built in. 
The mosfet drivers allow fast rise times, therfore reducing the power loss while switching. 

#### Parts

| Part name           | Order number       | Distibuter |
| ------------------- | ------------------ | ---------- |
| ESP32-S3-Zero       |                    |            |
| LM3404              | LM 3404 MA         | Reichelt   |
| DK_SLF10145 47uH    | 810-SLF10145T      | Mouser     |
| 0R2 2512            | 708-RNCL2512FTR200 | Mouser     |
| CP_Elec_10x10 470uF |                    |            |
| SMAJ40A             |                    |            |
| TSR_1-2490          | LME78_05-1.0       | Reichelt   |
| MCV_1,5_8-G-3.81    | 220-4888           | RS         |
|                     |                    |            |

## Software 

### ESP Now Example 
For the ESP-NOW example to work we first need the MAC address of the receiving ESP32. For this use the get_mac_address scetch. When uploaded the esp will print its mac address over serial. 
The receivers  MAC address must be inserted into the transmitter code.
Example:
`uint8_t broadcastAddress[] = {0x34, 0xB7, 0xDA, 0x5B, 0x58, 0xE0};`

### Transmitter Code


### Receiver and Drive Code 
The Receiver code. waits for ESP-NOW data and when data is beeing received sets the pwm signals an its four channels accordingly. 

#### PWM settings
The ESP32 is capable of different frequencies and resolutions for the PWM dimming signal. 
``` cpp
#define PWM_FREQ 5000
#define PWM_RESOLUTION 10  // 10-bit (0–1023)
```

#### Sending Data to the Transmitter (Bridge)
The PC communicates via Serial with the transmitter. To the the connection just select the right com port and Baudrate in the Arduino IDE and send a string like (2048, 0, 1000, 0) to the transmitter.
A more sophisticated way to generate the RGB data is to use the web_app.example. After connection to the right com port the program will generate the RGBW String automatically depending on the selected color, saturation and luminance. 

![[Pasted image 20260512211243.png|369]]

#### Python Code 
Another way to generate the Serial data is via a python program. 
The python example will also search for the transmitters com port automatically. 


### Web Server Example
The Web Server example eliminates the need for a dedicated brisge transmitter controller. The ESP32 on the driver board will act as a webserver. 


```cpp
// --- WiFi Credentials ---
const char* ssid = "WLAN19064339";
const char* password = "SfffaddchSu5";
```


## Disclaimer 
Ai was used in the workflow of creating the scetched 