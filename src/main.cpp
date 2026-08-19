#include <Arduino.h>
#include <WiFi.h>
#include <esp_display_panel.hpp>
#include "SaunaDebugDisplay.h"
#include "wifi_secrets.h"

using namespace esp_panel::board;
using namespace esp_panel::drivers;

Board *board = nullptr;
LCD *lcd = nullptr;

SaunaDebugDisplay debug;

constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 10000;
constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 10000;

unsigned long lastWiFiReconnectAttempt = 0;
bool wifiWasConnected = false;


void printWiFiStatus(bool ok) {

  String line = "WiFi";

  while (line.length() < 22) {
    line += " ";
  }

  line += ok ? "OK" : "WAITING";

  Serial.println(line);
}


void startWiFiConnection(
  const char *message,
  const char *displayStatus
) {

  Serial.println(message);
  Serial.println(message);
  printWiFiStatus(false);
  debug.wifiStatus(displayStatus);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  lastWiFiReconnectAttempt = millis();
}


void showWiFiConnection() {

  Serial.print("WiFi SSID: ");
  Serial.println(WIFI_SSID);
  Serial.print("WiFi IP: ");
  Serial.println(WiFi.localIP());

  printWiFiStatus(true);
  Serial.println("SSID: " + String(WIFI_SSID));
  Serial.println("IP: " + WiFi.localIP().toString());
  debug.wifiStatus(
    "WiFi    OK  " + WiFi.localIP().toString()
  );

  wifiWasConnected = true;
}


void connectWiFiAtStartup() {

  WiFi.mode(WIFI_STA);
  startWiFiConnection(
    "WiFi connecting...",
    "WiFi    CONNECTING..."
  );

  const unsigned long startedAt = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startedAt < WIFI_CONNECT_TIMEOUT_MS) {
    debug.update();
    delay(100);
  }

  if (WiFi.status() == WL_CONNECTED) {
    showWiFiConnection();
  } else {
    Serial.println("WiFi connection timed out");
    Serial.println("WiFi timeout");
    debug.wifiStatus("WiFi    TIMEOUT");
  }
}


void setup() {

  // --------------------------------------------------
  // Serial
  // --------------------------------------------------

  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("================================");
  Serial.println("Waveshare ESP32-S3 4.3B");
  Serial.println("Sauna Debug Display Test");
  Serial.println("================================");


  // --------------------------------------------------
  // Board
  // --------------------------------------------------

  board = new Board();

  Serial.println("Board created");

  if (!board->init()) {
    Serial.println("ERROR: board init failed");
    return;
  }

  Serial.println("Board initialized");


  // --------------------------------------------------
  // LCD
  // --------------------------------------------------

  lcd = board->getLCD();

  if (!lcd) {
    Serial.println("ERROR: LCD not found");
    return;
  }

  Serial.println("LCD found");


  // --------------------------------------------------
  // RGB bus
  // --------------------------------------------------

  auto bus = lcd->getBus();

  if (bus->getBasicAttributes().type ==
      ESP_PANEL_BUS_TYPE_RGB) {

    Serial.println("RGB bus detected");

    auto rgb = static_cast<BusRGB *>(bus);

    rgb->configRGB_BounceBufferSize(
      lcd->getFrameWidth() * 20
    );

    Serial.println("RGB buffer configured");
  }


  // --------------------------------------------------
  // Start board / display
  // --------------------------------------------------

  if (!board->begin()) {
    Serial.println("ERROR: board begin failed");
    return;
  }

  Serial.println("DISPLAY INITIALIZED!");


  // --------------------------------------------------
  // Start debug display
  // --------------------------------------------------

  if (!debug.begin(lcd)) {
    Serial.println(
      "ERROR: Debug display init failed"
    );
    return;
  }


  // --------------------------------------------------
  // Test information
  // --------------------------------------------------

  debug.println("SYSTEM");

  debug.status("Board", true);
  debug.status("Display", true);
  debug.status("Touch", true);

  debug.println("");
  debug.println("SENSORS");

  debug.status("DS2484", false);
  debug.status("K-Type", false);

  debug.println("");
  debug.println("TEST VALUES");

  debug.value(
    "Temperature",
    23.4,
    "C"
  );

  debug.value(
    "Humidity",
    48.0,
    "%"
  );

  debug.value(
    "Pressure",
    1012.0,
    "hPa"
  );

  debug.println("");
  debug.println("System running");


  // --------------------------------------------------
  // WiFi
  // --------------------------------------------------

  connectWiFiAtStartup();
}


void loop() {

  // Update uptime clock
  debug.update();


  const bool wifiConnected = WiFi.status() == WL_CONNECTED;

  if (wifiConnected && !wifiWasConnected) {
    showWiFiConnection();
  }

  if (!wifiConnected && wifiWasConnected) {
    Serial.println("WiFi disconnected");
    Serial.println("WiFi disconnected");
    debug.wifiStatus("WiFi    WAITING");
    wifiWasConnected = false;
  }

  if (!wifiConnected &&
      millis() - lastWiFiReconnectAttempt >=
        WIFI_RECONNECT_INTERVAL_MS) {

    startWiFiConnection(
      "WiFi reconnecting...",
      "WiFi    RECONNECTING..."
    );
  }

  delay(100);
}
