#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <esp_display_panel.hpp>

#include "DebugLog.h"
#include "SaunaDebugDisplay.h"
#include "PCF85063.h"
#include "wifi_secrets.h"

using namespace esp_panel::board;
using namespace esp_panel::drivers;


// ==================================================
// Board / display
// ==================================================

Board* board = nullptr;
LCD* lcd = nullptr;

SaunaDebugDisplay debugDisplay;


// ==================================================
// RTC
// ==================================================

PCF85063 rtc;

PCF85063::DateTime rtcDateTime;

bool rtcValid = false;


// ==================================================
// NTP
// ==================================================

constexpr char TIMEZONE[] =
  "CET-1CEST,M3.5.0,M10.5.0/3";

bool ntpStarted = false;
bool rtcSyncedFromNtp = false;


// ==================================================
// WiFi
// ==================================================

constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 10000;
constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS = 10000;

unsigned long lastWiFiReconnectAttempt = 0;
unsigned long wifiReconnectCount = 0;

bool wifiWasConnected = false;


// ==================================================
// Dashboard timing
// ==================================================

unsigned long initTimeMs = 0;
unsigned long lastDashboardUpdate = 0;


// ==================================================
// Formatting helpers
// ==================================================

static String formatRuntime(
  unsigned long milliseconds
) {

  unsigned long totalSeconds =
    milliseconds / 1000UL;

  const unsigned long hours =
    totalSeconds / 3600UL;

  totalSeconds %= 3600UL;

  const unsigned long minutes =
    totalSeconds / 60UL;

  const unsigned long seconds =
    totalSeconds % 60UL;

  char buffer[16];

  snprintf(
    buffer,
    sizeof(buffer),
    "%02lu:%02lu:%02lu",
    hours,
    minutes,
    seconds
  );

  return String(buffer);
}


static String formatRtcDateTime(
  const PCF85063::DateTime& dt
) {

  char buffer[32];

  snprintf(
    buffer,
    sizeof(buffer),
    "%04d-%02d-%02d %02d:%02d:%02d",
    dt.year,
    dt.month,
    dt.day,
    dt.hour,
    dt.minute,
    dt.second
  );

  return String(buffer);
}


// ==================================================
// RTC
// ==================================================

static bool readRtc() {

  if (
    !rtc.read(
      rtcDateTime
    )
  ) {

    rtcValid = false;

    return false;
  }

  rtcValid = true;

  return true;
}


static void updateRtcDashboard() {

  if (rtcValid) {

    debugDisplay.setDateTime(
      formatRtcDateTime(
        rtcDateTime
      ),
      "OK"
    );

  } else {

    debugDisplay.setDateTime(
      "RTC INVALID",
      "INVALID"
    );
  }
}


// ==================================================
// NTP
// ==================================================

static void startNtp() {

  if (ntpStarted) {
    return;
  }

  configTzTime(
    TIMEZONE,
    "pool.ntp.org",
    "time.nist.gov"
  );

  ntpStarted = true;

  DebugLog::println(
    "NTP started"
  );
}


static bool syncRtcFromNtp() {

  if (!ntpStarted) {
    return false;
  }

  struct tm timeInfo;

  if (
    !getLocalTime(
      &timeInfo,
      5000
    )
  ) {

    DebugLog::println(
      "NTP time not available"
    );

    return false;
  }


  PCF85063::DateTime newTime;

  newTime.year =
    timeInfo.tm_year + 1900;

  newTime.month =
    timeInfo.tm_mon + 1;

  newTime.day =
    timeInfo.tm_mday;

  newTime.weekday =
    timeInfo.tm_wday;

  newTime.hour =
    timeInfo.tm_hour;

  newTime.minute =
    timeInfo.tm_min;

  newTime.second =
    timeInfo.tm_sec;


  DebugLog::printf(
    "NTP time: %04d-%02d-%02d %02d:%02d:%02d\n",
    newTime.year,
    newTime.month,
    newTime.day,
    newTime.hour,
    newTime.minute,
    newTime.second
  );


  if (
    !rtc.write(
      newTime
    )
  ) {

    DebugLog::println(
      "RTC write failed"
    );

    return false;
  }


  // Read the newly written time back.
  if (
    !readRtc()
  ) {

    DebugLog::println(
      "RTC read-back failed"
    );

    return false;
  }


  DebugLog::println(
    "RTC synchronized from NTP"
  );


  rtcSyncedFromNtp =
    true;


  return true;
}


// ==================================================
// WiFi
// ==================================================

static void printWiFiStatus(
  bool ok
) {

  String line =
    "WiFi";

  while (
    line.length() < 22
  ) {

    line += " ";
  }

  line +=
    ok ? "OK" : "WAITING";

  DebugLog::println(
    line
  );
}


static void updateConnectionDashboard() {

  const bool connected =
    WiFi.status() ==
    WL_CONNECTED;


  if (connected) {

    debugDisplay.setConnection(
      "OK",
      WIFI_SSID,
      WiFi.localIP().toString(),
      String(WiFi.RSSI()) +
        " dBm",
      rtcValid
        ? "OK"
        : "INVALID"
    );

  } else {

    debugDisplay.setConnection(
      "WAITING",
      WIFI_SSID,
      "---",
      "---",
      rtcValid
        ? "OK"
        : "INVALID"
    );
  }
}


static void startWiFiConnection(
  const char* message,
  const char* displayStatus
) {

  DebugLog::println(
    message
  );

  printWiFiStatus(
    false
  );


  debugDisplay.setConnection(
    displayStatus,
    WIFI_SSID,
    "---",
    "---",
    rtcValid
      ? "OK"
      : "INVALID"
  );


  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );


  lastWiFiReconnectAttempt =
    millis();
}


static void showWiFiConnection() {

  const String ip =
    WiFi.localIP().toString();


  DebugLog::print(
    "WiFi SSID: "
  );

  DebugLog::println(
    WIFI_SSID
  );


  DebugLog::print(
    "WiFi IP: "
  );

  DebugLog::println(
    ip
  );


  printWiFiStatus(
    true
  );


  DebugLog::println(
    "SSID: " +
    String(WIFI_SSID)
  );


  DebugLog::println(
    "IP: " +
    ip
  );


  updateConnectionDashboard();


  wifiWasConnected =
    true;


  startNtp();
}


static void connectWiFiAtStartup() {

  WiFi.mode(
    WIFI_STA
  );


  startWiFiConnection(
    "WiFi connecting...",
    "CONNECTING"
  );


  const unsigned long startedAt =
    millis();


  while (
    WiFi.status() !=
      WL_CONNECTED &&
    millis() - startedAt <
      WIFI_CONNECT_TIMEOUT_MS
  ) {

    delay(100);
  }


  if (
    WiFi.status() ==
    WL_CONNECTED
  ) {

    showWiFiConnection();

  } else {

    DebugLog::println(
      "WiFi connection timed out"
    );


    debugDisplay.setConnection(
      "TIMEOUT",
      WIFI_SSID,
      "---",
      "---",
      rtcValid
        ? "OK"
        : "INVALID"
    );
  }
}


// ==================================================
// Dashboard
// ==================================================

static void buildDashboard() {

  debugDisplay.setSystem(
    "OK",
    "OK",
    "OK",
    String(initTimeMs) +
      " ms",
    formatRuntime(millis())
  );


  updateConnectionDashboard();


  debugDisplay.setSensors(
    "WAITING",
    "WAITING",
    "--",
    "--"
  );


  debugDisplay.setTestValues(
    "23.4 C",
    "48.0 %",
    "1012.0 hPa"
  );


  debugDisplay.setSystemLog(
    "OK",
    "OK",
    "OK",
    String(wifiReconnectCount)
  );


  debugDisplay.setHardware(
    ESP.getPsramSize() > 0
      ? "OK"
      : "NO",

    String(
      ESP.getFreeHeap()
    ) + " B",

    String(
      ESP.getFlashChipSize() /
      (1024UL * 1024UL)
    ) + " MB",

    String(
      ESP.getCpuFreqMHz()
    ) + " MHz"
  );


  updateRtcDashboard();


  debugDisplay.refresh();
}


// ==================================================
// Dashboard update
// ==================================================

static void updateDashboard() {

  if (
    millis() -
    lastDashboardUpdate <
    1000
  ) {

    return;
  }


  lastDashboardUpdate =
    millis();


  // -----------------------------------------------
  // RTC
  // -----------------------------------------------

  readRtc();


  // -----------------------------------------------
  // NTP → RTC
  //
  // Only synchronize when:
  //   - WiFi is connected
  //   - RTC is invalid
  //   - RTC has not already been synchronized
  // -----------------------------------------------

  if (
    WiFi.status() ==
      WL_CONNECTED
  ) {

    startNtp();


    if (
      !rtcValid &&
      !rtcSyncedFromNtp
    ) {

      if (
        syncRtcFromNtp()
      ) {

        DebugLog::println(
          "RTC now valid"
        );
      }
    }
  }


  // -----------------------------------------------
  // RTC display
  // -----------------------------------------------

  updateRtcDashboard();


  // -----------------------------------------------
  // Runtime
  // -----------------------------------------------

  debugDisplay.setRuntime(
    formatRuntime(
      millis()
    )
  );


  // -----------------------------------------------
  // WiFi
  // -----------------------------------------------

  updateConnectionDashboard();


  // -----------------------------------------------
  // System log
  // -----------------------------------------------

  debugDisplay.setSystemLog(
    "OK",
    "OK",
    "OK",
    String(
      wifiReconnectCount
    )
  );


  // -----------------------------------------------
  // Hardware
  // -----------------------------------------------

  debugDisplay.setHardware(
    ESP.getPsramSize() > 0
      ? "OK"
      : "NO",

    String(
      ESP.getFreeHeap()
    ) + " B",

    String(
      ESP.getFlashChipSize() /
      (1024UL * 1024UL)
    ) + " MB",

    String(
      ESP.getCpuFreqMHz()
    ) + " MHz"
  );


  // -----------------------------------------------
  // One complete framebuffer refresh
  // -----------------------------------------------

  debugDisplay.refresh();
}


// ==================================================
// Setup
// ==================================================

void setup() {

  const unsigned long bootStart =
    millis();


  // --------------------------------------------------
  // Serial
  // --------------------------------------------------

  DebugLog::begin(
    115200
  );


  delay(2000);


  DebugLog::println();


  DebugLog::println(
    "================================"
  );


  DebugLog::println(
    "Waveshare ESP32-S3 4.3B"
  );


  DebugLog::println(
    "Sauna Debug Dashboard"
  );


  DebugLog::println(
    "================================"
  );


  // --------------------------------------------------
  // Board
  // --------------------------------------------------

  board =
    new Board();


  DebugLog::println(
    "Board created"
  );


  if (
    !board->init()
  ) {

    DebugLog::println(
      "ERROR: board init failed"
    );

    return;
  }


  DebugLog::println(
    "Board initialized"
  );


  // --------------------------------------------------
  // LCD
  // --------------------------------------------------

  lcd =
    board->getLCD();


  if (!lcd) {

    DebugLog::println(
      "ERROR: LCD not found"
    );

    return;
  }


  DebugLog::println(
    "LCD found"
  );


  // --------------------------------------------------
  // RGB bus
  // --------------------------------------------------

  auto bus =
    lcd->getBus();


  if (
    bus->getBasicAttributes().type ==
    ESP_PANEL_BUS_TYPE_RGB
  ) {

    DebugLog::println(
      "RGB bus detected"
    );


    auto rgb =
      static_cast<BusRGB*>(
        bus
      );


    rgb->configRGB_BounceBufferSize(
      lcd->getFrameWidth() * 20
    );


    DebugLog::println(
      "RGB buffer configured"
    );
  }


  // --------------------------------------------------
  // Board begin
  // --------------------------------------------------

  if (
    !board->begin()
  ) {

    DebugLog::println(
      "ERROR: board begin failed"
    );

    return;
  }


  DebugLog::println(
    "DISPLAY INITIALIZED!"
  );


  // --------------------------------------------------
  // Debug dashboard
  // --------------------------------------------------

  if (
    !debugDisplay.begin(
      lcd
    )
  ) {

    DebugLog::println(
      "ERROR: Debug display init failed"
    );

    return;
  }


  // --------------------------------------------------
  // RTC
  // --------------------------------------------------
  //
  // ESP32_Display_Panel owns I2C_NUM_0.
  //
  // PCF85063 uses the same existing host.
  //
  // No Wire.begin().
  // No SensorLib.
  // No I2C driver installation.
  // --------------------------------------------------

  DebugLog::println(
    "Initializing PCF85063 RTC..."
  );


  if (
    rtc.begin()
  ) {

    DebugLog::println(
      "RTC found"
    );


    if (
      readRtc()
    ) {

      DebugLog::printf(
        "RTC time: %04d-%02d-%02d %02d:%02d:%02d\n",

        rtcDateTime.year,
        rtcDateTime.month,
        rtcDateTime.day,
        rtcDateTime.hour,
        rtcDateTime.minute,
        rtcDateTime.second
      );

    } else {

      DebugLog::println(
        "RTC found, but time is invalid"
      );
    }

  } else {

    DebugLog::println(
      "RTC: PCF85063 not found"
    );


    rtcValid =
      false;
  }


  // --------------------------------------------------
  // Initialization time
  // --------------------------------------------------

  initTimeMs =
    millis() -
    bootStart;


  DebugLog::printf(
    "[BOOT] TOTAL INIT = %lu ms\n",
    initTimeMs
  );


  // --------------------------------------------------
  // Initial dashboard
  // --------------------------------------------------

  buildDashboard();


  // --------------------------------------------------
  // WiFi
  // --------------------------------------------------

  connectWiFiAtStartup();


  // --------------------------------------------------
  // NTP / RTC synchronization
  //
  // Only set RTC when it is currently invalid.
  // --------------------------------------------------

  if (
    WiFi.status() ==
    WL_CONNECTED
  ) {

    startNtp();


    if (
      !rtcValid
    ) {

      syncRtcFromNtp();
    }
  }


  // --------------------------------------------------
  // Final dashboard update
  // --------------------------------------------------

  readRtc();

  updateConnectionDashboard();

  updateRtcDashboard();


  debugDisplay.setRuntime(
    formatRuntime(
      millis()
    )
  );


  debugDisplay.refresh();
}


// ==================================================
// Loop
// ==================================================

void loop() {

  const bool wifiConnected =
    WiFi.status() ==
    WL_CONNECTED;


  // --------------------------------------------------
  // Newly connected
  // --------------------------------------------------

  if (
    wifiConnected &&
    !wifiWasConnected
  ) {

    showWiFiConnection();


    debugDisplay.refresh();
  }


  // --------------------------------------------------
  // Disconnected
  // --------------------------------------------------

  if (
    !wifiConnected &&
    wifiWasConnected
  ) {

    DebugLog::println(
      "WiFi disconnected"
    );


    wifiWasConnected =
      false;


    updateConnectionDashboard();


    debugDisplay.refresh();
  }


  // --------------------------------------------------
  // Reconnect
  // --------------------------------------------------

  if (
    !wifiConnected &&
    millis() -
      lastWiFiReconnectAttempt >=
        WIFI_RECONNECT_INTERVAL_MS
  ) {

    ++wifiReconnectCount;


    startWiFiConnection(
      "WiFi reconnecting...",
      "RECONNECTING"
    );


    debugDisplay.setSystemLog(
      "OK",
      "OK",
      "OK",
      String(
        wifiReconnectCount
      )
    );


    debugDisplay.refresh();
  }


  // --------------------------------------------------
  // Dashboard
  // --------------------------------------------------

  updateDashboard();


  // --------------------------------------------------
  // Main loop timing
  // --------------------------------------------------

  delay(100);
}