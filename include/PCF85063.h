#pragma once

#include <Arduino.h>
#include "driver/i2c.h"


class PCF85063 {

public:

  static constexpr uint8_t ADDRESS = 0x51;
  static constexpr i2c_port_t I2C_PORT = I2C_NUM_0;

  struct DateTime {
    int year = 0;
    int month = 0;
    int day = 0;
    int weekday = 0;   // 0 = Sunday
    int hour = 0;
    int minute = 0;
    int second = 0;
  };


  bool begin() {

    // ESP32_Display_Panel already owns I2C_NUM_0.
    // Do NOT initialize or install the I2C driver here.

    return probe();
  }


  bool read(DateTime& dt) {

    uint8_t data[7] = {0};

    if (!readRegisters(
          0x04,
          data,
          sizeof(data)
        )) {
      return false;
    }


    const uint8_t seconds = data[0];

    // Oscillator Stop flag.
    // If set, the RTC time is not trustworthy.
    if (seconds & 0x80) {
      return false;
    }


    dt.second =
      bcdToDec(
        seconds & 0x7F
      );

    dt.minute =
      bcdToDec(
        data[1] & 0x7F
      );

    dt.hour =
      bcdToDec(
        data[2] & 0x3F
      );

    dt.day =
      bcdToDec(
        data[3] & 0x3F
      );

    dt.weekday =
      data[4] & 0x07;

    dt.month =
      bcdToDec(
        data[5] & 0x1F
      );

    dt.year =
      2000 +
      bcdToDec(
        data[6]
      );


    return
      dt.year >= 2020 &&
      dt.year <= 2099 &&
      dt.month >= 1 &&
      dt.month <= 12 &&
      dt.day >= 1 &&
      dt.day <= 31 &&
      dt.hour <= 23 &&
      dt.minute <= 59 &&
      dt.second <= 59;
  }


  bool write(
    const DateTime& dt
  ) {

    if (
      dt.year < 2000 ||
      dt.year > 2099 ||
      dt.month < 1 ||
      dt.month > 12 ||
      dt.day < 1 ||
      dt.day > 31 ||
      dt.hour < 0 ||
      dt.hour > 23 ||
      dt.minute < 0 ||
      dt.minute > 59 ||
      dt.second < 0 ||
      dt.second > 59
    ) {
      return false;
    }


    uint8_t data[7];

    // IMPORTANT:
    // Seconds bit 7 must be 0 to clear the OS flag.
    data[0] =
      decToBcd(
        static_cast<uint8_t>(dt.second)
      ) & 0x7F;

    data[1] =
      decToBcd(
        static_cast<uint8_t>(dt.minute)
      );

    data[2] =
      decToBcd(
        static_cast<uint8_t>(dt.hour)
      );

    data[3] =
      decToBcd(
        static_cast<uint8_t>(dt.day)
      );

    data[4] =
      static_cast<uint8_t>(
        dt.weekday & 0x07
      );

    data[5] =
      decToBcd(
        static_cast<uint8_t>(dt.month)
      );

    data[6] =
      decToBcd(
        static_cast<uint8_t>(
          dt.year - 2000
        )
      );


    return writeRegisters(
      0x04,
      data,
      sizeof(data)
    );
  }


private:

  static uint8_t bcdToDec(
    uint8_t value
  ) {

    return static_cast<uint8_t>(
      ((value >> 4) * 10) +
      (value & 0x0F)
    );
  }


  static uint8_t decToBcd(
    uint8_t value
  ) {

    return static_cast<uint8_t>(
      ((value / 10) << 4) |
      (value % 10)
    );
  }


  bool probe() {

    i2c_cmd_handle_t cmd =
      i2c_cmd_link_create();

    if (!cmd) {
      return false;
    }


    esp_err_t result =
      i2c_master_start(cmd);

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_write_byte(
        cmd,
        (ADDRESS << 1) |
          I2C_MASTER_WRITE,
        true
      );

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_stop(cmd);

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_cmd_begin(
        I2C_PORT,
        cmd,
        pdMS_TO_TICKS(100)
      );


    i2c_cmd_link_delete(cmd);

    return result == ESP_OK;
  }


  bool readRegisters(
    uint8_t startRegister,
    uint8_t* buffer,
    size_t length
  ) {

    if (
      buffer == nullptr ||
      length == 0
    ) {
      return false;
    }


    i2c_cmd_handle_t cmd =
      i2c_cmd_link_create();

    if (!cmd) {
      return false;
    }


    esp_err_t result;


    result =
      i2c_master_start(cmd);

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_write_byte(
        cmd,
        (ADDRESS << 1) |
          I2C_MASTER_WRITE,
        true
      );

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_write_byte(
        cmd,
        startRegister,
        true
      );

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_start(cmd);

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_write_byte(
        cmd,
        (ADDRESS << 1) |
          I2C_MASTER_READ,
        true
      );

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    for (
      size_t i = 0;
      i < length - 1;
      ++i
    ) {

      result =
        i2c_master_read_byte(
          cmd,
          &buffer[i],
          I2C_MASTER_ACK
        );

      if (result != ESP_OK) {
        i2c_cmd_link_delete(cmd);
        return false;
      }
    }


    result =
      i2c_master_read_byte(
        cmd,
        &buffer[length - 1],
        I2C_MASTER_NACK
      );

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_stop(cmd);

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_cmd_begin(
        I2C_PORT,
        cmd,
        pdMS_TO_TICKS(100)
      );


    i2c_cmd_link_delete(cmd);

    return result == ESP_OK;
  }


  bool writeRegisters(
    uint8_t startRegister,
    const uint8_t* data,
    size_t length
  ) {

    if (
      data == nullptr ||
      length == 0
    ) {
      return false;
    }


    i2c_cmd_handle_t cmd =
      i2c_cmd_link_create();

    if (!cmd) {
      return false;
    }


    esp_err_t result;


    result =
      i2c_master_start(cmd);

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_write_byte(
        cmd,
        (ADDRESS << 1) |
          I2C_MASTER_WRITE,
        true
      );

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_write_byte(
        cmd,
        startRegister,
        true
      );

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_write(
        cmd,
        const_cast<uint8_t*>(data),
        length,
        true
      );

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_stop(cmd);

    if (result != ESP_OK) {
      i2c_cmd_link_delete(cmd);
      return false;
    }


    result =
      i2c_master_cmd_begin(
        I2C_PORT,
        cmd,
        pdMS_TO_TICKS(100)
      );


    i2c_cmd_link_delete(cmd);

    return result == ESP_OK;
  }
};