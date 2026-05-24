/*
 * ====================================================================
 * Project:      RGBW Led Driver Controller
 * Description:  Hosts an internal web page allowing real-time adjustment
 * of 4 PWM channels.
 * Target Board: ESP32-S3 Zero
 * ====================================================================
 */

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// --- WiFi Credentials ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";


// --- Pin & PWM Definitions ---
const int pins[] = {10, 11, 12, 13}; 

// Locked to your target hardware parameters
const uint32_t PWM_FREQ = 1000;  // 1 kHz Frequency
const uint8_t PWM_RES   = 10;    // 10-bit Resolution (0 - 1023)

WebServer server(80);

// HTML Dashboard String
const char PAGE_MAIN[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>LED Dashboard</title>
    <style>
        body { font-family: sans-serif; text-align: center; background: #121212; color: white; padding: 20px; }
        .card { background: #1e1e1e; padding: 15px; border-radius: 12px; margin: 10px auto; max-width: 350px; border: 1px solid #333; }
        h2 { color: #00adb5; }
        input[type=range] { width: 100%; accent-color: #00adb5; cursor: pointer; }
        .val-display { font-size: 1.1em; margin-top: 5px; color: #00adb5; font-weight: bold; }
    </style>
</head>
<body>
    <h2>Led Driver Control</h2>
    <div class="card">
        <p>Channel 1 (red) </p>
        <input type="range" min="0" max="1023" value="0" oninput="update(1, this.value)">
        <div class="val-display" id="val1">0</div>
    </div>
    <div class="card">
        <p>Channel 2 (green)</p>
        <input type="range" min="0" max="1023" value="0" oninput="update(2, this.value)">
        <div class="val-display" id="val2">0</div>
    </div>
    <div class="card">
        <p>Channel 3 (blue)</p>
        <input type="range" min="0" max="1023" value="0" oninput="update(3, this.value)">
        <div class="val-display" id="val3">0</div>
    </div>
    <div class="card">
        <p>Channel 4 (white)</p>
        <input type="range" min="0" max="1023" value="0" oninput="update(4, this.value)">
        <div class="val-display" id="val4">0</div>
    </div>

    <script>
        function update(ch, val) {
            document.getElementById('val' + ch).innerHTML = val;
            fetch(`/set?ch=${ch}&val=${val}`);
        }
    </script>
</body>
</html>
)=====";

void handleRoot() {
  server.send(200, "text/html", PAGE_MAIN);
}

void handleUpdate() {
  if (server.hasArg("ch") && server.hasArg("val")) {
    int ch = server.arg("ch").toInt();
    int val = server.arg("val").toInt();
    
    if (ch >= 1 && ch <= 4) {
      // Guard bounds to keep values within 10-bit hardware boundaries
      if(val > 1023) val = 1023;
      if(val < 0) val = 0;

      // Updated native ESP32 API call for writing duty cycle changes
      ledcWrite(pins[ch-1], val);
    }
    server.send(200, "text/plain", "OK");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize pins using the modern ESP32 LEDC API
  for(int i = 0; i < 4; i++) {
    // Under Arduino ESP32 core v3.x+, ledcAttach combines initialization and routing automatically
    ledcAttach(pins[i], PWM_FREQ, PWM_RES);
    ledcWrite(pins[i], 0); // Start off
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected!");

  // --- START MDNS ---
  if (MDNS.begin("ledcontrol")) {
    Serial.println("Access dashboard at: http://ledcontrol.local");
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleUpdate);
  server.begin();
}

void loop() {
  server.handleClient();
}