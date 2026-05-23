#include <ESP8266WiFi.h>
#include <espnow.h>
#include <U8g2lib.h>
#include <Wire.h>

/*
COM-OLED2.42 SSD1309 - ESP8266

VDD  ->  3.3V  
VSS  ->  GND  
A0   ->  D2  (GPIO4)  
D0   ->  D5  (GPIO14 - SCK)  
D1   ->  D7  (GPIO13 - MOSI)  
CS   ->  D8  (GPIO15)  
RST  ->  D1  (GPIO5)  
*/

// Receiver MAC address
uint8_t broadcastAddress[] = {0xB4, 0xE6, 0x2D, 0x29, 0x2F, 0x1C};


typedef struct struct_message {
  int red = 0;
  int green = 0;
  int blue = 0;
  int white = 0;
} struct_message;

struct_message myData;
U8G2_SSD1309_128X64_NONAME2_1_4W_HW_SPI u8g2(U8G2_R2, /* cs=*/ 15, /* dc=*/ 4, /* reset=*/ 5);
String inputString = ""; // to store incoming data
bool stringComplete = false;

void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
  Serial.print("Last Packet Send Status: ");
  if (sendStatus == 0) {
    Serial.println("Delivery success");
  } else {
    Serial.println("Delivery fail");
  }
}

void setup() {
  Serial.begin(115200);
  u8g2.setDisplayRotation(U8G2_R0);  
  u8g2.begin();
  WiFi.mode(WIFI_STA);
  welcomeScreen();

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  inputString.reserve(64);  // reserve some memory
}

void updateDisplay(String inputString) {
  const char* labels[] = {"R", "G", "B", "W"};
  u8g2.setFont(u8g2_font_helvB10_tr);
  u8g2.firstPage();
  do {
    int y = 18; // starting y position
    int rowSpacing = 15;

    int lastIndex = 0;
    int componentIndex = 0;

    while (componentIndex < 4) {
      int nextIndex = inputString.indexOf(':', lastIndex);
      String value;

      if (nextIndex == -1) {
        value = inputString.substring(lastIndex);
      } else {
        value = inputString.substring(lastIndex, nextIndex);
      }

      String line = String(labels[componentIndex]) + ": " + value;
      u8g2.drawStr(5, y, line.c_str());
      y += rowSpacing;

      if (nextIndex == -1) break;
      lastIndex = nextIndex + 1;
      componentIndex++;
    }

  } while (u8g2.nextPage());
}

void loop() {
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
      // Expected format: r:g:b:w
      int sep1 = inputString.indexOf(':');
      int sep2 = inputString.indexOf(':', sep1 + 1);
      int sep3 = inputString.indexOf(':', sep2 + 1);

      if (sep1 > 0 && sep2 > sep1 && sep3 > sep2) {
        myData.red   = inputString.substring(0, sep1).toInt();
        myData.green = inputString.substring(sep1 + 1, sep2).toInt();
        myData.blue  = inputString.substring(sep2 + 1, sep3).toInt();
        myData.white = inputString.substring(sep3 + 1).toInt();
        esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        updateDisplay(inputString);
      }
    }

    inputString = "";
    stringComplete = false;
  }
}

void welcomeScreen(){
  u8g2.setFont(u8g2_font_helvB14_tr);
  u8g2.firstPage();
  do {
    u8g2.drawStr(5, 30, "Wireless");
    u8g2.drawStr(5, 50, "Led Driver");
  } while (u8g2.nextPage());
  delay(2000);
}