#include <WiFi.h>
#include <esp_now.h>

#define MAX_PWM 1023 //vaues above this wil be clamped


String MAC_ADDR = "34:B7:DA:5B:58:E0"; //insert MAC address of receiver
 
uint8_t broadcastAddress[] = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}; //DO not change

typedef struct struct_message {
  int red;
  int green;
  int blue;
  int white;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

String inputString = "";
bool stringComplete = false;

void OnDataSent(const esp_now_send_info_t *info, esp_now_send_status_t status) {
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "OK" : "FAIL");
}

void setup() {
  Serial.begin(115200);
  updateMacAddress(MAC_ADDR);
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_send_cb(OnDataSent);
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }

  inputString.reserve(64); 
}

void loop() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      stringComplete = true;
    } else {
      inputString += inChar;
    }
  }

  if (stringComplete) {
    inputString.trim();

    if (inputString.equalsIgnoreCase("deviceinfo")) {
      Serial.println("leddriver");
    } 
    else {
      int sep1 = inputString.indexOf(':');
      int sep2 = inputString.indexOf(':', sep1 + 1);
      int sep3 = inputString.indexOf(':', sep2 + 1);

      if (sep1 > 0 && sep2 > sep1 && sep3 > sep2) {
        // Parse and constrain the values immediately
        myData.red   = constrain(inputString.substring(0, sep1).toInt(), 0, MAX_PWM);
        myData.green = constrain(inputString.substring(sep1 + 1, sep2).toInt(), 0, MAX_PWM);
        myData.blue  = constrain(inputString.substring(sep2 + 1, sep3).toInt(), 0, MAX_PWM);
        myData.white = constrain(inputString.substring(sep3 + 1).toInt(), 0, MAX_PWM);
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        if (result != ESP_OK) {
          Serial.println("Error");
        }
      } 
    }
    inputString = "";
    stringComplete = false;
  }
}



bool updateMacAddress(String macStr) {
  // A valid MAC string "XX:XX:XX:XX:XX:XX" is exactly 17 characters
  if (macStr.length() != 17) return false;

  // sscanf needs 'unsigned int' to process hex safely before casting to bytes
  unsigned int mac[6]; 
  
  // Scans the string for 6 Hexadecimal values (%x) separated by colons
  int parsed = sscanf(macStr.c_str(), "%x:%x:%x:%x:%x:%x", 
                      &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

  if (parsed == 6) {
    // If all 6 were found, update the global broadcastAddress array
    for (int i = 0; i < 6; i++) {
      broadcastAddress[i] = (uint8_t)mac[i];
    }
    return true;
  }
  return false;
}