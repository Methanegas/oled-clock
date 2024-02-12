#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);

const char* ssid = "";
const char* password = "";

int GMTOffset = -18000;
int daylightOffset = 0;

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to Wi-Fi!");
  configTime(GMTOffset, daylightOffset, "pool.ntp.org");
}

void loop() {
  time_t rawtime = time(nullptr);
  struct tm* timeinfo = localtime(&rawtime);

  int hour = timeinfo->tm_hour;
  bool pm = false;
  if (hour >= 12) {
    pm = true;
    if (hour > 12) {
      hour -= 12;
    }
  }
  if (hour == 0) {
    hour = 12;
  }

  display.clearDisplay();
  display.setCursor(15, 0);
  display.print((hour < 10) ? "0" : "");
  display.print(hour);
  display.print(":");
  display.print((timeinfo->tm_min < 10) ? "0" : "");
  display.print(timeinfo->tm_min);
  if (pm) {
    display.print(" PM");
  } else {
    display.print(" AM");
  }

  int16_t x = (display.width() - display.getCursorX()) / 2;
  display.setCursor(x, display.getCursorY());

  int radius = 15;
  display.drawCircle(display.width() / 2, display.height() / 2 + 10, 2, WHITE);

  for (int i = 0; i < 360; i += 30) {
    float angle = i;
    angle /= 57.29577951;
    int x1 = (64 + (sin(angle) * radius));
    int y1 = (32 + 10 - (cos(angle) * radius));
    int x2 = (64 + (sin(angle) * (radius - 3)));
    int y2 = (32 + 10 - (cos(angle) * (radius - 3)));
    display.drawLine(x1, y1, x2, y2, WHITE);
  }

  float angle = timeinfo->tm_sec * 6;
  angle /= 57.29577951;
  int x2 = (64 + (sin(angle) * radius));
  int y2 = (32 + 10 - (cos(angle) * radius));
  display.drawLine(64, 32 + 10, x2, y2, WHITE);

  angle = timeinfo->tm_min * 6;
  angle /= 57.29577951;
  x2 = (64 + (sin(angle) * (radius - 3)));
  y2 = (32 + 10 - (cos(angle) * (radius - 3)));
  display.drawLine(64, 32 + 10, x2, y2, WHITE);

  angle = timeinfo->tm_hour * 30 + int((timeinfo->tm_min / 12) * 6);
  angle /= 57.29577951;
  x2 = (64 + (sin(angle) * (radius - 6)));
  y2 = (32 + 10 - (cos(angle) * (radius - 6)));
  display.drawLine(64, 32 + 10, x2, y2, WHITE);

  display.display();
  delay(1000);
}
