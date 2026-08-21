#include <Arduino.h>
#include <WiFi.h>
#include <esp_display_panel.hpp>

#include "DebugLog.h"
#include "SaunaDebugDisplay.h"
#include "SysteemStatus.h"
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
// Dashboard timing
// ==================================================

unsigned long initTimeMs = 0;
unsigned long lastDashboardUpdate = 0;


// ==================================================
// Formatting helpers
// ==================================================

static String formatRuntime(
    unsigned long milliseconds
)
{
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
)
{
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
// RTC display
// ==================================================

static void updateRtcDashboard()
{
    const auto& status =
        SysteemStatus::get();


    if (status.rtcValid) {

        debugDisplay.setDateTime(
            formatRtcDateTime(
                status.rtcDateTime
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
// WiFi display
// ==================================================

static void updateConnectionDashboard()
{
    const auto& status =
        SysteemStatus::get();


    if (status.wifiConnected) {

        debugDisplay.setConnection(
            "OK",
            WIFI_SSID,
            status.wifiIp,
            String(status.wifiRssi) +
                " dBm",
            status.rtcValid
                ? "OK"
                : "INVALID"
        );

    } else {

        debugDisplay.setConnection(
            "WAITING",
            WIFI_SSID,
            "---",
            "---",
            status.rtcValid
                ? "OK"
                : "INVALID"
        );
    }
}


// ==================================================
// Dashboard
// ==================================================

static void buildDashboard()
{
    const auto& status =
        SysteemStatus::get();


    debugDisplay.setSystem(
        "OK",
        "OK",
        "OK",
        String(initTimeMs) +
            " ms",
        formatRuntime(
            millis()
        )
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
        String(
            status.wifiReconnectCount
        )
    );


    debugDisplay.setHardware(
        status.psram
            ? "OK"
            : "NO",

        String(
            status.freeHeap
        ) + " B",

        String(
            status.flashSizeMb
        ) + " MB",

        String(
            status.cpuMHz
        ) + " MHz"
    );


    updateRtcDashboard();


    debugDisplay.refresh();
}


// ==================================================
// Dashboard update
// ==================================================

static void updateDashboard()
{
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
    // System status
    // -----------------------------------------------

    SysteemStatus::update();


    // -----------------------------------------------
    // RTC
    // -----------------------------------------------

    SysteemStatus::updateRtc();


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
    // WiFi display
    // -----------------------------------------------

    updateConnectionDashboard();


    // -----------------------------------------------
    // System log
    // -----------------------------------------------

    const auto& status =
        SysteemStatus::get();


    debugDisplay.setSystemLog(
        "OK",
        "OK",
        "OK",
        String(
            status.wifiReconnectCount
        )
    );


    // -----------------------------------------------
    // Hardware
    // -----------------------------------------------

    debugDisplay.setHardware(
        status.psram
            ? "OK"
            : "NO",

        String(
            status.freeHeap
        ) + " B",

        String(
            status.flashSizeMb
        ) + " MB",

        String(
            status.cpuMHz
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

void setup()
{
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
    // SysteemStatus
    // --------------------------------------------------

    SysteemStatus::begin();


    // --------------------------------------------------
    // RTC
    // --------------------------------------------------

    SysteemStatus::initializeRtc();


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

    SysteemStatus::initializeWiFi();


    // --------------------------------------------------
    // NTP / RTC synchronization
    // --------------------------------------------------

    SysteemStatus::updateRtc();


    // --------------------------------------------------
    // Final dashboard update
    // --------------------------------------------------

    SysteemStatus::update();

    SysteemStatus::updateRtc();

    updateRtcDashboard();

    updateConnectionDashboard();


    debugDisplay.setRuntime(
        formatRuntime(
            millis()
        )
    );


    debugDisplay.setSystemLog(
        "OK",
        "OK",
        "OK",
        String(
            SysteemStatus::get().wifiReconnectCount
        )
    );


    debugDisplay.refresh();
}


// ==================================================
// Loop
// ==================================================

void loop()
{
    // --------------------------------------------------
    // System status
    // --------------------------------------------------

    SysteemStatus::update();


    // --------------------------------------------------
    // RTC
    // --------------------------------------------------

    SysteemStatus::updateRtc();


    // --------------------------------------------------
    // Dashboard
    // --------------------------------------------------

    updateDashboard();


    // --------------------------------------------------
    // Main loop timing
    // --------------------------------------------------

    delay(100);
}