// Inspiriert von https://github.com/SerenityOS/serenity/blob/master/Kernel/Assertions.h
#pragma once

#ifdef DEBUG

#include <Arduino.h>

#define VERIFY(expression)                                                                         \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      Serial.print("ASSERTION FAILED: " #expression);                                              \
      Serial.print(" in " __FILE__ ":");                                                           \
      Serial.print(__LINE__);                                                                      \
      Serial.flush();                                                                              \
      exit(EXIT_FAILURE);                                                                          \
    }                                                                                              \
  } while (0)

#define VERIFY_NOT_REACHED() VERIFY(false)

#else

#define VERIFY(expression)
#define VERIFY_NOT_REACHED()

#endif