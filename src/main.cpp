#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

#include <esp_display_panel.hpp>

#include "DebugLog.h"
#include "PCF85063.h"
#include "SysteemStatus.h"
#include "LCDDisplay.h"
#include "Web.h"


using namespace esp_panel::drivers;


// ==================================================
// Globals
// ==================================================

esp_panel::board::Board* board = nullptr;


// ==================================================
// Setup
// ==================================================

void setup()
{
    const unsigned long bootStart =
        millis();


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


    // =================================================
    // Board
    // =================================================

    board =
        new esp_panel::board::Board();


    if (!board)
    {
        DebugLog::println(
            "ERROR: Board allocation failed"
        );

        return;
    }


    DebugLog::println(
        "Board created"
    );


    // -------------------------------------------------
    // Board initialization
    // -------------------------------------------------

    if (!board->init())
    {
        DebugLog::println(
            "ERROR: Board initialization failed"
        );

        return;
    }


    DebugLog::println(
        "Board initialized"
    );


    // =================================================
    // LCD
    // =================================================

    auto lcd =
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


    auto lcdBus =
        lcd->getBus();


    if (!lcdBus)
    {
        DebugLog::println(
            "ERROR: LCD bus not found"
        );

        return;
    }


    if (
        lcdBus->getBasicAttributes().type ==
        ESP_PANEL_BUS_TYPE_RGB
    )
    {
        DebugLog::println(
            "RGB bus detected"
        );


        // ------------------------------------------------
        // RGB bounce buffer
        //
        // ESP32-S3 RGB LCD:
        // width * 10 pixels
        // ------------------------------------------------

        auto rgbBus =
            static_cast<BusRGB*>(lcdBus);


        const int bounceBufferSize =
            lcd->getFrameWidth() * 10;


        rgbBus->configRGB_BounceBufferSize(
            bounceBufferSize
        );


        DebugLog::print(
            "RGB bounce buffer: "
        );

        DebugLog::print(
            bounceBufferSize
        );

        DebugLog::println(
            " pixels"
        );
    }


    // =================================================
    // Board begin
    // =================================================

    if (!board->begin())
    {
        DebugLog::println(
            "ERROR: Board begin failed"
        );

        return;
    }


    DebugLog::println(
        "DISPLAY INITIALIZED!"
    );


    // =================================================
    // LCD debug display
    // =================================================

    if (
        !LCDDisplay::begin(lcd)
    )
    {
        DebugLog::println(
            "ERROR: LCDDisplay initialization failed"
        );

        return;
    }


    LCDDisplay::update();
    LCDDisplay::refresh();


    // =================================================
    // RTC
    // =================================================

    DebugLog::println(
        "Initializing PCF85063 RTC..."
    );


    SysteemStatus::initializeRtc();


    // =================================================
    // System initialization time
    // =================================================

    SysteemStatus::setInitTime(
        millis() - bootStart
    );


    DebugLog::print(
        "[BOOT] TOTAL INIT = "
    );

    DebugLog::print(
        millis() - bootStart
    );

    DebugLog::println(
        " ms"
    );


    // =================================================
    // WiFi
    // =================================================

    SysteemStatus::initializeWiFi();


    // =================================================
    // Web
    // =================================================

    Web::begin();


    // =================================================
    // Initial system update
    // =================================================

    SysteemStatus::update();

    LCDDisplay::update();
    LCDDisplay::refresh();
}


// ==================================================
// Loop
// ==================================================

void loop()
{
    // -------------------------------------------------
    // System status
    // -------------------------------------------------

    SysteemStatus::update();


    // -------------------------------------------------
    // RTC
    // -------------------------------------------------

    SysteemStatus::updateRtc();


    // -------------------------------------------------
    // Web
    // -------------------------------------------------

    Web::update();


    // -------------------------------------------------
    // LCD
    //
    // Update once per second.
    // -------------------------------------------------

    static unsigned long lastLcdUpdate = 0;


    if (
        millis() - lastLcdUpdate >= 1000
    )
    {
        lastLcdUpdate =
            millis();


        LCDDisplay::update();

        LCDDisplay::refresh();
    }


    delay(1);
}