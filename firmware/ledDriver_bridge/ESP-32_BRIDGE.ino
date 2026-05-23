#include <WiFi.h>
#include <esp_now.h>

uint8_t broadcastAddress[] = {0x34, 0xB7, 0xDA, 0x5B, 0x58, 0xE0};

typedef struct struct_message {
  int red;
  int green;
  int blue;
  int white;
} struct_message;

struct_message myData;
String inputString = ""; 
bool stringComplete = false;

// --- FIX 1: Updated Callback Signature ---
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

  // Register callback
  esp_now_register_send_cb(OnDataSent);

  // --- FIX 2: Clear the struct before use ---
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

  if (stringComplete) {
    inputString.trim();

    if (inputString.equalsIgnoreCase("deviceinfo")) {
      Serial.println("leddriver_sender");
    } else {
      int sep1 = inputString.indexOf(':');
      int sep2 = inputString.indexOf(':', sep1 + 1);
      int sep3 = inputString.indexOf(':', sep2 + 1);

      if (sep1 > 0 && sep2 > sep1 && sep3 > sep2) {
        myData.red   = inputString.substring(0, sep1).toInt();
        myData.green = inputString.substring(sep1 + 1, sep2).toInt();
        myData.blue  = inputString.substring(sep2 + 1, sep3).toInt();
        myData.white = inputString.substring(sep3 + 1).toInt();
        
        esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
        
        if (result != ESP_OK) {
          Serial.println("Error sending the data");
        }
      } else {
        Serial.println("Invalid Format! Use R:G:B:W");
      }
    }
    inputString = "";
    stringComplete = false;
  }
}