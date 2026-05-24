/*
 * ====================================================================
 * Project:      ESP-NOW 4 channel LED Driver Receiver
 * Description:  Receives 10-bit RGBW color data via ESP-NOW protocol 
 *               and controls LEDs using PWM.
 * Target Board: ESP32-S3 Zero
 * Author:       Thomas Stäudelmayr
 * Date:         May 2026
 * ====================================================================
 */

#include <WiFi.h>
#include <esp_now.h>

// LED pins (ESP32-S3 Zero)
#define redLedPin   10
#define greenLedPin 11
#define blueLedPin  12
#define whiteLedPin 13

// PWM settings
#define PWM_FREQ 1000 //1kHz
#define PWM_RESOLUTION 10  // 10-bit (0–1023)

// Structure for incoming RGBW data
typedef struct struct_message {
  int red;
  int green;
  int blue;
  int white;
} struct_message;

struct_message rgbwData;

// Callback when data is received
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {

  if (len != sizeof(rgbwData)) {
    Serial.println("Invalid data size received!");
    return;
  }

  memcpy(&rgbwData, incomingData, sizeof(rgbwData));

  ledcWrite(redLedPin,   rgbwData.red);
  ledcWrite(greenLedPin, rgbwData.green);
  ledcWrite(blueLedPin,  rgbwData.blue);
  ledcWrite(whiteLedPin, rgbwData.white);

  Serial.printf("Received - R:%d G:%d B:%d W:%d\n",
                rgbwData.red,
                rgbwData.green,
                rgbwData.blue,
                rgbwData.white);
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  WiFi.mode(WIFI_STA);
  delay(1000);

  ledcAttach(redLedPin,   PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(greenLedPin, PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(blueLedPin,  PWM_FREQ, PWM_RESOLUTION);
  ledcAttach(whiteLedPin, PWM_FREQ, PWM_RESOLUTION);

  // Print MAC address (Copy this into your Transmitter code!)
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  //wait for data 
}