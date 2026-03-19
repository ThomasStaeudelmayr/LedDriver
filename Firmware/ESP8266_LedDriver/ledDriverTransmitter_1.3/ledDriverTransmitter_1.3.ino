#include <ESP8266WiFi.h>
#include <espnow.h>
#include <U8g2lib.h>
#include <Wire.h>

/*
  OLED Display Wiring (2.42" SSD1309 SPI) to ESP8266:

  OLED    →  ESP8266
  VDD     →  3.3V  
  VSS     →  GND  
  A0 (DC) →  D2 (GPIO4)  
  D0 (SCK)→  D5 (GPIO14)  
  D1 (MOSI)→ D7 (GPIO13)  
  CS      →  D8 (GPIO15)  
  RST     →  D1 (GPIO5)  
*/

// MAC address of the receiving ESP8266 device
uint8_t broadcastAddress[] = {0xB4, 0xE6, 0x2D, 0x29, 0x2F, 0x1C};

// Structure for sending RGBW LED data
typedef struct struct_message {
  int red = 0;
  int green = 0;
  int blue = 0;
  int white = 0;
} struct_message;

struct_message rgbwData;

// Initialize OLED display (rotation: 180° flip for some OLEDs)
U8G2_SSD1309_128X64_NONAME2_1_4W_HW_SPI u8g2(U8G2_R2, /* CS= */15, /* DC=*/4, /* RESET= */5);

// Input buffer
String inputString = "";
bool stringComplete = false;

// Callback after ESP-NOW message is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  Serial.println(sendStatus == 0 ? "Delivery success" : "Delivery fail");
}

void setup() {
  Serial.begin(115200);
  
  // Initialize OLED
  u8g2.setDisplayRotation(U8G2_R0);  
  u8g2.begin();
  startupScreen();

  // Set Wi-Fi to station mode and initialize ESP-NOW
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register ESP-NOW sender
  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);

  // Reserve memory for incoming data
  inputString.reserve(64);
}

// Display RGBW values on OLED
void updateDisplay() {
  u8g2.setFont(u8g2_font_helvB10_tr);
  u8g2.firstPage();
  do {
    u8g2.drawStr(5, 18,  ("R: " + String(myData.red)).c_str());
    u8g2.drawStr(5, 33,  ("G: " + String(myData.green)).c_str());
    u8g2.drawStr(5, 48,  ("B: " + String(myData.blue)).c_str());
    u8g2.drawStr(5, 63,  ("W: " + String(myData.white)).c_str());
  } while (u8g2.nextPage());
}

// Handle serial input and send data
void loop() {
  // Read incoming serial characters
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
      break;
    } else {
      inputString += inChar;
    }
  }

  if (stringComplete) {
    inputString.trim();

    if (inputString.equalsIgnoreCase("deviceinfo")) {
      Serial.println("leddriver");
    } else {
      // Parse RGBW string: format expected as r:g:b:w
      int sep1 = inputString.indexOf(':');
      int sep2 = inputString.indexOf(':', sep1 + 1);
      int sep3 = inputString.indexOf(':', sep2 + 1);

      if (sep1 > 0 && sep2 > sep1 && sep3 > sep2) {
        rgbwData.red   = inputString.substring(0, sep1).toInt();
        rgbwData.green = inputString.substring(sep1 + 1, sep2).toInt();
        rgbwData.blue  = inputString.substring(sep2 + 1, sep3).toInt();
        rgbwData.white = inputString.substring(sep3 + 1).toInt();

        // Send data over ESP-NOW
        esp_now_send(broadcastAddress, (uint8_t *) &rgbwData, sizeof(rgbwData));

        // Update OLED display
        updateDisplay(inputString);
      }
    }
    inputString = "";
    stringComplete = false;
  }
}

// StartupScreen
void startupScreen() {
  u8g2.setFont(u8g2_font_helvB14_tr);
  u8g2.firstPage();
  do {
    u8g2.drawStr(5, 30, "Wireless");
    u8g2.drawStr(5, 50, "Led Driver");
  } while (u8g2.nextPage());
}
