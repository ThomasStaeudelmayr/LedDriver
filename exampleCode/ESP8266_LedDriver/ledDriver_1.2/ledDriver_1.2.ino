#include <ESP8266WiFi.h>
#include <espnow.h>
#define greenLedPin D1
#define redLedPin D2
#define whiteLedPin D3
#define blueLedPin D4

int pwmRed=0;
int pwmGreen=0;
int pwmBlue=0;
int pwmWhite=0;


typedef struct struct_message {
  int red = 0;
  int green = 0;
  int blue = 0;
  int white = 0;
} struct_message;
struct_message rgbwData;


void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&rgbwData, incomingData, sizeof(rgbwData));
  pwmRed=rgbwData.red;
  pwmGreen=rgbwData.green;
  pwmBlue=rgbwData.blue;
  pwmWhite=rgbwData.white;
  analogWrite(greenLedPin, pwmGreen);
  analogWrite(redLedPin, pwmRed);
  analogWrite(blueLedPin, pwmBlue);
  analogWrite(whiteLedPin, pwmWhite);
}
 
void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  delay(100);
  pinMode(greenLedPin, OUTPUT);  // put your setup code here, to run once:
  pinMode(redLedPin, OUTPUT); 
  pinMode(blueLedPin, OUTPUT); 
  pinMode(whiteLedPin, OUTPUT); 
  analogWrite(greenLedPin, 0);
  analogWrite(redLedPin, 0);
  analogWrite(blueLedPin, 0);
  analogWrite(whiteLedPin, 0);

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);
}

void loop() {

}
