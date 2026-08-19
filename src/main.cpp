#include <Arduino.h>
#include <esp_display_panel.hpp>
#include "SaunaDebugDisplay.h"

using namespace esp_panel::board;
using namespace esp_panel::drivers;

Board *board = nullptr;
LCD *lcd = nullptr;

SaunaDebugDisplay debug;


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
}


void loop() {

  // Update uptime clock
  debug.update();

  delay(1000);
}