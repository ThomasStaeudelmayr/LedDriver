#include <WiFi.h>
#include <esp_now.h>

// REPLACE WITH YOUR RECEIVER'S MAC ADDRESS
uint8_t broadcastAddress[] = {0x34, 0xB7, 0xDA, 0x5B, 0x58, 0xE0}; 




typedef struct struct_message {
  int red;
  int green;
  int blue;
  int white;
} struct_message;

struct_message myData;
esp_now_peer_info_t peerInfo;

// UPDATED CALLBACK SIGNATURE
// The first argument is now 'const esp_now_send_info_t *info'
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

  // Register the updated callback
  esp_now_register_send_cb(OnDataSent);
  
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  myData.red = random(0, 1024);
  myData.green = random(0, 1024);
  myData.blue = random(0, 1024);
  myData.white = random(0, 1024);

  Serial.printf("Sending - R:%d G:%d B:%d W:%d\n", 
                myData.red, myData.green, myData.blue, myData.white);

  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  } else {
    Serial.println("Error sending the data");
  }

  delay(1000);
}