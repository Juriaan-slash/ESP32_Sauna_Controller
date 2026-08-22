#pragma once

#include <Arduino.h>
#include <esp_display_panel.hpp>
#include "SysteemStatus.h"


class SaunaDebugDisplay
{
public:

    bool begin(
        esp_panel::drivers::LCD* lcd
    );

    void clear();


    // ==================================================
    // System
    // ==================================================

    void setSystem(
        const String& board,
        const String& display,
        const String& touch,
        const String& initTime,
        const String& runtime
    );


    // ==================================================
    // Connection
    // ==================================================

    void setConnection(
        const String& wifi,
        SysteemStatus::ValueStatus wifiStatus,
        const String& ssid,
        const String& ip,
        const String& rssi,
        SysteemStatus::ValueStatus rssiStatus,
        const String& rtc
    );


    // ==================================================
    // Sensors
    // ==================================================

    void setSensors(
        const String& ds2484,
        const String& kType,
        const String& co,
        const String& smoke
    );


    // ==================================================
    // Test values
    // ==================================================

    void setTestValues(
        const String& temperature,
        SysteemStatus::ValueStatus temperatureStatus,
        const String& humidity,
        SysteemStatus::ValueStatus humidityStatus,
        const String& pressure,
        SysteemStatus::ValueStatus pressureStatus
    );


    // ==================================================
    // System log
    // ==================================================

    void setSystemLog(
        const String& rgbBuffer,
        const String& display,
        const String& usbCdc,
        const String& wifiReconnects
    );


    // ==================================================
    // Hardware
    // ==================================================

    void setHardware(
        const String& psram,
        const String& freeRam,
        const String& flash,
        const String& cpu,
        const String& minFreeRam
    );


    // ==================================================
    // Date / Time
    // ==================================================

    void setDateTime(
        const String& text,
        const String& rtcStatus
    );


    // ==================================================
    // Runtime
    // ==================================================

    void setRuntime(
        const String& text
    );


    void refresh();


private:

    // ==================================================
    // Hardware
    // ==================================================

    esp_panel::drivers::LCD* _lcd = nullptr;
    uint16_t* _buffer = nullptr;

    int _width = 0;
    int _height = 0;

    bool _dirty = true;


    // ==================================================
    // Drawing
    // ==================================================

    void redraw();

    void pixel(
        int x,
        int y,
        uint16_t color
    );

    void drawChar(
        int x,
        int y,
        char c,
        uint16_t color
    );

    void drawString(
        int x,
        int y,
        const String& text,
        uint16_t color
    );

    void fillRect(
        int x,
        int y,
        int w,
        int h,
        uint16_t color
    );

    void drawSection(
        int x,
        int y,
        const String& title
    );

    void drawField(
        int x,
        int y,
        const String& label,
        const String& value,
        SysteemStatus::ValueStatus status =
            SysteemStatus::ValueStatus::VALID
    );

    uint16_t colorForStatus(
        SysteemStatus::ValueStatus status
    );

    void glyph(
        char c,
        uint8_t g[5]
    );


    // ==================================================
    // System
    // ==================================================

    String _systemBoard = "OK";
    String _systemDisplay = "OK";
    String _systemTouch = "OK";

    String _initTime = "---";
    String _runtime = "00:00:00";


    // ==================================================
    // Connection
    // ==================================================

    String _wifi = "WAITING";

    SysteemStatus::ValueStatus _wifiStatus =
        SysteemStatus::ValueStatus::INVALID;

    String _ssid = "---";
    String _ip = "---";
    String _rssi = "---";
    SysteemStatus::ValueStatus _rssiStatus =
        SysteemStatus::ValueStatus::INVALID;
    String _rtc = "---";


    // ==================================================
    // Sensors
    // ==================================================

    String _ds2484 = "WAITING";
    String _kType = "WAITING";
    String _co = "--";
    String _smoke = "--";


    // ==================================================
    // Test values
    // ==================================================

    String _temperature = "23.4 C";
    String _humidity = "48.0 %";
    String _pressure = "1012.0 hPa";

    SysteemStatus::ValueStatus _temperatureStatus =
        SysteemStatus::ValueStatus::INVALID;

    SysteemStatus::ValueStatus _humidityStatus =
        SysteemStatus::ValueStatus::INVALID;

    SysteemStatus::ValueStatus _pressureStatus =
        SysteemStatus::ValueStatus::INVALID;


    // ==================================================
    // System log
    // ==================================================

    String _rgbBuffer = "OK";
    String _displayLog = "OK";
    String _usbCdc = "OK";
    String _wifiReconnects = "0";


    // ==================================================
    // Hardware
    // ==================================================

    String _psram = "OK";
    String _freeRam = "---";
    String _flash = "---";
    String _cpu = "---";
    String _minFreeRam = "---";


    // ==================================================
    // Date / Time
    // ==================================================

    String _dateTime = "RTC INVALID";


    // ==================================================
    // Colors
    // ==================================================

    uint16_t _background = 0x0000;
    uint16_t _foreground = 0xFFFF;
    uint16_t _headerTextColor = 0x0320;
    uint16_t _dividerColor = 0x7BEF;

    uint16_t _invalidColor = 0x8410;
    uint16_t _errorColor   = 0x8010;
    uint16_t _warningColor = 0xFD20;
    uint16_t _panicColor   = 0xF800;


    // ==================================================
    // Layout
    // ==================================================

    static constexpr int CHAR_W = 6;
    static constexpr int SCALE = 2;
    static constexpr int LINE_HEIGHT = 22;
    static constexpr int HEADER_H = 44;

    static constexpr int LEFT_X = 16;
    static constexpr int RIGHT_X = 410;

    static constexpr int VALUE_OFFSET = 180;
    static constexpr int DIVIDER_X = 400;

    static constexpr int BODY_Y =
        HEADER_H + 8;
};
