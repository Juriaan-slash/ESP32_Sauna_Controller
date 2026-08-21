#include "Web.h"

#include <Arduino.h>
#include <WebServer.h>

#include "SysteemStatus.h"


namespace
{

WebServer server(80);


// ==================================================
// Root
// ==================================================

void handleRoot()
{
    server.send(
        200,
        "text/html",
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' "
        "content='width=device-width,initial-scale=1'>"
        "<title>Sauna Controller</title>"
        "</head>"
        "<body>"
        "<h1>Hello World</h1>"
        "<p><a href='/SaunaDebug'>Sauna Debug</a></p>"
        "</body>"
        "</html>"
    );
}


// ==================================================
// Sauna Debug
// ==================================================

void handleSaunaDebug()
{
    const auto& status =
        SysteemStatus::get();


    String html;

    html.reserve(12000);


    // ==================================================
    // HTML header
    // ==================================================

    html +=
        "<!DOCTYPE html>"
        "<html>"
        "<head>"
        "<meta charset='UTF-8'>"
        "<meta name='viewport' "
        "content='width=device-width,initial-scale=1'>"
        "<meta http-equiv='refresh' content='12'>"
        "<title>Sauna Debug</title>";


    // ==================================================
    // CSS
    // ==================================================

    html +=

        "<style>"

        "body{"
        "font-family:Arial,sans-serif;"
        "background:#111;"
        "color:#eee;"
        "margin:20px;"
        "}"

        "h1{"
        "margin-bottom:20px;"
        "}"

        ".grid{"
        "display:grid;"
        "grid-template-columns:1fr 1fr;"
        "gap:12px;"
        "max-width:1100px;"
        "}"

        ".card{"
        "background:#222;"
        "border:1px solid #444;"
        "border-radius:8px;"
        "padding:18px;"
        "}"

        ".card h2{"
        "font-size:18px;"
        "margin:0 0 12px 0;"
        "}"

        ".row{"
        "display:grid;"
        "grid-template-columns:1fr 1fr;"
        "align-items:center;"
        "padding:6px 0;"
        "border-bottom:1px solid #333;"
        "}"

        ".row:last-child{"
        "border-bottom:none;"
        "}"

        ".label{"
        "color:#aaa;"
        "font-size:16px;"
        "}"

        ".value{"
        "font-family:Arial,sans-serif;"
        "font-size:16px;"
        "font-weight:500;"
        "text-align:right;"
        "}"

        "a{"
        "color:#8ab4f8;"
        "}"

        "@media(max-width:700px){"

        "body{"
        "margin:12px;"
        "}"

        ".grid{"
        "grid-template-columns:1fr;"
        "gap:10px;"
        "}"

        ".card{"
        "padding:16px;"
        "}"

        ".card h2{"
        "font-size:18px;"
        "}"

        ".label{"
        "font-size:16px;"
        "}"

        ".value{"
        "font-size:16px;"
        "}"

        "}"

        "</style>"
        "</head>"
        "<body>";


    // ==================================================
    // Page title
    // ==================================================

    html +=
        "<h1>Sauna Debug</h1>"

        "<div class='grid'>";


    // ==================================================
    // System
    // ==================================================

    html +=
        "<div class='card'>"

        "<h2>System</h2>"

        "<div class='row'>"
        "<span class='label'>Board</span>"
        "<span class='value'>OK</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Display</span>"
        "<span class='value'>OK</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Touch</span>"
        "<span class='value'>OK</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Init time</span>"
        "<span class='value'>" +
        String(status.initTimeMs) +
        " ms</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Runtime</span>"
        "<span class='value'>" +
        String(status.runtimeMs / 1000UL) +
        " s</span>"
        "</div>"

        "</div>";


    // ==================================================
    // Connection
    // ==================================================

    html +=
        "<div class='card'>"

        "<h2>Connection</h2>"

        "<div class='row'>"
        "<span class='label'>WiFi</span>"
        "<span class='value'>" +
        String(
            status.wifiConnected
                ? "OK"
                : "WAITING"
        ) +
        "</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>SSID</span>"
        "<span class='value'>Inforatio</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>IP</span>"
        "<span class='value'>" +
        status.wifiIp +
        "</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>RSSI</span>"
        "<span class='value'>" +
        String(status.wifiRssi) +
        " dBm</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Reconnects</span>"
        "<span class='value'>" +
        String(status.wifiReconnectCount) +
        "</span>"
        "</div>"

        "</div>";


    // ==================================================
    // Sensors
    // ==================================================

    html +=
        "<div class='card'>"

        "<h2>Sensors</h2>"

        "<div class='row'>"
        "<span class='label'>DS2484</span>"
        "<span class='value'>" +
        String(
            status.ds2484Present
                ? "OK"
                : "WAITING"
        ) +
        "</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>K-Type</span>"
        "<span class='value'>" +
        String(
            status.kTypeValid
                ? "OK"
                : "WAITING"
        ) +
        "</span>"
        "</div>"

        "</div>";


    // ==================================================
    // Test values
    // ==================================================

    html +=
        "<div class='card'>"

        "<h2>Test Values</h2>"

        "<div class='row'>"
        "<span class='label'>Temperature</span>"
        "<span class='value'>23.4 C</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Humidity</span>"
        "<span class='value'>48.0 %</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Pressure</span>"
        "<span class='value'>1012.0 hPa</span>"
        "</div>"

        "</div>";


    // ==================================================
    // System log
    // ==================================================

    html +=
        "<div class='card'>"

        "<h2>System Log</h2>"

        "<div class='row'>"
        "<span class='label'>Board</span>"
        "<span class='value'>OK</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Display</span>"
        "<span class='value'>OK</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>WiFi reconnects</span>"
        "<span class='value'>" +
        String(status.wifiReconnectCount) +
        "</span>"
        "</div>"

        "</div>";


    // ==================================================
    // Hardware
    // ==================================================

    html +=
        "<div class='card'>"

        "<h2>Hardware</h2>"

        "<div class='row'>"
        "<span class='label'>PSRAM</span>"
        "<span class='value'>" +
        String(
            status.psram
                ? "OK"
                : "NO"
        ) +
        "</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Free heap</span>"
        "<span class='value'>" +
        String(status.freeHeap) +
        " B</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>Flash</span>"
        "<span class='value'>" +
        String(status.flashSizeMb) +
        " MB</span>"
        "</div>"

        "<div class='row'>"
        "<span class='label'>CPU</span>"
        "<span class='value'>" +
        String(status.cpuMHz) +
        " MHz</span>"
        "</div>"

        "</div>";


    // ==================================================
    // RTC
    // ==================================================

    html +=
        "<div class='card'>"

        "<h2>RTC</h2>";


    if (status.rtcValid)
    {
        char buffer[32];

        snprintf(
            buffer,
            sizeof(buffer),
            "%04d-%02d-%02d %02d:%02d:%02d",
            status.rtcDateTime.year,
            status.rtcDateTime.month,
            status.rtcDateTime.day,
            status.rtcDateTime.hour,
            status.rtcDateTime.minute,
            status.rtcDateTime.second
        );


        html +=

            "<div class='row'>"
            "<span class='label'>Status</span>"
            "<span class='value'>OK</span>"
            "</div>"

            "<div class='row'>"
            "<span class='label'>Date / time</span>"
            "<span class='value'>" +
            String(buffer) +
            "</span>"
            "</div>";
    }
    else
    {
        html +=

            "<div class='row'>"
            "<span class='label'>Status</span>"
            "<span class='value'>INVALID</span>"
            "</div>";
    }


    html +=
        "</div>";


    // ==================================================
    // Footer
    // ==================================================

    html +=
        "</div>"

        "<p>"
        "<a href='/'>Home</a>"
        "</p>"

        "</body>"
        "</html>";


    // ==================================================
    // Send response
    // ==================================================

    server.send(
        200,
        "text/html",
        html
    );
}

} // namespace


// ==================================================
// Web
// ==================================================

void Web::begin()
{
    server.on(
        "/",
        handleRoot
    );


    server.on(
        "/SaunaDebug",
        handleSaunaDebug
    );


    server.begin();


    Serial.println(
        "Web server started"
    );
}


void Web::update()
{
    server.handleClient();
}