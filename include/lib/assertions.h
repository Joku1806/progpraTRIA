#pragma once

#ifdef BENCH_ON

#include <Arduino.h>

// clang-format off
#define BENCHMARK(fn)                                                                              \
  do {                                                                                             \
    unsigned long bench_start = micros();                                                          \
    fn                                                                                             \
    unsigned long bench_stop = micros();                                                           \
    Serial.print("--- Benchmark for " #fn " finished, took ");                                     \
    Serial.print(bench_start <= bench_stop ? bench_stop - bench_start                              \
                                           : ULONG_MAX - bench_start + bench_stop);                \
    Serial.print("us.\n");                                                                         \
  } while (0)
// clang-format on

#else

#define BENCHMARK(fn) fn

#endif

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