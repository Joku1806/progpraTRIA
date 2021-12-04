// Inspiriert von https://github.com/SerenityOS/serenity/blob/master/Kernel/Assertions.h
#pragma once

#ifdef DEBUG

#include <Arduino.h>

#define VERIFY(expression)                                                             \
  do {                                                                                 \
    if (!(expression)) {                                                               \
      Serial.printf("ASSERTION FAILED: %s in %s:%u", #expression, __FILE__, __LINE__); \
      exit(EXIT_FAILURE)                                                               \
    }                                                                                  \
  } while (0)

#define VERIFY_NOT_REACHED() VERIFY(false)

#else

#define VERIFY(expression)
#define VERIFY_NOT_REACHED()

#endif