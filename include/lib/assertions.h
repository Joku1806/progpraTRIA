#pragma once

#include <Arduino.h>

#ifdef BENCH_ON

// clang-format off
#define BENCHMARK(code)                                                                            \
  do {                                                                                             \
    unsigned long bench_start = micros();                                                          \
    code                                                                                           \
    unsigned long bench_stop = micros();                                                           \
    Serial.print("--- Benchmark for " #code " finished, took ");                                   \
    Serial.print(bench_start <= bench_stop ? bench_stop - bench_start                              \
                                           : ULONG_MAX - bench_start + bench_stop);                \
    Serial.print("us.\n");                                                                         \
  } while (0)
// clang-format on

#else

#define BENCHMARK(code) code

#endif

#ifdef DEBUG

#define VERIFY(expression)                                                                         \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      Serial.print("ASSERTION FAILED: " #expression);                                              \
      Serial.print(" in " __FILE__ ":");                                                           \
      Serial.print(__LINE__);                                                                      \
      Serial.flush();                                                                              \
                                                                                                   \
      for (;;) {                                                                                   \
        digitalWrite(LED_BUILTIN, LOW);                                                            \
        delay(500);                                                                                \
        digitalWrite(LED_BUILTIN, HIGH);                                                           \
        delay(500);                                                                                \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#else

#define VERIFY(expression)                                                                         \
  do {                                                                                             \
    if (!(expression)) {                                                                           \
      for (;;) {                                                                                   \
        digitalWrite(LED_BUILTIN, LOW);                                                            \
        delay(500);                                                                                \
        digitalWrite(LED_BUILTIN, HIGH);                                                           \
        delay(500);                                                                                \
      }                                                                                            \
    }                                                                                              \
  } while (0)

#endif

#define VERIFY_NOT_REACHED() VERIFY(false)