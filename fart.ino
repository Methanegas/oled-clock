#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire, -1);

const char* ssid = ""; // Enter your WiFi SSID
const char* password = ""; // Enter your WiFi password

int GMTOffset = -18000; // GMT offset in seconds (EST in this example)
int daylightOffset = 0; // Daylight saving time offset (if applicable)

void setup() {
  Serial.begin(115200); // Initialize serial communication

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Initialize the OLED display
    Serial.println(F("SSD1306 allocation failed")); // Display error message if initialization fails
    for (;;);
  }
  delay(2000);
  display.clearDisplay(); // Clear the display
  display.setTextSize(2); // Set text size
  display.setTextColor(WHITE); // Set text color

  WiFi.begin(ssid, password); // Connect to WiFi network
  while (WiFi.status() != WL_CONNECTED) { // Wait for connection
    delay(1000);
    Serial.println("Connecting...");
  }
  Serial.println("Connected to Wi-Fi!"); // Connection successful
  configTime(GMTOffset, daylightOffset, "pool.ntp.org"); // Configure time synchronization with NTP server
}

void loop() {
  time_t rawtime = time(nullptr); // Get current time
  struct tm* timeinfo = localtime(&rawtime); // Convert to local time

  int hour = timeinfo->tm_hour; // Extract hours
  bool pm = false;
  if (hour >= 12) { // Check if PM
    pm = true;
    if (hour > 12) {
      hour -= 12;
    }
  }
  if (hour == 0) { // Convert midnight to 12 AM
    hour = 12;
  }

  display.clearDisplay(); // Clear the display
  display.setCursor(15, 0); // Set cursor position
  display.print((hour < 10) ? "0" : ""); // Add leading zero if necessary
  display.print(hour); // Print hours
  display.print(":");
  display.print((timeinfo->tm_min < 10) ? "0" : ""); // Add leading zero if necessary
  display.print(timeinfo->tm_min); // Print minutes
  if (pm) {
    display.print(" PM");
  } else {
    display.print(" AM");
  }

  int16_t x = (display.width() - display.getCursorX()) / 2; // Calculate center position
  display.setCursor(x, display.getCursorY());

  int radius = 15; // Set radius for clock hands
  display.drawCircle(display.width() / 2, display.height() / 2 + 10, 2, WHITE); // Draw clock center

  // Draw clock tick marks
  for (int i = 0; i < 360; i += 30) {
    float angle = i;
    angle /= 57.29577951; // Convert degrees to radians
    int x1 = (64 + (sin(angle) * radius)); // Calculate x coordinate
    int y1 = (32 + 10 - (cos(angle) * radius)); // Calculate y coordinate
    int x2 = (64 + (sin(angle) * (radius - 3)));
    int y2 = (32 + 10 - (cos(angle) * (radius - 3)));
    display.drawLine(x1, y1, x2, y2, WHITE); // Draw tick mark
  }

  // Draw seconds hand
  float angle = timeinfo->tm_sec * 6; // Calculate angle (360 degrees for 60 seconds)
  angle /= 57.29577951; // Convert degrees to radians
  int x2 = (64 + (sin(angle) * radius)); // Calculate endpoint x coordinate
  int y2 = (32 + 10 - (cos(angle) * radius)); // Calculate endpoint y coordinate
  display.drawLine(64, 32 + 10, x2, y2, WHITE); // Draw seconds hand

  // Draw minutes hand
  angle = timeinfo->tm_min * 6; // Calculate angle (360 degrees for 60 minutes)
  angle /= 57.29577951; // Convert degrees to radians
  x2 = (64 + (sin(angle) * (radius - 3))); // Calculate endpoint x coordinate
  y2 = (32 + 10 - (cos(angle) * (radius - 3))); // Calculate endpoint y coordinate
  display.drawLine(64, 32 + 10, x2, y2, WHITE); // Draw minutes hand

  // Draw hours hand
  angle = timeinfo->tm_hour * 30 + int((timeinfo->tm_min / 12) * 6); // Calculate angle (360 degrees for 12 hours)
  angle /= 57.29577951; // Convert degrees to radians
  x2 = (64 + (sin(angle) * (radius - 6))); // Calculate endpoint x coordinate
  y2 = (32 + 10 - (cos(angle) * (radius - 6))); // Calculate endpoint y coordinate
  display.drawLine(64, 32 + 10, x2, y2, WHITE); // Draw hours hand

  display.display(); // Update display
  delay(1000); // Wait for 1 second
}
