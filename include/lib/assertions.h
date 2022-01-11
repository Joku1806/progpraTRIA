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

#else

#define VERIFY(expression)                                                                         \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      exit(EXIT_FAILURE);                                                                          \
    }                                                                                              \
  } while (0)

#endif

#define VERIFY_NOT_REACHED() VERIFY(false)