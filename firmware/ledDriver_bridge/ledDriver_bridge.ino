/*
 * ====================================================================
 * Project:      ESP-NOW Led Driver Bridge
 * Description:  Parses raw serial strings formatted as "R:G:B:W" and 
 *               transmits the parsed values via ESP-NOW to a led driver receiver.
 * Target Board: ESP32-S3 Zero
 * Author:       Thomas Stäudelmayr
 * Date:         May 2026
 * ====================================================================
 */

#include <WiFi.h>
#include <esp_now.h>

#define MAX_PWM 1023 //vaues above this will be clamped

// MAC Address of target ESP32-S3 receiver
uint8_t broadcastAddress[] = {0x34, 0xB7, 0xDA, 0x5B, 0x58, 0xE0};

// Structure for outgoing RGBW data (Must match receiver structure exactly)
typedef struct struct_message {
  int red;
  int green;
  int blue;
  int white;
} struct_message;

struct_message myData;
String inputString = ""; 
bool stringComplete = false;

// Callback triggered when a packet is sent
void OnDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register send callback
  esp_now_register_send_cb(OnDataSent);

  // Initialize and register peer device
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo)); 
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  inputString.reserve(64);
  Serial.println("Sender Ready. Format: R:G:B:W");
}

void loop() {
  // Read incoming bytes from the Serial Buffer
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n' || inChar == '\r') {
      if (inputString.length() > 0) {
        stringComplete = true;
      }
    } else {
      inputString += inChar;
    }
  }

  // Parse and transmit when a newline or carriage return is detected
  if (stringComplete) {
    inputString.trim();

    // Check for identification query
    if (inputString.equalsIgnoreCase("deviceinfo")) {
      Serial.println("leddriver_bridge");
    } else {
      // Find delimiters for parsing string substring chunks
      int sep1 = inputString.indexOf(':');
      int sep2 = inputString.indexOf(':', sep1 + 1);
      int sep3 = inputString.indexOf(':', sep2 + 1);

      if (sep1 > 0 && sep2 > sep1 && sep3 > sep2) {
        myData.red   = constrain(inputString.substring(0, sep1).toInt(), 0, MAX_PWM);
        myData.green = constrain(inputString.substring(sep1 + 1, sep2).toInt(), 0, MAX_PWM);
        myData.blue  = constrain(inputString.substring(sep2 + 1, sep3).toInt(), 0, MAX_PWM);
        myData.white = constrain(inputString.substring(sep3 + 1).toInt(), 0, MAX_PWM);
        // Blast data package over ESP-NOW
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        
        if (result != ESP_OK) {
          Serial.println("Error sending the data");
        }
      } else {
        Serial.println("Invalid Format! Use R:G:B:W");
      }
    }
    
    // Clear string state to receive next batch
    inputString = "";
    stringComplete = false;
  }
}