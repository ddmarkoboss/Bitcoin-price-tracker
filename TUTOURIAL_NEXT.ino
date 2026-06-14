#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Wi-Fi Credentials
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

const char* apiUrl = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("OLED initialization failed"));
    for(;;);
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0, 20);
  display.print("Connecting to WiFi...");
  display.display();

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  display.clearDisplay();
  display.setCursor(0, 20);
  display.print("Connected!");
  display.display();
  delay(1500);
}

void loop() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(apiUrl);
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0) {
      String payload = http.getString();
      
      DynamicJsonDocument doc(512);
      deserializeJson(doc, payload);
      long btcPrice = doc["bitcoin"]["usd"];

      display.clearDisplay();
      
      display.setTextSize(1);
      display.setCursor(12, 0);
      display.print("LIVE CRYPTO TICKER");
      display.drawFastHLine(0, 12, 128, WHITE);

      display.setTextSize(2);
      display.setCursor(0, 25);
      display.print("BTC:");

      display.setCursor(50, 25);
      display.print("$");
      display.print(btcPrice);

      // Bottom Decorative Accent
      display.drawFastHLine(0, 52, 128, WHITE);
      display.setTextSize(1);
      display.setCursor(35, 56);
      display.print("Updating...");

      display.display();
    } else {
      Serial.print("Error on HTTP request: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    // Failover if connection drops
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 25);
    display.print("WiFi Lost! Reconnecting");
    display.display();
  }
  
  // Wait 30 seconds before pulling live numbers again
  delay(30000);
}