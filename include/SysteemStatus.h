#pragma once

#include <Arduino.h>
#include "PCF85063.h"


class SysteemStatus {

public:

    enum class ValueStatus
    {
        INVALID,
        VALID,
        ERROR,
        WARNING,
        PANIC
    };

    struct Measurement
    {
        float value;
        ValueStatus status;
    };

    struct Data {

        // System
        bool boardOk = false;
        bool displayOk = false;
        bool touchOk = false;

        unsigned long initTimeMs = 0;
        unsigned long runtimeMs = 0;


        // RTC
        bool rtcValid = false;
        PCF85063::DateTime rtcDateTime;


        // WiFi
        bool wifiConnected = false;
        String wifiIp;
        int wifiRssi = 0;
        ValueStatus wifiRssiStatus = ValueStatus::INVALID;
        unsigned long wifiReconnectCount = 0;


        // Sensors
        bool ds2484Present = false;
        bool kTypeValid = false;

        Measurement temperature{
            0.0f,
            ValueStatus::INVALID
        };

        Measurement humidity{
            0.0f,
            ValueStatus::INVALID
        };

        Measurement pressure{
            0.0f,
        ValueStatus::INVALID
        };


        // Hardware
        bool psram = false;
        uint32_t freeHeap = 0;
        uint32_t minFreeHeap = 0;
        uint32_t cpuMHz = 0;
        uint32_t flashSizeMb = 0;
    };

    static void setInitTime(unsigned long milliseconds);

    static bool begin();

    static void update();

    static const Data& get();

    static ValueStatus evaluateWifiRssi(int rssi);

    // RTC
    static bool initializeRtc();

    static void updateRtc();


    // WiFi
    static void initializeWiFi();

    static void updateWiFi();


private:

    static Data _data;

    static PCF85063 _rtc;

    static bool _ntpStarted;

    static bool _rtcSyncedFromNtp;

    static unsigned long _lastWiFiReconnectAttempt;


    // RTC
    static bool readRtc();

    static void startNtp();

    static bool syncRtcFromNtp();


    // WiFi
    static void printWiFiStatus(bool ok);

    static void startWiFiConnection(
        const char* message
    );

    static void showWiFiConnection();

    static void connectWiFiAtStartup();
};
