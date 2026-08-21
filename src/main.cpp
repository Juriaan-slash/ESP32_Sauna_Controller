#include <Arduino.h>
#include <esp_display_panel.hpp>

#include "DebugLog.h"
#include "LCDDisplay.h"
#include "SysteemStatus.h"


using namespace esp_panel::board;
using namespace esp_panel::drivers;


// ==================================================
// Board / physical LCD
// ==================================================

Board* board = nullptr;

esp_panel::drivers::LCD* lcd = nullptr;


// ==================================================
// Boot timing
// ==================================================

unsigned long bootStart = 0;


// ==================================================
// Setup
// ==================================================

void setup()
{
    bootStart = millis();


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


    if (!board->init())
    {
        DebugLog::println(
            "ERROR: board init failed"
        );

        return;
    }


    DebugLog::println(
        "Board initialized"
    );


    // --------------------------------------------------
    // Physical LCD
    // --------------------------------------------------

    lcd =
        board->getLCD();


    if (!lcd)
    {
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
    )
    {
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

    if (!board->begin())
    {
        DebugLog::println(
            "ERROR: board begin failed"
        );

        return;
    }


    DebugLog::println(
        "DISPLAY INITIALIZED!"
    );


    // --------------------------------------------------
    // SysteemStatus
    // --------------------------------------------------

    SysteemStatus::begin();


    // --------------------------------------------------
    // LCD presentation layer
    // --------------------------------------------------

    if (!LCDDisplay::begin(lcd))
    {
        DebugLog::println(
            "ERROR: LCD display init failed"
        );

        return;
    }


    // --------------------------------------------------
    // RTC
    // --------------------------------------------------

    SysteemStatus::initializeRtc();


    // --------------------------------------------------
    // Initialization time
    // --------------------------------------------------

    const unsigned long initTime =
        millis() - bootStart;


    SysteemStatus::setInitTime(
        initTime
    );


    DebugLog::printf(
        "[BOOT] TOTAL INIT = %lu ms\n",
        initTime
    );


    // --------------------------------------------------
    // Initial dashboard
    // --------------------------------------------------

    SysteemStatus::update();

    SysteemStatus::updateRtc();

    LCDDisplay::update();
    LCDDisplay::refresh();


    // --------------------------------------------------
    // WiFi
    // --------------------------------------------------

    SysteemStatus::initializeWiFi();


    // --------------------------------------------------
    // NTP / RTC synchronization
    // --------------------------------------------------

    SysteemStatus::updateRtc();


    // --------------------------------------------------
    // Final dashboard
    // --------------------------------------------------

    SysteemStatus::update();

    SysteemStatus::updateRtc();

    LCDDisplay::update();
    LCDDisplay::refresh();
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
    // LCD
    // --------------------------------------------------

    LCDDisplay::update();

    LCDDisplay::refresh();


    // --------------------------------------------------
    // Loop timing
    // --------------------------------------------------

    delay(100);
}