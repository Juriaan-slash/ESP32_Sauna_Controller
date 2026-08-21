#pragma once

#include <Arduino.h>
#include <esp_display_panel.hpp>


class LCDDisplay {

public:

    static bool begin(
        esp_panel::drivers::LCD* lcd
    );

    static void update();

    static void refresh();
};