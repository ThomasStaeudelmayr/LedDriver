#include <ESP8266WiFi.h>
#include <espnow.h>

// Define LED pins (change if needed)
#define greenLedPin D1  // GPIO5
#define redLedPin   D2  // GPIO4
#define whiteLedPin D3  // GPIO0
#define blueLedPin  D4  // GPIO2

// PWM intensity values (0–1023)
int pwmRed = 0;
int pwmGreen = 0;
int pwmBlue = 0;
int pwmWhite = 0;

// Structure for incoming RGBW data
typedef struct struct_message {
  int red = 0;
  int green = 0;
  int blue = 0;
  int white = 0;
} struct_message;

struct_message rgbwData; // Variable to hold received values

// Callback function triggered when ESP-NOW packet is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&rgbwData, incomingData, sizeof(rgbwData));  // Copy data into rgbwData

  // Extract and apply PWM values
  pwmRed = rgbwData.red;
  pwmGreen = rgbwData.green;
  pwmBlue = rgbwData.blue;
  pwmWhite = rgbwData.white;

  analogWrite(redLedPin, pwmRed);
  analogWrite(greenLedPin, pwmGreen);
  analogWrite(blueLedPin, pwmBlue);
  analogWrite(whiteLedPin, pwmWhite);

  // Debug output 
  Serial.printf("Received - R:%d G:%d B:%d W:%d\n", pwmRed, pwmGreen, pwmBlue, pwmWhite);
}

void setup() {
  // Start serial communication
  Serial.begin(115200);
  delay(100);

  // Set WiFi to station mode (no access point)
  WiFi.mode(WIFI_STA);

  // Initialize LED pins
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(whiteLedPin, OUTPUT);

  // Turn off LEDs at startup
  analogWrite(redLedPin, 0);
  analogWrite(greenLedPin, 0);
  analogWrite(blueLedPin, 0);
  analogWrite(whiteLedPin, 0);

  // Print MAC address (useful to pair with sender)
  Serial.println();
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Initialize ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Set this device as a receiver (slave)
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  
  // Register receive callback
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nothing needed in loop – work is done in callback
}
