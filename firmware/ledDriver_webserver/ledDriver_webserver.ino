#include <WiFi.h>
#include <WebServer.h>

// --- WiFi Credentials ---
const char* ssid = "WLAN19064339";
const char* password = "SfffaddchSu5";

// --- Pin Definitions ---
const int pins[] = {13, 12, 11, 10}; 

WebServer server(80);

// HTML, CSS, and JavaScript for the Dashboard
const char PAGE_MAIN[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <title>ESP32 LED Control</title>
    <style>
        body { font-family: Arial; text-align: center; background: #121212; color: white; }
        .container { max-width: 400px; margin: auto; padding: 20px; }
        .card { background: #1e1e1e; padding: 20px; border-radius: 15px; margin-bottom: 20px; box-shadow: 0 4px 10px rgba(0,0,0,0.5); }
        h2 { color: #00adb5; }
        input[type=range] { width: 100%; margin: 15px 0; accent-color: #00adb5; }
        .val-display { font-weight: bold; font-size: 1.2em; color: #eee; }
    </style>
</head>
<body>
    <div class="container">
        <h2>LED Channels</h2>
        <div class="card">
            <p>Channel 1 (D13)</p>
            <input type="range" min="0" max="255" value="0" oninput="updateLED(1, this.value)">
            <div class="val-display" id="val1">0</div>
        </div>
        <div class="card">
            <p>Channel 2 (D12)</p>
            <input type="range" min="0" max="255" value="0" oninput="updateLED(2, this.value)">
            <div class="val-display" id="val2">0</div>
        </div>
        <div class="card">
            <p>Channel 3 (D11)</p>
            <input type="range" min="0" max="255" value="0" oninput="updateLED(3, this.value)">
            <div class="val-display" id="val3">0</div>
        </div>
        <div class="card">
            <p>Channel 4 (D10)</p>
            <input type="range" min="0" max="255" value="0" oninput="updateLED(4, this.value)">
            <div class="val-display" id="val4">0</div>
        </div>
    </div>

    <script>
        function updateLED(ch, val) {
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
      analogWrite(pins[ch-1], val);
      Serial.printf("Channel %d set to %d\n", ch, val);
    }
    server.send(200, "text/plain", "OK");
  }
}

void setup() {
  Serial.begin(115200);
  
  for(int i=0; i<4; i++) {
    pinMode(pins[i], OUTPUT);
    analogWrite(pins[i], 0);
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleUpdate);
  server.begin();
}

void loop() {
  server.handleClient();
}