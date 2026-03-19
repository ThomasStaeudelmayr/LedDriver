#include <WiFi.h>
#include <esp_now.h>

// LED pins (ESP32-S3 Zero)
#define redLedPin   10
#define greenLedPin 11
#define blueLedPin  12
#define whiteLedPin 13

// PWM settings
#define PWM_FREQ 5000
#define PWM_RESOLUTION 10  // 10-bit (0–1023)

// PWM channels
#define CH_RED   0
#define CH_GREEN 1
#define CH_BLUE  2
#define CH_WHITE 3

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

  // Apply PWM
  ledcWrite(CH_RED,   rgbwData.red);
  ledcWrite(CH_GREEN, rgbwData.green);
  ledcWrite(CH_BLUE,  rgbwData.blue);
  ledcWrite(CH_WHITE, rgbwData.white);

  Serial.printf("Received - R:%d G:%d B:%d W:%d\n",
                rgbwData.red,
                rgbwData.green,
                rgbwData.blue,
                rgbwData.white);
}

void setup() {
  Serial.begin(115200);
  delay(100);

  // WiFi in station mode
  WiFi.mode(WIFI_STA);

  // Setup PWM channels
  ledcSetup(CH_RED, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(CH_GREEN, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(CH_BLUE, PWM_FREQ, PWM_RESOLUTION);
  ledcSetup(CH_WHITE, PWM_FREQ, PWM_RESOLUTION);

  // Attach pins to PWM channels
  ledcAttachPin(redLedPin, CH_RED);
  ledcAttachPin(greenLedPin, CH_GREEN);
  ledcAttachPin(blueLedPin, CH_BLUE);
  ledcAttachPin(whiteLedPin, CH_WHITE);

  // Turn off LEDs initially
  ledcWrite(CH_RED, 0);
  ledcWrite(CH_GREEN, 0);
  ledcWrite(CH_BLUE, 0);
  ledcWrite(CH_WHITE, 0);

  // Print MAC address
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
  // Nothing needed
}