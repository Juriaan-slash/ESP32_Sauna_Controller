#include "SysteemStatus.h"

#include <WiFi.h>
#include <time.h>

#include "DebugLog.h"
#include "wifi_secrets.h"


// ==================================================
// Constants
// ==================================================

constexpr char TIMEZONE[] =
    "CET-1CEST,M3.5.0,M10.5.0/3";

constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS =
    10000;

constexpr unsigned long WIFI_RECONNECT_INTERVAL_MS =
    10000;



// ==================================================
// Static data
// ==================================================

SysteemStatus::Data
SysteemStatus::_data;

PCF85063
SysteemStatus::_rtc;

bool
SysteemStatus::_ntpStarted = false;

bool
SysteemStatus::_rtcSyncedFromNtp = false;

unsigned long
SysteemStatus::_lastWiFiReconnectAttempt = 0;


// ==================================================
// Begin
// ==================================================

void SysteemStatus::setInitTime(
    unsigned long milliseconds
)
{
    _data.initTimeMs = milliseconds;
}

bool SysteemStatus::begin()
{
    _data.boardOk = false;
    _data.displayOk = false;
    _data.touchOk = false;

    _data.initTimeMs = 0;
    _data.runtimeMs = 0;

    _data.rtcValid = false;

    _data.wifiConnected = false;
    _data.wifiIp = "";
    _data.wifiRssi = 0;
    _data.wifiReconnectCount = 0;

    _data.ds2484Present = false;
    _data.kTypeValid = false;

    _data.psram =
        ESP.getPsramSize() > 0;

    _data.freeHeap =
        ESP.getFreeHeap();

    _data.cpuMHz =
        ESP.getCpuFreqMHz();

    _data.flashSizeMb =
        ESP.getFlashChipSize() /
        (1024UL * 1024UL);

    return true;
}


// ==================================================
// Update
// ==================================================

void SysteemStatus::update()
{
    _data.runtimeMs =
        millis();

    _data.freeHeap =
        ESP.getFreeHeap();

    _data.cpuMHz =
        ESP.getCpuFreqMHz();

    _data.psram =
        ESP.getPsramSize() > 0;

    updateWiFi();
}


// ==================================================
// Get
// ==================================================

const SysteemStatus::Data&
SysteemStatus::get()
{
    return _data;
}


// ==================================================
// RTC
// ==================================================

bool SysteemStatus::initializeRtc()
{
    DebugLog::println(
        "Initializing PCF85063 RTC..."
    );


    if (!_rtc.begin()) {

        DebugLog::println(
            "RTC: PCF85063 not found"
        );

        _data.rtcValid = false;

        return false;
    }


    DebugLog::println(
        "RTC found"
    );


    if (readRtc()) {

        DebugLog::printf(
            "RTC time: %04d-%02d-%02d %02d:%02d:%02d\n",
            _data.rtcDateTime.year,
            _data.rtcDateTime.month,
            _data.rtcDateTime.day,
            _data.rtcDateTime.hour,
            _data.rtcDateTime.minute,
            _data.rtcDateTime.second
        );

        return true;
    }


    DebugLog::println(
        "RTC found, but time is invalid"
    );

    return false;
}


void SysteemStatus::updateRtc()
{
    readRtc();


    // ------------------------------------------------
    // NTP → RTC
    // ------------------------------------------------

    if (
        _data.wifiConnected
    ) {

        startNtp();


        if (
            !_data.rtcValid &&
            !_rtcSyncedFromNtp
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
}


bool SysteemStatus::readRtc()
{
    if (
        !_rtc.read(
            _data.rtcDateTime
        )
    ) {

        _data.rtcValid = false;

        return false;
    }


    _data.rtcValid = true;

    return true;
}


// ==================================================
// NTP
// ==================================================

void SysteemStatus::startNtp()
{
    if (_ntpStarted) {
        return;
    }


    configTzTime(
        TIMEZONE,
        "pool.ntp.org",
        "time.nist.gov"
    );


    _ntpStarted = true;


    DebugLog::println(
        "NTP started"
    );
}


bool SysteemStatus::syncRtcFromNtp()
{
    if (!_ntpStarted) {
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
        !_rtc.write(
            newTime
        )
    ) {

        DebugLog::println(
            "RTC write failed"
        );

        return false;
    }


    // Read newly written time back.
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


    _rtcSyncedFromNtp =
        true;


    return true;
}


// ==================================================
// WiFi initialization
// ==================================================

void SysteemStatus::initializeWiFi()
{
    WiFi.mode(
        WIFI_STA
    );


    startWiFiConnection(
        "WiFi connecting..."
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
    }
}


// ==================================================
// WiFi update
// ==================================================

void SysteemStatus::updateWiFi()
{
    const bool connected =
        WiFi.status() ==
        WL_CONNECTED;


    // ------------------------------------------------
    // Newly connected
    // ------------------------------------------------

    if (
        connected &&
        !_data.wifiConnected
    ) {

        _data.wifiConnected =
            true;

        _data.wifiIp =
            WiFi.localIP().toString();

        _data.wifiRssi =
            WiFi.RSSI();

        showWiFiConnection();

        return;
    }


    // ------------------------------------------------
    // Disconnected
    // ------------------------------------------------

    if (
        !connected &&
        _data.wifiConnected
    ) {

        DebugLog::println(
            "WiFi disconnected"
        );

        _data.wifiConnected =
            false;

        _data.wifiIp = "";

        _data.wifiRssi = 0;
    }


    // ------------------------------------------------
    // Reconnect
    // ------------------------------------------------

    if (
        !connected &&
        millis() -
            _lastWiFiReconnectAttempt >=
            WIFI_RECONNECT_INTERVAL_MS
    ) {

        ++_data.wifiReconnectCount;


        startWiFiConnection(
            "WiFi reconnecting..."
        );
    }


    // ------------------------------------------------
    // Normal connected status
    // ------------------------------------------------

    if (connected) {

        _data.wifiConnected =
            true;

        _data.wifiIp =
            WiFi.localIP().toString();

        _data.wifiRssi =
            WiFi.RSSI();
    }
}


// ==================================================
// WiFi helpers
// ==================================================

void SysteemStatus::printWiFiStatus(
    bool ok
)
{
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


void SysteemStatus::startWiFiConnection(
    const char* message
)
{
    DebugLog::println(
        message
    );


    printWiFiStatus(
        false
    );


    WiFi.begin(
        WIFI_SSID,
        WIFI_PASSWORD
    );


    _lastWiFiReconnectAttempt =
        millis();
}


void SysteemStatus::showWiFiConnection()
{
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
}