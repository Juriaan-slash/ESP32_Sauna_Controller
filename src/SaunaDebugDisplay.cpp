#include "SaunaDebugDisplay.h"

#include <esp_heap_caps.h>

#include "DebugLog.h"


// ==================================================
// Character glyphs
// ==================================================

void SaunaDebugDisplay::glyph(
    char c,
    uint8_t g[5]
)
{
    memset(g, 0, 5);

    switch (c) {

        case 'A': g[0]=0x7E; g[1]=0x09; g[2]=0x09; g[3]=0x09; g[4]=0x7E; break;
        case 'B': g[0]=0x7F; g[1]=0x49; g[2]=0x49; g[3]=0x49; g[4]=0x36; break;
        case 'C': g[0]=0x3E; g[1]=0x41; g[2]=0x41; g[3]=0x41; g[4]=0x22; break;
        case 'D': g[0]=0x7F; g[1]=0x41; g[2]=0x41; g[3]=0x22; g[4]=0x1C; break;
        case 'E': g[0]=0x7F; g[1]=0x49; g[2]=0x49; g[3]=0x49; g[4]=0x41; break;
        case 'F': g[0]=0x7F; g[1]=0x09; g[2]=0x09; g[3]=0x09; g[4]=0x01; break;
        case 'G': g[0]=0x3E; g[1]=0x41; g[2]=0x49; g[3]=0x49; g[4]=0x7A; break;
        case 'H': g[0]=0x7F; g[1]=0x08; g[2]=0x08; g[3]=0x08; g[4]=0x7F; break;
        case 'I': g[0]=0x41; g[1]=0x41; g[2]=0x7F; g[3]=0x41; g[4]=0x41; break;
        case 'J': g[0]=0x20; g[1]=0x40; g[2]=0x41; g[3]=0x3F; g[4]=0x01; break;
        case 'K': g[0]=0x7F; g[1]=0x08; g[2]=0x14; g[3]=0x22; g[4]=0x41; break;
        case 'L': g[0]=0x7F; g[1]=0x40; g[2]=0x40; g[3]=0x40; g[4]=0x40; break;
        case 'M': g[0]=0x7F; g[1]=0x02; g[2]=0x0C; g[3]=0x02; g[4]=0x7F; break;
        case 'N': g[0]=0x7F; g[1]=0x04; g[2]=0x08; g[3]=0x10; g[4]=0x7F; break;
        case 'O': g[0]=0x3E; g[1]=0x41; g[2]=0x41; g[3]=0x41; g[4]=0x3E; break;
        case 'P': g[0]=0x7F; g[1]=0x09; g[2]=0x09; g[3]=0x09; g[4]=0x06; break;
        case 'Q': g[0]=0x3E; g[1]=0x41; g[2]=0x51; g[3]=0x21; g[4]=0x5E; break;
        case 'R': g[0]=0x7F; g[1]=0x09; g[2]=0x19; g[3]=0x29; g[4]=0x46; break;
        case 'S': g[0]=0x46; g[1]=0x49; g[2]=0x49; g[3]=0x49; g[4]=0x31; break;
        case 'T': g[0]=0x01; g[1]=0x01; g[2]=0x7F; g[3]=0x01; g[4]=0x01; break;
        case 'U': g[0]=0x3F; g[1]=0x40; g[2]=0x40; g[3]=0x40; g[4]=0x3F; break;
        case 'V': g[0]=0x1F; g[1]=0x20; g[2]=0x40; g[3]=0x20; g[4]=0x1F; break;
        case 'W': g[0]=0x3F; g[1]=0x40; g[2]=0x38; g[3]=0x40; g[4]=0x3F; break;
        case 'X': g[0]=0x63; g[1]=0x14; g[2]=0x08; g[3]=0x14; g[4]=0x63; break;
        case 'Y': g[0]=0x07; g[1]=0x08; g[2]=0x70; g[3]=0x08; g[4]=0x07; break;
        case 'Z': g[0]=0x61; g[1]=0x51; g[2]=0x49; g[3]=0x45; g[4]=0x43; break;

        case 'a': g[0]=0x20; g[1]=0x54; g[2]=0x54; g[3]=0x54; g[4]=0x78; break;
        case 'b': g[0]=0x7F; g[1]=0x48; g[2]=0x44; g[3]=0x44; g[4]=0x38; break;
        case 'c': g[0]=0x38; g[1]=0x44; g[2]=0x44; g[3]=0x44; g[4]=0x20; break;
        case 'd': g[0]=0x38; g[1]=0x44; g[2]=0x44; g[3]=0x48; g[4]=0x7F; break;
        case 'e': g[0]=0x38; g[1]=0x54; g[2]=0x54; g[3]=0x54; g[4]=0x18; break;
        case 'f': g[0]=0x08; g[1]=0x7E; g[2]=0x09; g[3]=0x01; g[4]=0x02; break;
        case 'g': g[0]=0x0C; g[1]=0x52; g[2]=0x52; g[3]=0x52; g[4]=0x3E; break;
        case 'h': g[0]=0x7F; g[1]=0x08; g[2]=0x04; g[3]=0x04; g[4]=0x78; break;
        case 'i': g[0]=0x00; g[1]=0x44; g[2]=0x7D; g[3]=0x40; g[4]=0x00; break;
        case 'j': g[0]=0x20; g[1]=0x40; g[2]=0x44; g[3]=0x3D; g[4]=0x00; break;
        case 'k': g[0]=0x7F; g[1]=0x10; g[2]=0x28; g[3]=0x44; g[4]=0x00; break;
        case 'l': g[0]=0x00; g[1]=0x41; g[2]=0x7F; g[3]=0x40; g[4]=0x00; break;
        case 'm': g[0]=0x7C; g[1]=0x04; g[2]=0x18; g[3]=0x04; g[4]=0x78; break;
        case 'n': g[0]=0x7C; g[1]=0x08; g[2]=0x04; g[3]=0x04; g[4]=0x78; break;
        case 'o': g[0]=0x38; g[1]=0x44; g[2]=0x44; g[3]=0x44; g[4]=0x38; break;
        case 'p': g[0]=0x7C; g[1]=0x14; g[2]=0x14; g[3]=0x14; g[4]=0x08; break;
        case 'q': g[0]=0x08; g[1]=0x14; g[2]=0x14; g[3]=0x18; g[4]=0x7C; break;
        case 'r': g[0]=0x7C; g[1]=0x08; g[2]=0x04; g[3]=0x04; g[4]=0x08; break;
        case 's': g[0]=0x48; g[1]=0x54; g[2]=0x54; g[3]=0x54; g[4]=0x20; break;
        case 't': g[0]=0x04; g[1]=0x3F; g[2]=0x44; g[3]=0x40; g[4]=0x20; break;
        case 'u': g[0]=0x3C; g[1]=0x40; g[2]=0x40; g[3]=0x20; g[4]=0x7C; break;
        case 'v': g[0]=0x1C; g[1]=0x20; g[2]=0x40; g[3]=0x20; g[4]=0x1C; break;
        case 'w': g[0]=0x3C; g[1]=0x40; g[2]=0x30; g[3]=0x40; g[4]=0x3C; break;
        case 'x': g[0]=0x44; g[1]=0x28; g[2]=0x10; g[3]=0x28; g[4]=0x44; break;
        case 'y': g[0]=0x0C; g[1]=0x50; g[2]=0x50; g[3]=0x50; g[4]=0x3C; break;
        case 'z': g[0]=0x44; g[1]=0x64; g[2]=0x54; g[3]=0x4C; g[4]=0x44; break;

        case '0': g[0]=0x3E; g[1]=0x45; g[2]=0x49; g[3]=0x51; g[4]=0x3E; break;
        case '1': g[0]=0x00; g[1]=0x21; g[2]=0x7F; g[3]=0x01; g[4]=0x00; break;
        case '2': g[0]=0x23; g[1]=0x45; g[2]=0x49; g[3]=0x51; g[4]=0x21; break;
        case '3': g[0]=0x22; g[1]=0x41; g[2]=0x49; g[3]=0x49; g[4]=0x36; break;
        case '4': g[0]=0x0C; g[1]=0x14; g[2]=0x24; g[3]=0x7F; g[4]=0x04; break;
        case '5': g[0]=0x72; g[1]=0x51; g[2]=0x51; g[3]=0x51; g[4]=0x4E; break;
        case '6': g[0]=0x3E; g[1]=0x49; g[2]=0x49; g[3]=0x49; g[4]=0x06; break;
        case '7': g[0]=0x40; g[1]=0x47; g[2]=0x48; g[3]=0x50; g[4]=0x60; break;
        case '8': g[0]=0x36; g[1]=0x49; g[2]=0x49; g[3]=0x49; g[4]=0x36; break;
        case '9': g[0]=0x30; g[1]=0x49; g[2]=0x49; g[3]=0x49; g[4]=0x3E; break;

        case ':': g[0]=0x00; g[1]=0x36; g[2]=0x36; g[3]=0x00; g[4]=0x00; break;
        case '.': g[0]=0x00; g[1]=0x40; g[2]=0x60; g[3]=0x00; g[4]=0x00; break;
        case '-': g[0]=0x08; g[1]=0x08; g[2]=0x08; g[3]=0x08; g[4]=0x08; break;
        case '_': g[0]=0x40; g[1]=0x40; g[2]=0x40; g[3]=0x40; g[4]=0x40; break;
        case '/': g[0]=0x01; g[1]=0x06; g[2]=0x08; g[3]=0x30; g[4]=0x40; break;
        case '%': g[0]=0x63; g[1]=0x13; g[2]=0x08; g[3]=0x64; g[4]=0x63; break;
        case '+': g[0]=0x08; g[1]=0x08; g[2]=0x3E; g[3]=0x08; g[4]=0x08; break;
        case '(': g[0]=0x00; g[1]=0x1C; g[2]=0x22; g[3]=0x41; g[4]=0x00; break;
        case ')': g[0]=0x00; g[1]=0x41; g[2]=0x22; g[3]=0x1C; g[4]=0x00; break;

        default:
            break;
    }
}


// ==================================================
// Drawing
// ==================================================

void SaunaDebugDisplay::pixel(
    int x,
    int y,
    uint16_t color
)
{
    if (
        !_buffer ||
        x < 0 ||
        y < 0 ||
        x >= _width ||
        y >= _height
    ) {
        return;
    }

    _buffer[
        y * _width + x
    ] = color;
}


void SaunaDebugDisplay::drawChar(
    int x,
    int y,
    char c,
    uint16_t color
)
{
    uint8_t g[5];

    glyph(c, g);

    const bool flipVertical =
        c >= '0' &&
        c <= '9';

    for (int col = 0; col < 5; ++col) {

        for (int row = 0; row < 7; ++row) {

            const int sourceRow =
                flipVertical
                    ? 6 - row
                    : row;

            if (
                g[col] &
                (1 << sourceRow)
            ) {

                for (int dx = 0; dx < SCALE; ++dx) {

                    for (int dy = 0; dy < SCALE; ++dy) {

                        pixel(
                            x + col * SCALE + dx,
                            y + row * SCALE + dy,
                            color
                        );
                    }
                }
            }
        }
    }
}


void SaunaDebugDisplay::drawString(
    int x,
    int y,
    const String& text,
    uint16_t color
)
{
    int cursor = x;

    for (
        size_t i = 0;
        i < text.length();
        ++i
    ) {

        drawChar(
            cursor,
            y,
            text[i],
            color
        );

        cursor +=
            CHAR_W * SCALE;
    }
}


void SaunaDebugDisplay::fillRect(
    int x,
    int y,
    int w,
    int h,
    uint16_t color
)
{
    const int x0 =
        max(0, x);

    const int y0 =
        max(0, y);

    const int x1 =
        min(_width, x + w);

    const int y1 =
        min(_height, y + h);

    for (int yy = y0; yy < y1; ++yy) {

        for (int xx = x0; xx < x1; ++xx) {

            _buffer[
                yy * _width + xx
            ] = color;
        }
    }
}


void SaunaDebugDisplay::drawSection(
    int x,
    int y,
    const String& title
)
{
    drawString(
        x,
        y,
        title,
        _headerTextColor
    );
}


void SaunaDebugDisplay::drawField(
    int x,
    int y,
    const String& label,
    const String& value
)
{
    drawString(
        x,
        y,
        label,
        _foreground
    );

    drawString(
        x + VALUE_OFFSET,
        y,
        value,
        _foreground
    );
}


// ==================================================
// Redraw
// ==================================================

void SaunaDebugDisplay::redraw()
{
    if (
        !_buffer ||
        !_lcd
    ) {
        return;
    }

    fillRect(
        0,
        0,
        _width,
        _height,
        _background
    );


    // ------------------------------------------------
    // Header
    // ------------------------------------------------

    drawString(
        LEFT_X,
        6,
        "SAUNA DEBUG",
        _headerTextColor
    );

    drawString(
        560,
        4,
        _dateTime,
        _headerTextColor
    );


    // ------------------------------------------------
    // Divider
    // ------------------------------------------------

    fillRect(
        DIVIDER_X,
        HEADER_H,
        2,
        _height - HEADER_H,
        _dividerColor
    );


    // =================================================
    // Left column
    // =================================================

    int y = BODY_Y;


    // -------------------------------------------------
    // System
    // -------------------------------------------------

    drawSection(
        LEFT_X,
        y,
        "SYSTEM"
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "Board",
        _systemBoard
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "Display",
        _systemDisplay
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "Touch",
        _systemTouch
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "Init time",
        _initTime
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "Runtime",
        _runtime
    );

    y += LINE_HEIGHT + 6;


    // -------------------------------------------------
    // Sensors
    // -------------------------------------------------

    drawSection(
        LEFT_X,
        y,
        "SENSORS"
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "DS2484",
        _ds2484
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "K-Type",
        _kType
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "CO",
        _co
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "Smoke",
        _smoke
    );

    y += LINE_HEIGHT + 6;


    // -------------------------------------------------
    // System log
    // -------------------------------------------------

    drawSection(
        LEFT_X,
        y,
        "SYSTEM LOG"
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "RGB buffer",
        _rgbBuffer
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "Display",
        _displayLog
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "USB CDC",
        _usbCdc
    );

    y += LINE_HEIGHT;

    drawField(
        LEFT_X,
        y,
        "WiFi retries",
        _wifiReconnects
    );


    // =================================================
    // Right column
    // =================================================

    y = BODY_Y;


    // -------------------------------------------------
    // Connection
    // -------------------------------------------------

    drawSection(
        RIGHT_X,
        y,
        "CONNECTION"
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "WiFi",
        _wifi
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "SSID",
        _ssid
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "IP",
        _ip
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "RSSI",
        _rssi
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "RTC",
        _rtc
    );

    y += LINE_HEIGHT + 6;


    // -------------------------------------------------
    // Test values
    // -------------------------------------------------

    drawSection(
        RIGHT_X,
        y,
        "TEST VALUES"
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "Temperature",
        _temperature
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "Humidity",
        _humidity
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "Pressure",
        _pressure
    );

    y += LINE_HEIGHT + 6;


    // -------------------------------------------------
    // Hardware
    // -------------------------------------------------

    drawSection(
        RIGHT_X,
        y,
        "HARDWARE"
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "PSRAM",
        _psram
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "Free RAM",
        _freeRam
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "Flash",
        _flash
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "CPU",
        _cpu
    );

    y += LINE_HEIGHT;

    drawField(
        RIGHT_X,
        y,
        "Min Free RAM",
        _minFreeRam
    );


    // -------------------------------------------------
    // Send framebuffer
    // -------------------------------------------------

    _lcd->drawBitmap(
        0,
        0,
        _width,
        _height,
        reinterpret_cast<const uint8_t*>(
            _buffer
        )
    );

    _dirty = false;
}


// ==================================================
// Public interface
// ==================================================

bool SaunaDebugDisplay::begin(
    esp_panel::drivers::LCD* lcd
)
{
    _lcd = lcd;

    if (!_lcd) {
        return false;
    }

    _width =
        _lcd->getFrameWidth();

    _height =
        _lcd->getFrameHeight();


    _buffer =
        reinterpret_cast<uint16_t*>(
            heap_caps_malloc(
                _width *
                _height *
                sizeof(uint16_t),
                MALLOC_CAP_SPIRAM
            )
        );


    if (!_buffer) {
        return false;
    }


    clear();

    return true;
}


void SaunaDebugDisplay::clear()
{
    _dirty = true;

    _initTime = "---";

    _runtime = "00:00:00";

    _dateTime = "RTC INVALID";
}


void SaunaDebugDisplay::setSystem(
    const String& board,
    const String& display,
    const String& touch,
    const String& initTime,
    const String& runtime
)
{
    _systemBoard = board;
    _systemDisplay = display;
    _systemTouch = touch;

    _initTime = initTime;
    _runtime = runtime;

    _dirty = true;
}


void SaunaDebugDisplay::setConnection(
    const String& wifi,
    const String& ssid,
    const String& ip,
    const String& rssi,
    const String& rtc
)
{
    _wifi = wifi;
    _ssid = ssid;
    _ip = ip;
    _rssi = rssi;
    _rtc = rtc;

    _dirty = true;
}


void SaunaDebugDisplay::setSensors(
    const String& ds2484,
    const String& kType,
    const String& co,
    const String& smoke
)
{
    _ds2484 = ds2484;
    _kType = kType;
    _co = co;
    _smoke = smoke;

    _dirty = true;
}


void SaunaDebugDisplay::setTestValues(
    const String& temperature,
    const String& humidity,
    const String& pressure
)
{
    _temperature = temperature;
    _humidity = humidity;
    _pressure = pressure;

    _dirty = true;
}


void SaunaDebugDisplay::setSystemLog(
    const String& rgbBuffer,
    const String& display,
    const String& usbCdc,
    const String& wifiReconnects
)
{
    _rgbBuffer = rgbBuffer;
    _displayLog = display;
    _usbCdc = usbCdc;
    _wifiReconnects = wifiReconnects;

    _dirty = true;
}


void SaunaDebugDisplay::setHardware(
    const String& psram,
    const String& freeRam,
    const String& flash,
    const String& cpu,
    const String& minFreeRam
)
{
    _psram = psram;
    _freeRam = freeRam;
    _flash = flash;
    _cpu = cpu;
    _minFreeRam = minFreeRam;

    _dirty = true;
}


void SaunaDebugDisplay::setDateTime(
    const String& text,
    const String& rtcStatus
)
{
    _dateTime = text;
    _rtc = rtcStatus;

    _dirty = true;
}


void SaunaDebugDisplay::setRuntime(
    const String& text
)
{
    _runtime = text;

    _dirty = true;
}


void SaunaDebugDisplay::refresh()
{
    if (_dirty) {
        redraw();
    }
}