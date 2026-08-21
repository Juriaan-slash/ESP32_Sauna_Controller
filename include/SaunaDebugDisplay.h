#pragma once

#include <Arduino.h>
#include <esp_display_panel.hpp>

class SaunaDebugDisplay
{
public:

    bool begin(
        esp_panel::drivers::LCD* lcd
    );

    void clear();

    void setSystem(
        const String& board,
        const String& display,
        const String& touch,
        const String& initTime,
        const String& runtime
    );

    void setConnection(
        const String& wifi,
        const String& ssid,
        const String& ip,
        const String& rssi,
        const String& rtc
    );

    void setSensors(
        const String& ds2484,
        const String& kType,
        const String& co,
        const String& smoke
    );

    void setTestValues(
        const String& temperature,
        const String& humidity,
        const String& pressure
    );

    void setSystemLog(
        const String& rgbBuffer,
        const String& display,
        const String& usbCdc,
        const String& wifiReconnects
    );

    void setHardware(
        const String& psram,
        const String& freeRam,
        const String& flash,
        const String& cpu,
        const String& minFreeRam
    );

    void setDateTime(
        const String& text,
        const String& rtcStatus
    );

    void setRuntime(
        const String& text
    );

    void refresh();


private:

    // Hardware
    esp_panel::drivers::LCD* _lcd = nullptr;
    uint16_t* _buffer = nullptr;

    int _width = 0;
    int _height = 0;

    bool _dirty = true;


    // Drawing
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
        const String& value
    );

    void glyph(
        char c,
        uint8_t g[5]
    );


    // System
    String _systemBoard = "OK";
    String _systemDisplay = "OK";
    String _systemTouch = "OK";

    String _initTime = "---";
    String _runtime = "00:00:00";


    // Connection
    String _wifi = "WAITING";
    String _ssid = "---";
    String _ip = "---";
    String _rssi = "---";
    String _rtc = "---";


    // Sensors
    String _ds2484 = "WAITING";
    String _kType = "WAITING";
    String _co = "--";
    String _smoke = "--";


    // Test values
    String _temperature = "23.4 C";
    String _humidity = "48.0 %";
    String _pressure = "1012.0 hPa";


    // System log
    String _rgbBuffer = "OK";
    String _displayLog = "OK";
    String _usbCdc = "OK";
    String _wifiReconnects = "0";


    // Hardware
    String _psram = "OK";
    String _freeRam = "---";
    String _flash = "---";
    String _cpu = "---";
    String _minFreeRam = "---";


    // Date/time
    String _dateTime = "RTC INVALID";


    // Colors
    uint16_t _background = 0x0000;
    uint16_t _foreground = 0xFFFF;
    uint16_t _headerTextColor = 0x0320;
    uint16_t _dividerColor = 0x7BEF;


    // Layout
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