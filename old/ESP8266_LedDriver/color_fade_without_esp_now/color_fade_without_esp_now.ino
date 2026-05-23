#include <Arduino.h>

// LED pins
#define greenLedPin D1  // GPIO5
#define redLedPin   D2  // GPIO4
#define whiteLedPin D3  // GPIO0
#define blueLedPin  D4  // GPIO2

// Fade speed (bigger delay = slower cycle)
const int hueStepDelay = 40;  // ms per step
const float hueStepSize = 0.1; // degrees per step (smaller = smoother)

// Hue value (0–360)
float hue = 0.0;

// HSV → RGB conversion
void hsvToRgb(float h, float s, float v, int &r, int &g, int &b) {
  float C = v * s;
  float X = C * (1 - fabs(fmod(h / 60.0, 2) - 1));
  float m = v - C;

  float r_, g_, b_;

  if (h < 60)      { r_ = C; g_ = X; b_ = 0; }
  else if (h < 120){ r_ = X; g_ = C; b_ = 0; }
  else if (h < 180){ r_ = 0; g_ = C; b_ = X; }
  else if (h < 240){ r_ = 0; g_ = X; b_ = C; }
  else if (h < 300){ r_ = X; g_ = 0; b_ = C; }
  else             { r_ = C; g_ = 0; b_ = X; }

  r = (int)((r_ + m) * 1023);
  g = (int)((g_ + m) * 1023);
  b = (int)((b_ + m) * 1023);
}

void setup() {
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
  pinMode(whiteLedPin, OUTPUT);

  analogWrite(redLedPin, 0);
  analogWrite(greenLedPin, 0);
  analogWrite(blueLedPin, 0);
  analogWrite(whiteLedPin, 0);
}

void loop() {
  int r, g, b;

  // Full saturation, full brightness
  hsvToRgb(hue, 1.0, 1.0, r, g, b);

  // Write to LEDs
  analogWrite(redLedPin, r);
  analogWrite(greenLedPin, g);
  analogWrite(blueLedPin, b);
  analogWrite(whiteLedPin, 0); // keep white off

  // Advance hue slowly
  hue += hueStepSize;
  if (hue >= 360.0) hue = 0.0;

  delay(hueStepDelay);
}



