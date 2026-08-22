#include "LCDDisplay.h"

#include <WiFi.h>

#include "SaunaDebugDisplay.h"
#include "SysteemStatus.h"
#include "wifi_secrets.h"


namespace
{

SaunaDebugDisplay debugDisplay;


// ==================================================
// Formatting
// ==================================================

String formatRuntime(
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


String formatRtcDateTime(
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
// System
// ==================================================

void updateSystem()
{
    const auto& status =
        SysteemStatus::get();

    debugDisplay.setSystem(
        "OK",
        "OK",
        "OK",
        String(status.initTimeMs) + " ms",
        formatRuntime(status.runtimeMs)
    );
}


// ==================================================
// Connection
// ==================================================

void updateConnection()
{
    const auto& status =
        SysteemStatus::get();

    debugDisplay.setConnection(
    status.wifiConnected
        ? "OK"
        : "WAITING",

    status.wifiConnected
        ? SysteemStatus::ValueStatus::VALID
        : SysteemStatus::ValueStatus::INVALID,

    WIFI_SSID,

    status.wifiConnected
        ? status.wifiIp
        : "---",

    status.wifiConnected
        ? String(status.wifiRssi) + " dBm"
        : "---",

    status.wifiRssiStatus,

    status.rtcValid
        ? "OK"
        : "INVALID"
);
}


// ==================================================
// Sensors
// ==================================================

void updateSensors()
{
    const auto& status =
        SysteemStatus::get();

    debugDisplay.setSensors(
        status.ds2484Present
            ? "OK"
            : "WAITING",

        status.kTypeValid
            ? "OK"
            : "WAITING",

        "--",
        "--"
    );
}


// ==================================================
// Test values
// ==================================================

void updateTestValues()
{
    const auto& status =
        SysteemStatus::get();

    // -------------------------------------------------
    // Temperature
    // -------------------------------------------------

    const String temperatureText =
        status.temperature.status ==
            SysteemStatus::ValueStatus::INVALID
            ? "---"
            : String(status.temperature.value, 1) + " C";


    // -------------------------------------------------
    // Humidity
    // -------------------------------------------------

    const String humidityText =
        status.humidity.status ==
            SysteemStatus::ValueStatus::INVALID
            ? "---"
            : String(status.humidity.value, 1) + " %";


    // -------------------------------------------------
    // Pressure
    // -------------------------------------------------

    const String pressureText =
        status.pressure.status ==
            SysteemStatus::ValueStatus::INVALID
            ? "---"
            : String(status.pressure.value, 1) + " hPa";


    // -------------------------------------------------
    // Update display
    // -------------------------------------------------

    debugDisplay.setTestValues(
        temperatureText,
        status.temperature.status,

        humidityText,
        status.humidity.status,

        pressureText,
        status.pressure.status
    );
}


// ==================================================
// System log
// ==================================================

void updateSystemLog()
{
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
}


// ==================================================
// Hardware
// ==================================================

void updateHardware()
{
    const auto& status =
        SysteemStatus::get();

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
        ) + " MHz",

        String(
            status.minFreeHeap
        ) + " B"
    );
}


// ==================================================
// RTC
// ==================================================

void updateRtc()
{
    const auto& status =
        SysteemStatus::get();

    if (status.rtcValid)
    {
        debugDisplay.setDateTime(
            formatRtcDateTime(
                status.rtcDateTime
            ),
            "OK"
        );
    }
    else
    {
        debugDisplay.setDateTime(
            "RTC INVALID",
            "INVALID"
        );
    }
}

} // namespace


// ==================================================
// LCDDisplay
// ==================================================

bool LCDDisplay::begin(
    esp_panel::drivers::LCD* lcd
)
{
    if (!lcd)
    {
        return false;
    }

    if (!debugDisplay.begin(lcd))
    {
        return false;
    }

    return true;
}


void LCDDisplay::update()
{
    updateSystem();
    updateConnection();
    updateSensors();
    updateTestValues();
    updateSystemLog();
    updateHardware();
    updateRtc();
}


void LCDDisplay::refresh()
{
    debugDisplay.refresh();
}
