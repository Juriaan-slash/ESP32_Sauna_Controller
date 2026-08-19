#pragma once

#include <Arduino.h>
#include <esp_display_panel.hpp>
#include "esp_heap_caps.h"

using namespace esp_panel::drivers;


class SaunaDebugDisplay {

private:

  LCD* _lcd = nullptr;

  uint16_t* _buffer = nullptr;

  int _width = 0;
  int _height = 0;


  // --------------------------------------------------
  // Font
  // --------------------------------------------------

  static const int CHAR_W = 6;
  static const int CHAR_H = 8;
  static const int SCALE = 2;


  // --------------------------------------------------
  // Display layout
  // --------------------------------------------------

  static const int HEADER_H = 36;

  // 22 pixels between lines gives much better readability.
  // 20 lines fit on the 800x480 display.
  static const int LINE_HEIGHT = 22;
  static const int MAX_LINES = 20;
  static const int WIFI_STATUS_Y =
    HEADER_H + 8 + 15 * LINE_HEIGHT;


  String _lines[MAX_LINES];
  String _wifiStatus;

  int _lineCount = 0;


  // --------------------------------------------------
  // RGB565 colours
  // --------------------------------------------------

  uint16_t _background      = 0x0000;  // Black
  uint16_t _foreground      = 0xFFFF; // White
  uint16_t _headerTextColor = 0x0320; // Dark green


  // ==================================================
  // FONT
  // ==================================================

  void glyph(char c, uint8_t g[5]) {

    memset(g, 0, 5);

    switch (c) {

      case 'A':
        g[0]=0x7E; g[1]=0x09; g[2]=0x09;
        g[3]=0x09; g[4]=0x7E;
        break;

      case 'B':
        g[0]=0x7F; g[1]=0x49; g[2]=0x49;
        g[3]=0x49; g[4]=0x36;
        break;

      case 'C':
        g[0]=0x3E; g[1]=0x41; g[2]=0x41;
        g[3]=0x41; g[4]=0x22;
        break;

      case 'D':
        g[0]=0x7F; g[1]=0x41; g[2]=0x41;
        g[3]=0x22; g[4]=0x1C;
        break;

      case 'E':
        g[0]=0x7F; g[1]=0x49; g[2]=0x49;
        g[3]=0x49; g[4]=0x41;
        break;

      case 'F':
        g[0]=0x7F; g[1]=0x09; g[2]=0x09;
        g[3]=0x09; g[4]=0x01;
        break;

      case 'G':
        g[0]=0x3E; g[1]=0x41; g[2]=0x49;
        g[3]=0x49; g[4]=0x7A;
        break;

      case 'H':
        g[0]=0x7F; g[1]=0x08; g[2]=0x08;
        g[3]=0x08; g[4]=0x7F;
        break;

      case 'I':
        g[0]=0x41; g[1]=0x41; g[2]=0x7F;
        g[3]=0x41; g[4]=0x41;
        break;

      case 'J':
        g[0]=0x20; g[1]=0x40; g[2]=0x41;
        g[3]=0x3F; g[4]=0x01;
        break;

      case 'K':
        g[0]=0x7F; g[1]=0x08; g[2]=0x14;
        g[3]=0x22; g[4]=0x41;
        break;

      case 'L':
        g[0]=0x7F; g[1]=0x40; g[2]=0x40;
        g[3]=0x40; g[4]=0x40;
        break;

      case 'M':
        g[0]=0x7F; g[1]=0x02; g[2]=0x0C;
        g[3]=0x02; g[4]=0x7F;
        break;

      case 'N':
        g[0]=0x7F; g[1]=0x04; g[2]=0x08;
        g[3]=0x10; g[4]=0x7F;
        break;

      case 'O':
        g[0]=0x3E; g[1]=0x41; g[2]=0x41;
        g[3]=0x41; g[4]=0x3E;
        break;

      case 'P':
        g[0]=0x7F; g[1]=0x09; g[2]=0x09;
        g[3]=0x09; g[4]=0x06;
        break;

      case 'Q':
        g[0]=0x3E; g[1]=0x41; g[2]=0x51;
        g[3]=0x21; g[4]=0x5E;
        break;

      case 'R':
        g[0]=0x7F; g[1]=0x09; g[2]=0x19;
        g[3]=0x29; g[4]=0x46;
        break;

      case 'S':
        g[0]=0x46; g[1]=0x49; g[2]=0x49;
        g[3]=0x49; g[4]=0x31;
        break;

      case 'T':
        g[0]=0x01; g[1]=0x01; g[2]=0x7F;
        g[3]=0x01; g[4]=0x01;
        break;

      case 'U':
        g[0]=0x3F; g[1]=0x40; g[2]=0x40;
        g[3]=0x40; g[4]=0x3F;
        break;

      case 'V':
        g[0]=0x1F; g[1]=0x20; g[2]=0x40;
        g[3]=0x20; g[4]=0x1F;
        break;

      case 'W':
        g[0]=0x3F; g[1]=0x40; g[2]=0x38;
        g[3]=0x40; g[4]=0x3F;
        break;

      case 'X':
        g[0]=0x63; g[1]=0x14; g[2]=0x08;
        g[3]=0x14; g[4]=0x63;
        break;

      case 'Y':
        g[0]=0x07; g[1]=0x08; g[2]=0x70;
        g[3]=0x08; g[4]=0x07;
        break;

      case 'Z':
        g[0]=0x61; g[1]=0x51; g[2]=0x49;
        g[3]=0x45; g[4]=0x43;
        break;


      // ------------------------------------------------
      // Lowercase letters
      // ------------------------------------------------

      case 'a':
        g[0]=0x20; g[1]=0x54; g[2]=0x54;
        g[3]=0x54; g[4]=0x78;
        break;

      case 'b':
        g[0]=0x7F; g[1]=0x48; g[2]=0x44;
        g[3]=0x44; g[4]=0x38;
        break;

      case 'c':
        g[0]=0x38; g[1]=0x44; g[2]=0x44;
        g[3]=0x44; g[4]=0x20;
        break;

      case 'd':
        g[0]=0x38; g[1]=0x44; g[2]=0x44;
        g[3]=0x48; g[4]=0x7F;
        break;

      case 'e':
        g[0]=0x38; g[1]=0x54; g[2]=0x54;
        g[3]=0x54; g[4]=0x18;
        break;

      case 'f':
        g[0]=0x08; g[1]=0x7E; g[2]=0x09;
        g[3]=0x01; g[4]=0x02;
        break;

      case 'g':
        g[0]=0x0C; g[1]=0x52; g[2]=0x52;
        g[3]=0x52; g[4]=0x3E;
        break;

      case 'h':
        g[0]=0x7F; g[1]=0x08; g[2]=0x04;
        g[3]=0x04; g[4]=0x78;
        break;

      case 'i':
        g[0]=0x00; g[1]=0x44; g[2]=0x7D;
        g[3]=0x40; g[4]=0x00;
        break;

      case 'j':
        g[0]=0x20; g[1]=0x40; g[2]=0x44;
        g[3]=0x3D; g[4]=0x00;
        break;

      case 'k':
        g[0]=0x7F; g[1]=0x10; g[2]=0x28;
        g[3]=0x44; g[4]=0x00;
        break;

      case 'l':
        g[0]=0x00; g[1]=0x41; g[2]=0x7F;
        g[3]=0x40; g[4]=0x00;
        break;

      case 'm':
        g[0]=0x7C; g[1]=0x04; g[2]=0x18;
        g[3]=0x04; g[4]=0x78;
        break;

      case 'n':
        g[0]=0x7C; g[1]=0x08; g[2]=0x04;
        g[3]=0x04; g[4]=0x78;
        break;

      case 'o':
        g[0]=0x38; g[1]=0x44; g[2]=0x44;
        g[3]=0x44; g[4]=0x38;
        break;

      case 'p':
        g[0]=0x7C; g[1]=0x14; g[2]=0x14;
        g[3]=0x14; g[4]=0x08;
        break;

      case 'q':
        g[0]=0x08; g[1]=0x14; g[2]=0x14;
        g[3]=0x18; g[4]=0x7C;
        break;

      case 'r':
        g[0]=0x7C; g[1]=0x08; g[2]=0x04;
        g[3]=0x04; g[4]=0x08;
        break;

      case 's':
        g[0]=0x48; g[1]=0x54; g[2]=0x54;
        g[3]=0x54; g[4]=0x20;
        break;

      case 't':
        g[0]=0x04; g[1]=0x3F; g[2]=0x44;
        g[3]=0x40; g[4]=0x20;
        break;

      case 'u':
        g[0]=0x3C; g[1]=0x40; g[2]=0x40;
        g[3]=0x20; g[4]=0x7C;
        break;

      case 'v':
        g[0]=0x1C; g[1]=0x20; g[2]=0x40;
        g[3]=0x20; g[4]=0x1C;
        break;

      case 'w':
        g[0]=0x3C; g[1]=0x40; g[2]=0x30;
        g[3]=0x40; g[4]=0x3C;
        break;

      case 'x':
        g[0]=0x44; g[1]=0x28; g[2]=0x10;
        g[3]=0x28; g[4]=0x44;
        break;

      case 'y':
        g[0]=0x0C; g[1]=0x50; g[2]=0x50;
        g[3]=0x50; g[4]=0x3C;
        break;

      case 'z':
        g[0]=0x44; g[1]=0x64; g[2]=0x54;
        g[3]=0x4C; g[4]=0x44;
        break;


      // ------------------------------------------------
      // Numbers
      // ------------------------------------------------

      case '0':
        g[0]=0x3E; g[1]=0x45; g[2]=0x49;
        g[3]=0x51; g[4]=0x3E;
        break;

      case '1':
        g[0]=0x00; g[1]=0x21; g[2]=0x7F;
        g[3]=0x01; g[4]=0x00;
        break;

      case '2':
        g[0]=0x23; g[1]=0x45; g[2]=0x49;
        g[3]=0x51; g[4]=0x21;
        break;

      case '3':
        g[0]=0x22; g[1]=0x41; g[2]=0x49;
        g[3]=0x49; g[4]=0x36;
        break;

      case '4':
        g[0]=0x0C; g[1]=0x14; g[2]=0x24;
        g[3]=0x7F; g[4]=0x04;
        break;

      case '5':
        g[0]=0x72; g[1]=0x51; g[2]=0x51;
        g[3]=0x51; g[4]=0x4E;
        break;

      case '6':
        g[0]=0x3E; g[1]=0x49; g[2]=0x49;
        g[3]=0x49; g[4]=0x06;
        break;

      case '7':
        g[0]=0x40; g[1]=0x47; g[2]=0x48;
        g[3]=0x50; g[4]=0x60;
        break;

      case '8':
        g[0]=0x36; g[1]=0x49; g[2]=0x49;
        g[3]=0x49; g[4]=0x36;
        break;

      case '9':
        g[0]=0x30; g[1]=0x49; g[2]=0x49;
        g[3]=0x49; g[4]=0x3E;
        break;


      // ------------------------------------------------
      // Symbols
      // ------------------------------------------------

      case ':':
        g[0]=0x00; g[1]=0x36; g[2]=0x36;
        g[3]=0x00; g[4]=0x00;
        break;

      case '.':
        g[0]=0x00; g[1]=0x40; g[2]=0x60;
        g[3]=0x00; g[4]=0x00;
        break;

      case '-':
        g[0]=0x08; g[1]=0x08; g[2]=0x08;
        g[3]=0x08; g[4]=0x08;
        break;

      case '_':
        g[0]=0x40; g[1]=0x40; g[2]=0x40;
        g[3]=0x40; g[4]=0x40;
        break;

      case '/':
        g[0]=0x01; g[1]=0x06; g[2]=0x08;
        g[3]=0x30; g[4]=0x40;
        break;

      case '%':
        g[0]=0x63; g[1]=0x13; g[2]=0x08;
        g[3]=0x64; g[4]=0x63;
        break;

      case '=':
        g[0]=0x14; g[1]=0x14; g[2]=0x14;
        g[3]=0x14; g[4]=0x14;
        break;

      case '+':
        g[0]=0x08; g[1]=0x08; g[2]=0x3E;
        g[3]=0x08; g[4]=0x08;
        break;

      case '(':
        g[0]=0x00; g[1]=0x1C; g[2]=0x22;
        g[3]=0x41; g[4]=0x00;
        break;

      case ')':
        g[0]=0x00; g[1]=0x41; g[2]=0x22;
        g[3]=0x1C; g[4]=0x00;
        break;

      default:
        break;
    }
  }


  // ==================================================
  // Numeric glyph detection
  // ==================================================

  bool isNumericGlyph(char c) {

    return (
      c >= '0' && c <= '9'
    );
  }


  // ==================================================
  // Pixel
  // ==================================================

  void pixel(
    int x,
    int y,
    uint16_t color
  ) {

    if (
      x < 0 ||
      x >= _width ||
      y < 0 ||
      y >= _height
    ) {
      return;
    }

    _buffer[
      y * _width + x
    ] = color;
  }


  // ==================================================
  // Character
  // ==================================================

  void drawChar(
    int x,
    int y,
    char c,
    uint16_t color
  ) {

    uint8_t g[5];

    glyph(c, g);

    bool flipVertical =
      isNumericGlyph(c);


    for (int col = 0; col < 5; col++) {

      for (int row = 0; row < 7; row++) {

        int sourceRow = row;

        // Numbers and numeric symbols were encoded
        // vertically mirrored. Flip only those glyphs.
        if (flipVertical) {
          sourceRow = 6 - row;
        }

        if (g[col] & (1 << sourceRow)) {

          for (int dx = 0; dx < SCALE; dx++) {

            for (int dy = 0; dy < SCALE; dy++) {

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


  // ==================================================
  // String with selectable colour
  // ==================================================

  void drawString(
    int x,
    int y,
    const String& text,
    uint16_t color
  ) {

    int cursor = x;

    for (size_t i = 0; i < text.length(); i++) {

      drawChar(
        cursor,
        y,
        text[i],
        color
      );

      cursor += CHAR_W * SCALE;
    }
  }


  // ==================================================
  // Header
  // ==================================================

  void drawHeader() {

    // Black header background

    for (int y = 0; y < HEADER_H; y++) {

      for (int x = 0; x < _width; x++) {

        _buffer[
          y * _width + x
        ] = _background;

      }
    }


    // Dark green title

    drawString(
      16,
      10,
      "SAUNA DEBUG",
      _headerTextColor
    );
  }


  // ==================================================
  // Uptime clock
  // ==================================================

  void drawClock() {

    if (!_buffer || !_lcd)
      return;


    unsigned long totalSeconds =
      millis() / 1000;


    unsigned long hours =
      totalSeconds / 3600;


    unsigned long minutes =
      (totalSeconds % 3600) / 60;


    unsigned long seconds =
      totalSeconds % 60;


    char timeText[16];


    snprintf(
      timeText,
      sizeof(timeText),
      "%02lu:%02lu:%02lu",
      hours,
      minutes,
      seconds
    );


    // Clock area

    const int clockWidth = 100;


    for (
      int y = 4;
      y < HEADER_H - 4;
      y++
    ) {

      for (
        int x = _width - clockWidth;
        x < _width - 4;
        x++
      ) {

        if (x >= 0) {

          _buffer[
            y * _width + x
          ] = _background;

        }
      }
    }


    // Dark green clock

    drawString(
      _width - clockWidth,
      10,
      String(timeText),
      _headerTextColor
    );


    // Update header

    _lcd->drawBitmap(
      0,
      0,
      _width,
      HEADER_H,
      (const uint8_t*)_buffer
    );
  }


  // ==================================================
  // WiFi status
  // ==================================================

  void drawWiFiStatus() {

    const int y = WIFI_STATUS_Y;

    const int height =
      (y + LINE_HEIGHT <= _height) ?
        LINE_HEIGHT : _height - y;

    if (height <= 0)
      return;


    for (int row = y; row < y + height; row++) {

      for (int x = 0; x < _width; x++) {

        _buffer[
          row * _width + x
        ] = _background;
      }
    }


    drawString(
      16,
      y,
      _wifiStatus,
      _foreground
    );
  }


  // ==================================================
  // Full redraw
  // ==================================================

  void redraw() {

    if (!_buffer || !_lcd)
      return;


    // Black background

    for (
      int i = 0;
      i < _width * _height;
      i++
    ) {

      _buffer[i] =
        _background;
    }


    // Header

    drawHeader();


    // Debug lines

    for (
      int i = 0;
      i < _lineCount;
      i++
    ) {

      drawString(
        16,
        HEADER_H + 8 +
        i * LINE_HEIGHT,
        _lines[i],
        _foreground
      );
    }


    // Fixed WiFi status area

    drawWiFiStatus();


    // Send framebuffer

    _lcd->drawBitmap(
      0,
      0,
      _width,
      _height,
      (const uint8_t*)_buffer
    );


    // Clock

    drawClock();
  }


public:


  // ==================================================
  // Begin
  // ==================================================

  bool begin(LCD* lcd) {

    _lcd = lcd;

    if (!_lcd)
      return false;


    _width =
      _lcd->getFrameWidth();


    _height =
      _lcd->getFrameHeight();


    // Allocate framebuffer in PSRAM

    _buffer =
      (uint16_t*)heap_caps_malloc(
        _width *
        _height *
        sizeof(uint16_t),
        MALLOC_CAP_SPIRAM
      );


    if (!_buffer)
      return false;


    clear();

    return true;
  }


  // ==================================================
  // Clear
  // ==================================================

  void clear() {

    _lineCount = 0;
    _wifiStatus = "";

    if (_lcd)
      redraw();
  }


  // ==================================================
  // Print line
  // ==================================================

  void println(
    const String& text
  ) {

    Serial.println(text);


    // Scroll when full

    if (_lineCount >= MAX_LINES) {

      for (
        int i = 1;
        i < MAX_LINES;
        i++
      ) {

        _lines[i - 1] =
          _lines[i];
      }

      _lineCount =
        MAX_LINES - 1;
    }


    _lines[_lineCount++] =
      text;


    redraw();
  }


  // ==================================================
  // Print without newline
  // ==================================================

  void print(
    const String& text
  ) {

    Serial.print(text);


    if (_lineCount == 0) {
      _lines[_lineCount++] = "";
    }


    _lines[_lineCount - 1] +=
      text;


    redraw();
  }


  // ==================================================
  // Status
  // ==================================================

  void status(
    const String& name,
    bool ok
  ) {

    String line = name;


    while (line.length() < 22) {
      line += " ";
    }


    line +=
      ok ? "OK" : "WAITING";


    println(line);
  }


  // ==================================================
  // Value
  // ==================================================

  void value(
    const String& name,
    float value,
    const String& unit
  ) {

    String line =
      name +
      ": " +
      String(value, 1) +
      " " +
      unit;


    println(line);
  }


  // ==================================================
  // Error
  // ==================================================

  void error(
    const String& name,
    const String& message
  ) {

    println(
      "ERROR " +
      name +
      ": " +
      message
    );
  }


  // ==================================================
  // Fixed WiFi status
  // ==================================================

  void wifiStatus(
    const String& text
  ) {

    _wifiStatus = text;

    if (!_buffer || !_lcd)
      return;


    const int y = WIFI_STATUS_Y;

    const int height =
      (y + LINE_HEIGHT <= _height) ?
        LINE_HEIGHT : _height - y;


    drawWiFiStatus();

    _lcd->drawBitmap(
      0,
      y,
      _width,
      height,
      (const uint8_t*)(
        _buffer + y * _width
      )
    );
  }


  // ==================================================
  // Update uptime
  // ==================================================

  void update() {

    drawClock();
  }


  // ==================================================
  // Change colours
  // ==================================================

  void setColors(
    uint16_t background,
    uint16_t foreground,
    uint16_t headerText
  ) {

    _background = background;
    _foreground = foreground;
    _headerTextColor = headerText;

    redraw();
  }
};
