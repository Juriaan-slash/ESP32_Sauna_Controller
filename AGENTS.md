# ESP32 Sauna Controller

## Project configuration

- **Hardware:** Waveshare ESP32-S3-Touch-LCD-4.3B.
- **MCU:** ESP32-S3 with 16 MB flash and PSRAM.
- **Build system:** PlatformIO using the pioarduino Espressif32 `55.03.311` platform.
- **Framework:** Arduino.
- **Board definition:** `boards/waveshare_esp32_s3_touch_lcd_4_3_b.json`.
- **Language standard:** GNU C++17.

## Dependencies

- `ESP32_Display_Panel` 1.0.4
- `ESP32_IO_Expander` 1.1.1
- `esp-lib-utils` 0.3.0

## Source layout

- `src/main.cpp` is the current application entry point.
- `src/SaunaDebugDisplay.h` contains the current display implementation.

## Development rules

- Preserve working hardware functionality.
- Do not change `platformio.ini` or the custom board definition unless explicitly requested.
- Always build the project after code changes.
- Do not commit or push Git changes unless explicitly requested.
- Prefer small, incremental changes.
- Do not modify unrelated files.
- Before changing hardware-specific code, explain the intended change.
- Keep the project buildable after every change.
