#include <ESP8266WiFi.h>
#include <espnow.h>

// Receiver MAC address
uint8_t broadcastAddress[] = {0xB4, 0xE6, 0x2D, 0x29, 0x2F, 0x1C};

typedef struct struct_message {
  int red = 0;
  int green = 0;
  int blue = 0;
  int white = 0;
} struct_message;

struct_message myData;

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
  WiFi.mode(WIFI_STA);

  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
  esp_now_register_send_cb(OnDataSent);
  esp_now_add_peer(broadcastAddress, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
  inputString.reserve(64);  // reserve some memory
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
      }
    }

    inputString = "";
    stringComplete = false;
  }
}

