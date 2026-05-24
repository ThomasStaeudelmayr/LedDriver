#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// --- WiFi Credentials ---
const char* ssid = "WLAN19064339";
const char* password = "SfffaddchSu5";
// --- Pin Definitions ---
const int pins[] = {13, 12, 11, 10}; 

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
    <h2>ESP32-S3 Zero Control</h2>
    <div class="card">
        <p>Channel 1 (D13)</p>
        <input type="range" min="0" max="255" value="0" oninput="update(1, this.value)">
        <div class="val-display" id="val1">0</div>
    </div>
    <div class="card">
        <p>Channel 2 (D12)</p>
        <input type="range" min="0" max="255" value="0" oninput="update(2, this.value)">
        <div class="val-display" id="val2">0</div>
    </div>
    <div class="card">
        <p>Channel 3 (D11)</p>
        <input type="range" min="0" max="255" value="0" oninput="update(3, this.value)">
        <div class="val-display" id="val3">0</div>
    </div>
    <div class="card">
        <p>Channel 4 (D10)</p>
        <input type="range" min="0" max="255" value="0" oninput="update(4, this.value)">
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
      analogWrite(pins[ch-1], val);
    }
    server.send(200, "text/plain", "OK");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Set pins as outputs
  for(int i=0; i<4; i++) {
    pinMode(pins[i], OUTPUT);
    analogWrite(pins[i], 0);
  }

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // --- START MDNS ---
  // This allows you to use http://ledcontrol.local instead of the IP
  if (MDNS.begin("ledcontrol")) {
    Serial.println("\nmDNS responder started");
    Serial.println("Access at: http://ledcontrol.local");
  }

  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/set", handleUpdate);
  server.begin();
}

void loop() {
  server.handleClient();
  // Optional: keep mDNS running on some older ESP versions
  // MDNS.update(); 
}