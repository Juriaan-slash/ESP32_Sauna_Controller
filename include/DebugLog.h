#pragma once

#include <Arduino.h>
#include <stdarg.h>
#include <stdio.h>


class DebugLog {

private:

  // Enable Serial logging for this test.
  static constexpr bool ENABLED = true;

  // Minimum amount of free TX buffer space required
  // before we attempt a Serial write.
  //
  // This is intentionally conservative for the test:
  // when the USB host is not consuming data and the
  // TX buffer becomes full, the message is simply dropped.
  static constexpr size_t MIN_TX_SPACE = 64;


  // --------------------------------------------------
  // Check whether a Serial write should be attempted
  // --------------------------------------------------

  static bool canWrite() {

    if (!ENABLED) {
      return false;
    }

    if (!Serial.isConnected()) {
      return false;
    }

    return Serial.availableForWrite() >= MIN_TX_SPACE;
  }


public:

  // --------------------------------------------------
  // Begin
  // --------------------------------------------------

  static void begin(unsigned long baud) {

    if (!ENABLED) {
      return;
    }

    Serial.begin(baud);

    // Keep the timeout low. The important part of this
    // test is that we refuse to write when there is not
    // enough TX space available.
    Serial.setTxTimeoutMs(5);
  }


  // --------------------------------------------------
  // Print
  // --------------------------------------------------

  template <typename T>
  static size_t print(const T& value) {

    if (!canWrite()) {
      return 0;
    }

    return Serial.print(value);
  }


  // --------------------------------------------------
  // Print newline
  // --------------------------------------------------

  static size_t println() {

    if (!canWrite()) {
      return 0;
    }

    return Serial.println();
  }


  // --------------------------------------------------
  // Print + newline
  // --------------------------------------------------

  template <typename T>
  static size_t println(const T& value) {

    if (!canWrite()) {
      return 0;
    }

    size_t written = Serial.print(value);
    written += Serial.println();

    return written;
  }


  // --------------------------------------------------
  // Formatted print
  // --------------------------------------------------

  static size_t printf(
    const char* format,
    ...
  ) {

    if (!ENABLED) {
      return 0;
    }

    char buffer[192];

    va_list arguments;

    va_start(arguments, format);

    const int length =
      vsnprintf(
        buffer,
        sizeof(buffer),
        format,
        arguments
      );

    va_end(arguments);


    if (length <= 0) {
      return 0;
    }


    buffer[
      sizeof(buffer) - 1
    ] = '\0';


    return print(buffer);
  }
};